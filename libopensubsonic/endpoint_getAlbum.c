#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "../logger.h"
#include "endpoint_getAlbum.h"

// Parse the JSON returned from the /rest/getAlbum endpoint
// Returns 1 if failure occured, else 0
int opensubsonic_getAlbum_parse(char* data, opensubsonic_getAlbum_struct* getAlbumStruct) {
    // Initialize struct variables
    getAlbumStruct->status = NULL;
    getAlbumStruct->errorCode = 0;
    getAlbumStruct->errorMessage = NULL;
    getAlbumStruct->albumId = NULL;
    getAlbumStruct->albumName = NULL;
    getAlbumStruct->albumArtist = NULL;
    getAlbumStruct->albumArtistId = NULL;
    getAlbumStruct->coverArtId = NULL;
    getAlbumStruct->songCount = 0;
    getAlbumStruct->duration = 0;
    getAlbumStruct->playCount = 0;
    getAlbumStruct->yearReleased = 0;
    getAlbumStruct->genre = NULL;
    getAlbumStruct->songs = NULL;

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
        getAlbumStruct->status = strdup(subsonic_status->valuestring);
    }

    // Check if API has returned an error
    if (strstr(getAlbumStruct->status, "ok") == NULL) {
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
            getAlbumStruct->errorCode = subsonic_error_code->valueint;
        }

        cJSON* subsonic_error_message = cJSON_GetObjectItemCaseSensitive(subsonic_error, "message");
        if (cJSON_IsString(subsonic_error_message) && subsonic_error_message->valuestring != NULL) {
            getAlbumStruct->errorMessage = strdup(subsonic_error_message->valuestring);
        }

        logger_log_error(__func__, "Error noted in JSON - Code %d: %s", getAlbumStruct->errorCode, getAlbumStruct->errorMessage);
        cJSON_Delete(root);
        return 1;
    }

    // Make an object from album
    cJSON* album_root = cJSON_GetObjectItemCaseSensitive(subsonic_root, "album");
    if (album_root == NULL) {
        logger_log_error(__func__, "Error handling JSON - album does not exist.");
        cJSON_Delete(root);
        return 1;
    }

    cJSON* album_id = cJSON_GetObjectItemCaseSensitive(album_root, "id");
    if (cJSON_IsString(album_id) && album_id->valuestring != NULL) {
        getAlbumStruct->albumId = strdup(album_id->valuestring);
    }

    cJSON* album_name = cJSON_GetObjectItemCaseSensitive(album_root, "name");
    if (cJSON_IsString(album_name) && album_name->valuestring != NULL) {
        getAlbumStruct->albumName = strdup(album_name->valuestring);
    }

    cJSON* album_artist = cJSON_GetObjectItemCaseSensitive(album_root, "artist");
    if (cJSON_IsString(album_artist) && album_artist->valuestring != NULL) {
        getAlbumStruct->albumArtist = strdup(album_artist->valuestring);
    }

    cJSON* album_artist_id = cJSON_GetObjectItemCaseSensitive(album_root, "artistId");
    if (cJSON_IsString(album_artist_id) && album_artist_id->valuestring != NULL) {
        getAlbumStruct->albumArtistId = strdup(album_artist_id->valuestring);
    }

    cJSON* album_cover_art_id = cJSON_GetObjectItemCaseSensitive(album_root, "coverArt");
    if (cJSON_IsString(album_cover_art_id) && album_cover_art_id->valuestring != NULL) {
        getAlbumStruct->coverArtId = strdup(album_cover_art_id->valuestring);
    }

    cJSON* album_duration = cJSON_GetObjectItemCaseSensitive(album_root, "duration");
    if (cJSON_IsNumber(album_duration)) {
        getAlbumStruct->duration = album_duration->valuedouble;
    }

    cJSON* album_play_count = cJSON_GetObjectItemCaseSensitive(album_root, "playCount");
    if (cJSON_IsNumber(album_play_count)) {
        getAlbumStruct->playCount = album_play_count->valueint;
    }

    cJSON* album_year_released = cJSON_GetObjectItemCaseSensitive(album_root, "year");
    if (cJSON_IsNumber(album_year_released)) {
        getAlbumStruct->yearReleased = album_year_released->valueint;
    }

    // Make an object from song
    cJSON* song_root = cJSON_GetObjectItemCaseSensitive(album_root, "song");
    if (song_root == NULL) {
        logger_log_error(__func__, "Error handling JSON - song does not exist.");
        cJSON_Delete(root);
        return 1;
    }

    // Get the amount of songs for malloc
    // NOTE: In 'album' (one layer up), there is the 'songCount' attribute, but I have found (specifically here, hours of debugging)
    // that the amount of songs in an album does not always match the 'songCount' attribute, leading to malloc issues.
    // So the current fix is to count them manually instead.
    getAlbumStruct->songCount = cJSON_GetArraySize(song_root);

    // Don't need to count songs because it's already stored in songCount
    getAlbumStruct->songs = (opensubsonic_getAlbum_songs_struct*)malloc(cJSON_GetArraySize(song_root) * sizeof(opensubsonic_getAlbum_songs_struct));

    // Go through the song array
    for (size_t i = 0; i < cJSON_GetArraySize(song_root); i++) {
        // Initialize varibables in this struct
        getAlbumStruct->songs[i].id = NULL;
        getAlbumStruct->songs[i].title = NULL;
        getAlbumStruct->songs[i].track = 0;
        getAlbumStruct->songs[i].coverArt = NULL;
        getAlbumStruct->songs[i].filesize = 0;
        getAlbumStruct->songs[i].duration = 0;
        getAlbumStruct->songs[i].playCount = 0;

        cJSON* array_song_root = cJSON_GetArrayItem(song_root, i);
        if (array_song_root != NULL) {
            cJSON* current_song_id = cJSON_GetObjectItemCaseSensitive(array_song_root, "id");
            if (cJSON_IsString(current_song_id) && current_song_id->valuestring != NULL) {
                getAlbumStruct->songs[i].id = strdup(current_song_id->valuestring);
            }

            cJSON* current_song_title = cJSON_GetObjectItemCaseSensitive(array_song_root, "title");
            if (cJSON_IsString(current_song_title) && current_song_title->valuestring != NULL) {
                getAlbumStruct->songs[i].title = strdup(current_song_title->valuestring);
            }

            cJSON* current_song_track = cJSON_GetObjectItemCaseSensitive(array_song_root, "track");
            if (cJSON_IsNumber(current_song_track)) {
                getAlbumStruct->songs[i].track = current_song_title->valueint;
            }

            cJSON* current_song_cover_art = cJSON_GetObjectItemCaseSensitive(array_song_root, "coverArt");
            if (cJSON_IsString(current_song_cover_art) && current_song_cover_art->valuestring != NULL) {
                getAlbumStruct->songs[i].coverArt = strdup(current_song_cover_art->valuestring);
            }

            cJSON* current_song_filesize = cJSON_GetObjectItemCaseSensitive(array_song_root, "size");
            if (cJSON_IsNumber(current_song_filesize)) {
                getAlbumStruct->songs[i].filesize = current_song_filesize->valuedouble;
            }

            cJSON* current_song_duration = cJSON_GetObjectItemCaseSensitive(array_song_root, "duration");
            if (cJSON_IsNumber(current_song_duration)) {
                getAlbumStruct->songs[i].duration = current_song_duration->valueint;
            }

            cJSON* current_song_play_count = cJSON_GetObjectItemCaseSensitive(array_song_root, "playCount");
            if (cJSON_IsNumber(current_song_play_count)) {
                getAlbumStruct->songs[i].playCount = current_song_play_count->valueint;
            }
        }
    }

    cJSON_Delete(root);
    return 0;
}

