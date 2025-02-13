#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "../logger.h"
#include "endpoint_ping.h"

// Parse the JSON returned from the /rest/ping endpoint
// Returns 1 if failure occured, else 0
int opensubsonic_ping_parse(char* data, opensubsonic_ping_struct* pingStruct) {
    // Initialize struct variables
    pingStruct->status = NULL;
    pingStruct->version = NULL;
    pingStruct->serverType = NULL;
    pingStruct->serverVersion = NULL;
    pingStruct->openSubsonicCapable = false;
    pingStruct->error = false;
    pingStruct->errorCode = 0;
    pingStruct->errorMessage = NULL;

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
        pingStruct->status = strdup(subsonic_status->valuestring);
    }

    cJSON* subsonic_version = cJSON_GetObjectItemCaseSensitive(subsonic_root, "version");
    if (cJSON_IsString(subsonic_version) && subsonic_version->valuestring != NULL) {
        pingStruct->version = strdup(subsonic_version->valuestring);
    }

    cJSON* subsonic_server_type = cJSON_GetObjectItemCaseSensitive(subsonic_root, "type");
    if (cJSON_IsString(subsonic_server_type) && subsonic_server_type->valuestring != NULL) {
        pingStruct->serverType = strdup(subsonic_server_type->valuestring);
    }

    cJSON* subsonic_server_version = cJSON_GetObjectItemCaseSensitive(subsonic_root, "serverVersion");
    if (cJSON_IsString(subsonic_server_version) && subsonic_server_version->valuestring != NULL) {
        pingStruct->serverVersion = strdup(subsonic_server_version->valuestring);
    }

    cJSON* subsonic_server_opensubsonic_capable = cJSON_GetObjectItemCaseSensitive(subsonic_root, "openSubsonic");
    if (cJSON_IsBool(subsonic_server_opensubsonic_capable)) {
        pingStruct->openSubsonicCapable = cJSON_IsTrue(subsonic_server_opensubsonic_capable); // TODO Verify if this works
    }

    // Check if an error is present
    cJSON* subsonic_error = cJSON_GetObjectItemCaseSensitive(subsonic_root, "error");
    if (subsonic_error == NULL) {
        // Error did not occur, return
        cJSON_Delete(root);
        return 0;
    }
    pingStruct->error = true;

    // From this point on, error has occured, capture error information
    cJSON* subsonic_error_code = cJSON_GetObjectItemCaseSensitive(subsonic_error, "code");
    if (cJSON_IsNumber(subsonic_error_code)) {
        pingStruct->errorCode = subsonic_error_code->valueint;
    }

    cJSON* subsonic_error_message = cJSON_GetObjectItemCaseSensitive(subsonic_error, "message");
    if (cJSON_IsString(subsonic_error_message) && subsonic_error_message->valuestring != NULL) {
        pingStruct->errorMessage = strdup(subsonic_error_message->valuestring);
    }

    // Print error
    logger_log_error(__func__, "Error noted in JSON - Code %d: %s", pingStruct->errorCode, pingStruct->errorMessage);

    cJSON_Delete(root);
    return 1;
}

// Free the dynamically allocated elements of the opensubsonic_ping_struct structure
void opensubsonic_ping_struct_free(opensubsonic_ping_struct* pingStruct) {
    logger_log_general(__func__, "Freeing /ping endpoint heap objects.");
    if (pingStruct->status) { free(pingStruct->status); }
    if (pingStruct->version) { free(pingStruct->version); }
    if (pingStruct->serverType) { free(pingStruct->serverType); }
    if (pingStruct->serverVersion) { free(pingStruct->serverVersion); }
    if (pingStruct->errorMessage) { free(pingStruct->errorMessage); }
}
