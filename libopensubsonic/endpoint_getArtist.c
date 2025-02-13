#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "../logger.h"
#include "endpoint_getArtist.h"

// Parse the JSON returned from the /rest/getArtist endpoint
// Returns 1 if failure occured, else 0
int opensubsonic_getArtist_parse(char* data, opensubsonic_getArtist_struct* getArtistStruct) {
    // Initialize struct variables
    getArtistStruct->status = NULL;
    getArtistStruct->artistId = NULL;
    getArtistStruct->artistName = NULL;
    getArtistStruct->coverArt = NULL;
    getArtistStruct->albumCount = 0;
    getArtistStruct->albums = NULL;

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

    // TODO Exit here if its not 'ok', and also for the other ones as well **************************
    cJSON* subsonic_status = cJSON_GetObjectItemCaseSensitive(subsonic_root, "status");
    if (cJSON_IsString(subsonic_status) && subsonic_status->valuestring != NULL) {
        getArtistStruct->status = strdup(subsonic_status->valuestring);
    }

    // Make an object from artist
    cJSON* artist_root = cJSON_GetObjectItemCaseSensitive(subsonic_root, "artist");
    if (artist_root == NULL) {
        logger_log_error(__func__, "Error handling JSON - artist does not exist.");
        cJSON_Delete(root);
        return 1;
    }

    // fetch id, name, coverArt, albumCount TODO
    cJSON* artist_id = cJSON_GetObjectItemCaseSensitive(artist_root, "id");
    if (cJSON_IsString) // CONTINUE LATER

    // Make an object from album
    cJSON* album_root = cJSON_GetObjectItemCaseSensitive(artist_root, "album");
    if (album_root == NULL) {
        logger_log_error(__func__, "Error handling JSON - album does not exist.");
        cJSON_Delete(root);
        return 1;
    }

    // P.S. Don't need to store the amount of elements here, already in albumCount, just start iterating over them
    // Allocate memory for opensubsonic_getArtist_album_struct inside opensubsonic_getArtist_struct
    getArtistStruct->albums = (opensubsonic_getArtist_album_struct*)malloc(cJSON_GetArraySize(album_root) * sizeof(opensubsonic_getArtist_album_struct));

    // Go through the album array
    for (size_t i = 0; i < cJSON_GetArraySize(album_root); i++) {
        cJSON* array_album_root = cJSON_GetArrayItem(album_root, i);
        if (array_album_root != NULL) {
            cJSON* current_album_id = cJSON_GetObjectItemCaseSensitive(array_album_root, "id");
            if (cJSON_IsString(current_album_id) && current_album_id->valuestring != NULL) {
                getArtistStruct->albums[i].id = strdup(current_album_id->valuestring);
            }

            // Get rest here
        }
    }

    cJSON_Delete(root);
    return 0;
}

// Free the dynamically allocated elements of the opensubsonic_getArtist_struct structure and the opensubsonic_getArtist_album_struct array structs.
void opensubsonic_getArtist_struct_free(opensubsonic_getArtist_struct* getArtistStruct) {
    //
}
