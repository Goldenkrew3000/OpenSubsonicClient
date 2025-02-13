#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "endpoint_ping.h"

/*
// {"subsonic-response":{"status":"ok","version":"1.16.1","type":"navidrome","serverVersion":"0.53.1-FREEBSD (1ba390a)","openSubsonic":true}}
// {"subsonic-response":{"status":"failed","version":"1.16.1","type":"navidrome","serverVersion":"0.53.1-FREEBSD (1ba390a)","openSubsonic":true,"error":{"code":40,"message":"Wrong username or password"}}}
*/

// Parse the JSON returned from the /rest/ping endpoint
void opensubsonic_ping_parse(char* data, opensubsonic_ping_struct* pingStruct) {
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
        printf("Error parsing root in opensubsonic_ping_parse()\n");
        exit(EXIT_FAILURE);
    }

    // Make an object from subsonic-response
    cJSON* subsonic_root = cJSON_GetObjectItemCaseSensitive(root, "subsonic-response");
    if (subsonic_root == NULL) {
        printf("Error in opensubsonic_ping_parse() - subsonic-response does not exist.\n");
        cJSON_Delete(root);
        return;
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
    } else {
        pingStruct->openSubsonicCapable = false;
    }

    // Check if an error is present
    cJSON* subsonic_error = cJSON_GetObjectItemCaseSensitive(subsonic_root, "error");
    if (subsonic_error == NULL) {
        // Error did not occur, return
        cJSON_Delete(root);
        return;
    }
    pingStruct->error = true;

    cJSON* subsonic_error_code = cJSON_GetObjectItemCaseSensitive(subsonic_error, "code");
    if (cJSON_IsNumber(subsonic_error_code)) {
        pingStruct->errorCode = subsonic_error_code->valueint;
    }

    cJSON* subsonic_error_message = cJSON_GetObjectItemCaseSensitive(subsonic_error, "message");
    if (cJSON_IsString(subsonic_error_message) && subsonic_error_message->valuestring != NULL) {
        pingStruct->errorMessage = strdup(subsonic_error_message->valuestring);
    }

    cJSON_Delete(root);
}

// Free the dynamically allocated elements of the opensubsonic_ping_struct structure
void opensubsonic_ping_struct_free(opensubsonic_ping_struct* pingStruct) {
    if (pingStruct->status) { free(pingStruct->status); }
    if (pingStruct->version) { free(pingStruct->version); }
    if (pingStruct->serverType) { free(pingStruct->serverType); }
    if (pingStruct->serverVersion) { free(pingStruct->serverVersion); }
    if (pingStruct->errorMessage) { free(pingStruct->errorMessage); }
}
