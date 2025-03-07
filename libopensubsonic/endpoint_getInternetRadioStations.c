/*
// libopensubsonic - /getInternetRadioStations
// Fully Implemented
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <sys/socket.h>
#include "../logger.h"
#include "endpoint_getInternetRadioStations.h"

// Parse the JSON returned from the /rest/getInternetRadioStations endpoint
// Returns 1 if failure occured, else 0
int opensubsonic_getInternetRadioStations_parse(char* data, opensubsonic_getInternetRadioStations_struct* getInternetRadioStationsStruct) {
    // Initialize struct variables
    getInternetRadioStationsStruct->status = NULL;
    getInternetRadioStationsStruct->errorCode = 0;
    getInternetRadioStationsStruct->errorMessage = NULL;
    getInternetRadioStationsStruct->internetRadioStationsCount = 0;
    getInternetRadioStationsStruct->internetRadioStations = NULL;

    // Parse the JSON
    cJSON* root = cJSON_Parse(data);
    if (root == NULL) {
        logger_log_error(__func__, "Error parsing JSON.");
        return 1;
    }

    // Make an object from subsonic-response
    cJSON* subsonic_root = cJSON_GetObjectItemCaseSensitive(root, "subsonic-response");
    if (subsonic_root == NULL) {
        logger_log_error(__func__, "Error handling JSON - subsonic-response does not exist.");
        cJSON_Delete(root);
        return 1;
    }

    cJSON* subsonic_status = cJSON_GetObjectItemCaseSensitive(subsonic_root, "status");
    if (cJSON_IsString(subsonic_status) && subsonic_status->valuestring != NULL) {
        getInternetRadioStationsStruct->status = strdup(subsonic_status->valuestring);
    }

    // Check if API has returned an error
    if (strstr(getInternetRadioStationsStruct->status, "ok") == NULL) {
        // API has not returned 'ok' in status, fetch error, and return
        // Check if an error is present
        cJSON* subsonic_error = cJSON_GetObjectItemCaseSensitive(subsonic_root, "error");
        if (subsonic_error == NULL) {
            // Error not defined in JSON
            logger_log_error(__func__, "API has indicated failure through status, but error does not exist.");
            cJSON_Delete(root);
            return 1;
        }

        cJSON* subsonic_error_code = cJSON_GetObjectItemCaseSensitive(subsonic_error, "code");
        if (cJSON_IsNumber(subsonic_error_code)) {
            getInternetRadioStationsStruct->errorCode = subsonic_error_code->valueint;
        }

        cJSON* subsonic_error_message = cJSON_GetObjectItemCaseSensitive(subsonic_error, "message");
        if (cJSON_IsString(subsonic_error_message) && subsonic_error_message->valuestring != NULL) {
            getInternetRadioStationsStruct->errorMessage = strdup(subsonic_error_message->valuestring);
        }

        logger_log_error(__func__, "Error noted in JSON - Code %d: %s", getInternetRadioStationsStruct->errorCode, getInternetRadioStationsStruct->errorMessage);
        cJSON_Delete(root);
        return 1;
    }

    // Make an object from internetRadioStations
    cJSON* internetRadioStations_root = cJSON_GetObjectItemCaseSensitive(subsonic_root, "internetRadioStations");
    if (internetRadioStations_root == NULL) {
        logger_log_error(__func__, "Error handling JSON - internetRadioStations does not exist.");
        cJSON_Delete(root);
        return 1;
    }

    // Make an object from internetRadioStation
    cJSON* internetRadioStation_root = cJSON_GetObjectItemCaseSensitive(internetRadioStations_root, "internetRadioStation");
    if (internetRadioStation_root == NULL) {
        logger_log_error(__func__, "Error handling JSON - internetRadioStation does not exist.");
        cJSON_Delete(root);
        return 1;
    }

    // Get size of array, and malloc it
    getInternetRadioStationsStruct->internetRadioStationsCount = cJSON_GetArraySize(internetRadioStation_root);
    getInternetRadioStationsStruct->internetRadioStations = (opensubsonic_getInternetRadioStations_internetRadioStation_struct*)malloc(getInternetRadioStationsStruct->internetRadioStationsCount * sizeof(opensubsonic_getInternetRadioStations_internetRadioStation_struct));

    // Go through the internet radio station array
    for (size_t i = 0; i < getInternetRadioStationsStruct->internetRadioStationsCount; i++) {
        // Initialize variables in this struct
        getInternetRadioStationsStruct->internetRadioStations[i].id = NULL;
        getInternetRadioStationsStruct->internetRadioStations[i].name = NULL;
        getInternetRadioStationsStruct->internetRadioStations[i].streamUrl = NULL;

        // Make an object from this current index in the array
        cJSON* array_internetRadioStation_root = cJSON_GetArrayItem(internetRadioStation_root, i);
        if (array_internetRadioStation_root != NULL) {
            cJSON* current_internetRadioStation_id = cJSON_GetObjectItemCaseSensitive(array_internetRadioStation_root, "id");
            if (cJSON_IsString(current_internetRadioStation_id) && current_internetRadioStation_id->valuestring != NULL) {
                getInternetRadioStationsStruct->internetRadioStations[i].id = strdup(current_internetRadioStation_id->valuestring);
            }

            cJSON* current_internetRadioStation_name = cJSON_GetObjectItemCaseSensitive(array_internetRadioStation_root, "name");
            if (cJSON_IsString(current_internetRadioStation_name) && current_internetRadioStation_name->valuestring != NULL) {
                getInternetRadioStationsStruct->internetRadioStations[i].name = strdup(current_internetRadioStation_name->valuestring);
            }

            cJSON* current_internetRadioStation_streamUrl = cJSON_GetObjectItemCaseSensitive(array_internetRadioStation_root, "streamUrl");
            if (cJSON_IsString(current_internetRadioStation_streamUrl) && current_internetRadioStation_streamUrl->valuestring != NULL) {
                getInternetRadioStationsStruct->internetRadioStations[i].streamUrl = strdup(current_internetRadioStation_streamUrl->valuestring);
            }
        }
    }

    cJSON_Delete(root);
    return 0;
}

// Free the dynamically allocated elements of the TODO structs
void opensubsonic_getInternetRadioStations_struct_free(opensubsonic_getInternetRadioStations_struct* getInternetRadioStationsStruct) {
    logger_log_general(__func__, "Freeing /getInternetRadioStations endpoint heap objects.");
    if (getInternetRadioStationsStruct->status) { free(getInternetRadioStationsStruct->status); }
    if (getInternetRadioStationsStruct->errorMessage) { free(getInternetRadioStationsStruct->errorMessage); }
    for (size_t i = 0; i < getInternetRadioStationsStruct->internetRadioStationsCount; i++) {
        if (getInternetRadioStationsStruct->internetRadioStations[i].id) { free(getInternetRadioStationsStruct->internetRadioStations[i].id); }
        if (getInternetRadioStationsStruct->internetRadioStations[i].name) { free(getInternetRadioStationsStruct->internetRadioStations[i].name); }
        if (getInternetRadioStationsStruct->internetRadioStations[i].streamUrl) { free(getInternetRadioStationsStruct->internetRadioStations[i].streamUrl); }
    }
    if (getInternetRadioStationsStruct->internetRadioStations) { free(getInternetRadioStationsStruct->internetRadioStations); }
}