void opensubsonic_getAlbum_struct_free(opensubsonic_getAlbum_struct* getAlbumStruct) {
    logger_log_general(__func__, "Freeing /getAlbum endpoint heap objects.");
    if (getAlbumStruct->status) { free(getAlbumStruct->status); }
    if (getAlbumStruct->errorMessage) { free(getAlbumStruct->errorMessage); }
    if (getAlbumStruct->albumId) { free(getAlbumStruct->albumId); }
    if (getAlbumStruct->albumName) { free(getAlbumStruct->albumName); }
    if (getAlbumStruct->albumArtist) { free(getAlbumStruct->albumArtist); }
    if (getAlbumStruct->albumArtistId) { free(getAlbumStruct->albumArtistId); }
    if (getAlbumStruct->coverArtId) { free(getAlbumStruct->coverArtId); }
    for (size_t i = 0; i < getAlbumStruct->songCount; i++) {
        if (getAlbumStruct->songs[i].id) { free(getAlbumStruct->songs[i].id); }
        if (getAlbumStruct->songs[i].title) { free(getAlbumStruct->songs[i].title); }
        if (getAlbumStruct->songs[i].coverArt) { free(getAlbumStruct->songs[i].coverArt); }
    }
    if (getAlbumStruct->songs) { free(getAlbumStruct->songs); }
}
