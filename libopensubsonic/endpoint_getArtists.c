#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "../logger.h"
#include "endpoint_getArtists.h"

// Parse the JSON returned from the /rest/getArtists endpoint
// Returns 1 if failure occured, else 0
int opensubsonic_getArtists_parse(char* data, opensubsonic_getArtists_struct* getArtistsStruct) {
    // Initialize struct variables
    getArtistsStruct->status = NULL;
    getArtistsStruct->errorMessage = NULL;
    getArtistsStruct->errorCode = 0;
    getArtistsStruct->artistCount = 0;
    getArtistsStruct->lastModified = 0;
    getArtistsStruct->artists = NULL;

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
        getArtistsStruct->status = strdup(subsonic_status->valuestring);
    }

    // Check if API has returned an error
    if (strstr(getArtistsStruct->status, "ok") == NULL) {
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
            getArtistsStruct->errorCode = subsonic_error_code->valueint;
        }

        cJSON* subsonic_error_message = cJSON_GetObjectItemCaseSensitive(subsonic_error, "message");
        if (cJSON_IsString(subsonic_error_message) && subsonic_error_message->valuestring != NULL) {
            getArtistsStruct->errorMessage = strdup(subsonic_error_message->valuestring);
        }

        logger_log_error(__func__, "Error noted in JSON - Code %d: %s", getArtistsStruct->errorCode, getArtistsStruct->errorMessage);
        cJSON_Delete(root);
        return 1;
    }

    // Make an object from artists
    cJSON* artists_root = cJSON_GetObjectItemCaseSensitive(subsonic_root, "artists");
    if (artists_root == NULL) {
        logger_log_error(__func__, "Error handling JSON - artists does not exist.");
        cJSON_Delete(root);
        return 1;
    }

    // Fetch the last modified time of the artist list
    cJSON* artists_list_last_modified = cJSON_GetObjectItemCaseSensitive(artists_root, "lastModified");
    if (cJSON_IsNumber(artists_list_last_modified)) {
        getArtistsStruct->lastModified = artists_list_last_modified->valuedouble;
    }

    // Make an object from index
    cJSON* index_root = cJSON_GetObjectItemCaseSensitive(artists_root, "index");
    if (index_root == NULL) {
        logger_log_error(__func__, "Error handling JSON - index does not exist.");
        cJSON_Delete(root);
        return 1;
    }

    // Count the amount of artists present for malloc (Copied from function below)
    size_t totalArtistIndex = 0;
    for (size_t i = 0; i < cJSON_GetArraySize(index_root); i++) {
        cJSON* array_letter_root = cJSON_GetArrayItem(index_root, i);

        cJSON* current_letter_artists_array = cJSON_GetObjectItemCaseSensitive(array_letter_root, "artist");
        if (current_letter_artists_array != NULL) {
            // Array of artists starting with the same letter
            for (size_t j = 0; j < cJSON_GetArraySize(current_letter_artists_array); j++) {
                cJSON* current_letter_artist_array_layer_b = cJSON_GetArrayItem(current_letter_artists_array, j);
                if (current_letter_artist_array_layer_b != NULL) {
                    // Increment total artist index
                    totalArtistIndex++;
                }
            }
        }
    }
    getArtistsStruct->artistCount = totalArtistIndex;

    // Allocate memory for opensubsonic_getArtists_artist_struct inside opensubsonic_getArtists_struct (Where the artist data is held)
    getArtistsStruct->artists = (opensubsonic_getArtists_artist_struct*)malloc(totalArtistIndex * sizeof(opensubsonic_getArtists_artist_struct));

    // Go through the alphabet array (Each element in this array is the first letter of the artist, organized)
    size_t currentArtistIndex = 0;
    for (size_t i = 0; i < cJSON_GetArraySize(index_root); i++) {
        cJSON* array_letter_root = cJSON_GetArrayItem(index_root, i);

        cJSON* current_letter_artists_array = cJSON_GetObjectItemCaseSensitive(array_letter_root, "artist");
        if (current_letter_artists_array != NULL) {
            // Array of artists starting with the same letter
            for (size_t j = 0; j < cJSON_GetArraySize(current_letter_artists_array); j++) {
                // Initialize variables in this struct
                getArtistsStruct->artists[currentArtistIndex].name = NULL;
                getArtistsStruct->artists[currentArtistIndex].id = NULL;
                getArtistsStruct->artists[currentArtistIndex].coverArt = NULL;
                getArtistsStruct->artists[currentArtistIndex].albumCount = 0;

                cJSON* current_letter_artist_array_layer_b = cJSON_GetArrayItem(current_letter_artists_array, j);
                if (current_letter_artist_array_layer_b != NULL) {
                    // Fetch artist name, id, cover art id, and album count
                    cJSON* current_artist_name = cJSON_GetObjectItemCaseSensitive(current_letter_artist_array_layer_b, "name");
                    if (cJSON_IsString(current_artist_name) && current_artist_name->valuestring != NULL) {
                        getArtistsStruct->artists[currentArtistIndex].name = strdup(current_artist_name->valuestring);
                    }

                    cJSON* current_artist_id = cJSON_GetObjectItemCaseSensitive(current_letter_artist_array_layer_b, "id");
                    if (cJSON_IsString(current_artist_id) && current_artist_id->valuestring != NULL) {
                        getArtistsStruct->artists[currentArtistIndex].id = strdup(current_artist_id->valuestring);
                    }

                    cJSON* current_artist_cover_art_id = cJSON_GetObjectItemCaseSensitive(current_letter_artist_array_layer_b, "coverArt");
                    if (cJSON_IsString(current_artist_cover_art_id) && current_artist_cover_art_id->valuestring != NULL) {
                        getArtistsStruct->artists[currentArtistIndex].coverArt = strdup(current_artist_cover_art_id->valuestring);
                    }

                    cJSON* current_artist_album_count = cJSON_GetObjectItemCaseSensitive(current_letter_artist_array_layer_b, "albumCount");
                    if (cJSON_IsNumber(current_artist_album_count)) {
                        getArtistsStruct->artists[currentArtistIndex].albumCount = current_artist_album_count->valueint;
                    }

                    // Increment current artist index
                    currentArtistIndex++;
                }
            }
        }
    }

    cJSON_Delete(root);
    return 0;
}

// Free the dynamically allocated elements of the opensubsonic_getArtists_struct structure and the opensubsonic_getArtists_artist_struct array structs.
void opensubsonic_getArtists_struct_free(opensubsonic_getArtists_struct* getArtistsStruct) {
    logger_log_general(__func__, "Freeing /getArtists endpoint heap objects.");
    if (getArtistsStruct->status) { free(getArtistsStruct->status); }
    if (getArtistsStruct->errorMessage) { free(getArtistsStruct->errorMessage); }
    for (size_t i = 0; i < getArtistsStruct->artistCount; i++) {
        if (getArtistsStruct->artists[i].name) { free(getArtistsStruct->artists[i].name); }
        if (getArtistsStruct->artists[i].id) { free(getArtistsStruct->artists[i].id); }
        if (getArtistsStruct->artists[i].coverArt) { free(getArtistsStruct->artists[i].coverArt); }
    }
    if (getArtistsStruct->artists) { free(getArtistsStruct->artists); }
}
