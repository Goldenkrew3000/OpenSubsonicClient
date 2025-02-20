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
    getArtistStruct->errorMessage = NULL;
    getArtistStruct->errorCode = 0;
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

    cJSON* subsonic_status = cJSON_GetObjectItemCaseSensitive(subsonic_root, "status");
    if (cJSON_IsString(subsonic_status) && subsonic_status->valuestring != NULL) {
        getArtistStruct->status = strdup(subsonic_status->valuestring);
    }

    // Check if API has returned an error
    if (strstr(getArtistStruct->status, "ok") == NULL) {
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
            getArtistStruct->errorCode = subsonic_error_code->valueint;
        }

        cJSON* subsonic_error_message = cJSON_GetObjectItemCaseSensitive(subsonic_error, "message");
        if (cJSON_IsString(subsonic_error_message) && subsonic_error_message->valuestring != NULL) {
            getArtistStruct->errorMessage = strdup(subsonic_error_message->valuestring);
        }

        logger_log_error(__func__, "Error noted in JSON - Code %d: %s", getArtistStruct->errorCode, getArtistStruct->errorMessage);
        cJSON_Delete(root);
        return 1;
    }

    // Make an object from artist
    cJSON* artist_root = cJSON_GetObjectItemCaseSensitive(subsonic_root, "artist");
    if (artist_root == NULL) {
        logger_log_error(__func__, "Error handling JSON - artist does not exist.");
        cJSON_Delete(root);
        return 1;
    }

    cJSON* artist_id = cJSON_GetObjectItemCaseSensitive(artist_root, "id");
    if (cJSON_IsString(artist_id) && artist_id->valuestring != NULL) {
        getArtistStruct->artistId = strdup(artist_id->valuestring);
    }

    cJSON* artist_name = cJSON_GetObjectItemCaseSensitive(artist_root, "name");
    if (cJSON_IsString(artist_name) && artist_name->valuestring != NULL) {
        getArtistStruct->artistName = strdup(artist_name->valuestring);
    }

    cJSON* artist_cover_art = cJSON_GetObjectItemCaseSensitive(artist_root, "coverArt");
    if (cJSON_IsString(artist_cover_art) && artist_cover_art->valuestring != NULL) {
        getArtistStruct->coverArt = strdup(artist_cover_art->valuestring);
    }

    cJSON* artist_album_count = cJSON_GetObjectItemCaseSensitive(artist_root, "albumCount");
    if (cJSON_IsNumber(artist_album_count)) {
        getArtistStruct->albumCount = artist_album_count->valueint;
    }

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
        // Initialize variables in this struct
        getArtistStruct->albums[i].id = NULL;
        getArtistStruct->albums[i].title = NULL;
        getArtistStruct->albums[i].name = NULL;
        getArtistStruct->albums[i].album = NULL;
        getArtistStruct->albums[i].artist = NULL;
        getArtistStruct->albums[i].yearReleased = 0;
        getArtistStruct->albums[i].genre = NULL;
        getArtistStruct->albums[i].coverArt = NULL;
        getArtistStruct->albums[i].duration = 0;
        getArtistStruct->albums[i].playCount = 0;
        getArtistStruct->albums[i].dateAdded = NULL;
        getArtistStruct->albums[i].songCount = 0;

        cJSON* array_album_root = cJSON_GetArrayItem(album_root, i);
        if (array_album_root != NULL) {
            cJSON* current_album_id = cJSON_GetObjectItemCaseSensitive(array_album_root, "id");
            if (cJSON_IsString(current_album_id) && current_album_id->valuestring != NULL) {
                getArtistStruct->albums[i].id = strdup(current_album_id->valuestring);
            }

            cJSON* current_album_title = cJSON_GetObjectItemCaseSensitive(array_album_root, "title");
            if (cJSON_IsString(current_album_title) && current_album_title->valuestring != NULL) {
                getArtistStruct->albums[i].title = strdup(current_album_title->valuestring);
            }

            // I _THINK_ the next two objects aren't needed as they are the same as current_album_title, but I have them here because it's like 20 extra bytes of memory and if I need them, they are here
            cJSON* current_album_name = cJSON_GetObjectItemCaseSensitive(array_album_root, "name");
            if (cJSON_IsString(current_album_name) && current_album_name->valuestring != NULL) {
                getArtistStruct->albums[i].name = strdup(current_album_name->valuestring);
            }

            cJSON* current_album_album = cJSON_GetObjectItemCaseSensitive(array_album_root, "album");
            if (cJSON_IsString(current_album_album) && current_album_album->valuestring != NULL) {
                getArtistStruct->albums[i].album = strdup(current_album_album->valuestring);
            }

            cJSON* current_album_artist = cJSON_GetObjectItemCaseSensitive(array_album_root, "artist");
            if (cJSON_IsString(current_album_artist) && current_album_artist->valuestring != NULL) {
                getArtistStruct->albums[i].artist = strdup(current_album_artist->valuestring);
            }

            cJSON* current_album_year = cJSON_GetObjectItemCaseSensitive(array_album_root, "year");
            if (cJSON_IsNumber(current_album_year)) {
                getArtistStruct->albums[i].yearReleased = current_album_year->valueint;
            }

            cJSON* current_album_genre = cJSON_GetObjectItemCaseSensitive(array_album_root, "genre");
            if (cJSON_IsString(current_album_genre) && current_album_genre->valuestring != NULL) {
                getArtistStruct->albums[i].genre = strdup(current_album_genre->valuestring);
            }

            cJSON* current_album_cover_art = cJSON_GetObjectItemCaseSensitive(array_album_root, "coverArt");
            if (cJSON_IsString(current_album_cover_art) && current_album_cover_art->valuestring != NULL) {
                getArtistStruct->albums[i].coverArt = strdup(current_album_cover_art->valuestring);
            }

            cJSON* current_album_duration = cJSON_GetObjectItemCaseSensitive(array_album_root, "duration");
            if (cJSON_IsNumber(current_album_duration)) {
                getArtistStruct->albums[i].duration = current_album_duration->valuedouble;
            }

            cJSON* current_album_play_count = cJSON_GetObjectItemCaseSensitive(array_album_root, "playCount");
            if (cJSON_IsNumber(current_album_play_count)) {
                getArtistStruct->albums[i].playCount = current_album_play_count->valueint;
            }

            cJSON* current_album_date_added = cJSON_GetObjectItemCaseSensitive(array_album_root, "created");
            if (cJSON_IsString(current_album_date_added) && current_album_date_added->valuestring != NULL) {
                getArtistStruct->albums[i].dateAdded = strdup(current_album_date_added->valuestring);
            }

            cJSON* current_album_song_count = cJSON_GetObjectItemCaseSensitive(array_album_root, "songCount");
            if (cJSON_IsNumber(current_album_song_count)) {
                getArtistStruct->albums[i].songCount = current_album_song_count->valueint;
            }
        }
    }

    cJSON_Delete(root);
    return 0;
}

