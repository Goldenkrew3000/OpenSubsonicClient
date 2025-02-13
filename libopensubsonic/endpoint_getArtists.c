#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "endpoint_getArtists.h"

// Parse the JSON returned from the /rest/getArtists endpoint
void opensubsonic_getArtists_parse(char* data, opensubsonic_getArtists_struct* getArtistsStruct) {
    // Initialize struct variables
    getArtistsStruct->status = NULL;
    getArtistsStruct->artistCount = 0;
    getArtistsStruct->lastModified = 0;
    getArtistsStruct->artists = NULL;

    // Parse the JSON
    cJSON* root = cJSON_Parse(data);
    if (root == NULL) {
        printf("Error parsing root in opensubsonic_getArtists_parse()\n");
        exit(EXIT_FAILURE);
    }

    // Make an object from subsonic-response
    cJSON* subsonic_root = cJSON_GetObjectItemCaseSensitive(root, "subsonic-response");
    if (subsonic_root == NULL) {
        printf("Error in opensubsonic_getArtists_parse() - subsonic-response does not exist.\n");
        cJSON_Delete(root);
        return;
    }

    cJSON* subsonic_status = cJSON_GetObjectItemCaseSensitive(subsonic_root, "status");
    if (cJSON_IsString(subsonic_status) && subsonic_status->valuestring != NULL) {
        getArtistsStruct->status = strdup(subsonic_status->valuestring);
    }

    // Make an object from artists
    cJSON* artists_root = cJSON_GetObjectItemCaseSensitive(subsonic_root, "artists");
    if (artists_root == NULL) {
        printf("Error in opensubsonic_getArtists_parse() - artists does not exist.\n");
        cJSON_Delete(root);
        return;
    }

    // Fetch the last modified time of the artist list
    cJSON* artists_list_last_modified = cJSON_GetObjectItemCaseSensitive(artists_root, "lastModified");
    if (cJSON_IsNumber(artists_list_last_modified)) {
        getArtistsStruct->lastModified = artists_list_last_modified->valuedouble;
    }

    // Make an object from index
    cJSON* index_root = cJSON_GetObjectItemCaseSensitive(artists_root, "index");
    if (index_root == NULL) {
        printf("Error in opensubsonic_getArtists_parse() - index does not exist.\n");
        cJSON_Delete(root);
        return;
    }

    // Get the amount of artists present
    getArtistsStruct->artistCount = cJSON_GetArraySize(index_root);

    // Allocate memory for opensubsonic_getArtists_artist_struct inside opensubsonic_getArtists_struct (Where the artist data is held)
    getArtistsStruct->artists = (opensubsonic_getArtists_artist_struct*)malloc(cJSON_GetArraySize(index_root) * sizeof(opensubsonic_getArtists_artist_struct));

    // Go through the alphabet array (Each element in this array is the first letter of the artist, organized)
    size_t currentArtistIndex = 0;
    for (size_t i = 0; i < cJSON_GetArraySize(index_root); i++) {
        cJSON* array_letter_root = cJSON_GetArrayItem(index_root, i);

        cJSON* current_letter_artists_array = cJSON_GetObjectItemCaseSensitive(array_letter_root, "artist");
        if (current_letter_artists_array != NULL) {
            // Array of artists starting with the same letter
            for (size_t j = 0; j < cJSON_GetArraySize(current_letter_artists_array); j++) {
                cJSON* current_letter_artist_array_layer_b = cJSON_GetArrayItem(current_letter_artists_array, j);
                if (current_letter_artist_array_layer_b != NULL) {
                    // Initialize variables in this struct
                    getArtistsStruct->artists[currentArtistIndex].name = NULL;
                    getArtistsStruct->artists[currentArtistIndex].id = NULL;
                    getArtistsStruct->artists[currentArtistIndex].coverArt = NULL;
                    getArtistsStruct->artists[currentArtistIndex].albumCount = 0;

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
}

// Free the dynamically allocated elements of the opensubsonic_getArtists_struct structure and the opensubsonic_getArtists_artist_struct array structs.
void opensubsonic_getArtists_struct_free(opensubsonic_getArtists_struct* getArtistsStruct) {
    if (getArtistsStruct->status) { free(getArtistsStruct->status); }
    for (size_t i = 0; i < getArtistsStruct->artistCount; i++) {
        if (getArtistsStruct->artists[i].name) { free(getArtistsStruct->artists[i].name); }
        if (getArtistsStruct->artists[i].id) { free(getArtistsStruct->artists[i].id); }
        if (getArtistsStruct->artists[i].coverArt) { free(getArtistsStruct->artists[i].coverArt); }
    }
    if (getArtistsStruct->artists) { free(getArtistsStruct->artists); }
}