// Free the dynamically allocated elements of the opensubsonic_getArtist_struct structure and the opensubsonic_getArtist_album_struct array structs.
void opensubsonic_getArtist_struct_free(opensubsonic_getArtist_struct* getArtistStruct) {
    logger_log_general(__func__, "Freeing /getArtist endpoint heap objects.");
    if (getArtistStruct->status) { free(getArtistStruct->status); }
    if (getArtistStruct->errorMessage) { free(getArtistStruct->errorMessage); }
    if (getArtistStruct->artistId) { free(getArtistStruct->artistId); }
    if (getArtistStruct->artistName) { free(getArtistStruct->artistName); }
    if (getArtistStruct->coverArt) { free(getArtistStruct->coverArt); }
    for (size_t i = 0; i < getArtistStruct->albumCount; i++) {
        if (getArtistStruct->albums[i].id) { free(getArtistStruct->albums[i].id); }
        if (getArtistStruct->albums[i].title) { free(getArtistStruct->albums[i].title); }
        if (getArtistStruct->albums[i].name) { free(getArtistStruct->albums[i].name); }
        if (getArtistStruct->albums[i].album) { free(getArtistStruct->albums[i].album); }
        if (getArtistStruct->albums[i].artist) { free(getArtistStruct->albums[i].artist); }
        if (getArtistStruct->albums[i].genre) { free(getArtistStruct->albums[i].genre); }
        if (getArtistStruct->albums[i].coverArt) { free(getArtistStruct->albums[i].coverArt); }
        if (getArtistStruct->albums[i].dateAdded) { free(getArtistStruct->albums[i].dateAdded); }
    }
    if (getArtistStruct->albums) { free(getArtistStruct->albums); }
}
