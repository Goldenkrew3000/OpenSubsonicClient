/*
// libopensubsonic - /getStarred
// Fully implemented
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <sys/socket.h>
#include "../logger.h"
#include "endpoint_getStarred.h"

// Parse the JSON returned from the /rest/getStarred endpoint
// Returns 1 if failure occured, else 0
int opensubsonic_getStarred_parse(char* data, opensubsonic_getStarred_struct* getStarredStruct) {
    // Initialize struct variables
    getStarredStruct->status = NULL;
    getStarredStruct->errorMessage = NULL;
    getStarredStruct->errorCode = 0;
    getStarredStruct->artistCount = 0;
    getStarredStruct->albumCount = 0;
    getStarredStruct->songCount = 0;
    getStarredStruct->artists = NULL;
    getStarredStruct->albums = NULL;
    getStarredStruct->songs = NULL;

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
        getStarredStruct->status = strdup(subsonic_status->valuestring);
    }

    // Check if API has returned an error
    if (strstr(getStarredStruct->status, "ok") == NULL) {
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
            getStarredStruct->errorCode = subsonic_error_code->valueint;
        }

        cJSON* subsonic_error_message = cJSON_GetObjectItemCaseSensitive(subsonic_error, "message");
        if (cJSON_IsString(subsonic_error_message) && subsonic_error_message->valuestring != NULL) {
            getStarredStruct->errorMessage = strdup(subsonic_error_message->valuestring);
        }

        logger_log_error(__func__, "Error noted in JSON - Code %d: %s", getStarredStruct->errorCode, getStarredStruct->errorMessage);
        cJSON_Delete(root);
        return 1;
    }

    // Make an object from starred
    cJSON* starred_root = cJSON_GetObjectItemCaseSensitive(subsonic_root, "starred");
    if (starred_root == NULL) {
        logger_log_error(__func__, "Error handling JSON - starred does not exist.");
        cJSON_Delete(root);
        return 1;
    }

    // Make an object from artist, album, and song
    cJSON* artist_root = cJSON_GetObjectItemCaseSensitive(starred_root, "artist");
    if (artist_root == NULL) {
        logger_log_error(__func__, "Error handling JSON - artist does not exist.");
        cJSON_Delete(root);
        return 1;
    }

    cJSON* album_root = cJSON_GetObjectItemCaseSensitive(starred_root, "album");
    if (album_root == NULL) {
        logger_log_error(__func__, "Error handling JSON - album does not exist.");
        cJSON_Delete(root);
        return 1;
    }

    cJSON* song_root = cJSON_GetObjectItemCaseSensitive(starred_root, "song");
    if (song_root == NULL) {
        logger_log_error(__func__, "Error handling JSON - song does not exist.");
        cJSON_Delete(root);
        return 1;
    }

    // Get sizes of arrays
    getStarredStruct->artistCount = cJSON_GetArraySize(artist_root);
    getStarredStruct->albumCount = cJSON_GetArraySize(album_root);
    getStarredStruct->songCount = cJSON_GetArraySize(song_root);

    // Malloc artist, album, and song structs
    getStarredStruct->artists = (opensubsonic_getStarred_artist_struct*)malloc(getStarredStruct->artistCount * sizeof(opensubsonic_getStarred_artist_struct));
    getStarredStruct->albums = (opensubsonic_getStarred_album_struct*)malloc(getStarredStruct->albumCount * sizeof(opensubsonic_getStarred_album_struct));
    getStarredStruct->songs = (opensubsonic_getStarred_song_struct*)malloc(getStarredStruct->songCount * sizeof(opensubsonic_getStarred_song_struct));

    // Go through the artist array
    for (size_t i = 0; i < getStarredStruct->artistCount; i++) {
        // Initialize variables in this struct
        getStarredStruct->artists[i].id = NULL;
        getStarredStruct->artists[i].name = NULL;
        getStarredStruct->artists[i].albumCount = 0;
        getStarredStruct->artists[i].starred = NULL;

        // Make an object from this current index in the array
        cJSON* array_artist_root = cJSON_GetArrayItem(artist_root, i);
        if (array_artist_root != NULL) {
            cJSON* current_artist_id = cJSON_GetObjectItemCaseSensitive(array_artist_root, "id");
            if (cJSON_IsString(current_artist_id) && current_artist_id->valuestring != NULL) {
                getStarredStruct->artists[i].id = strdup(current_artist_id->valuestring);
            }

            cJSON* current_artist_name = cJSON_GetObjectItemCaseSensitive(array_artist_root, "name");
            if (cJSON_IsString(current_artist_name) && current_artist_name->valuestring != NULL) {
                getStarredStruct->artists[i].name = strdup(current_artist_name->valuestring);
            }

            cJSON* current_artist_albumCount = cJSON_GetObjectItemCaseSensitive(array_artist_root, "albumCount");
            if (cJSON_IsNumber(current_artist_albumCount)) {
                getStarredStruct->artists[i].albumCount = array_artist_root->valueint;
            }

            cJSON* current_artist_starred = cJSON_GetObjectItemCaseSensitive(array_artist_root, "starred");
            if (cJSON_IsString(current_artist_starred) && current_artist_starred->valuestring != NULL) {
                getStarredStruct->artists[i].starred = strdup(current_artist_starred->valuestring);
            }
        }
    }

    // Go through the album array
    for (size_t i = 0; i < getStarredStruct->albumCount; i++) {
        // Initialize variables in this struct
        getStarredStruct->albums[i].id = NULL;
        getStarredStruct->albums[i].title = NULL;
        getStarredStruct->albums[i].name = NULL;
        getStarredStruct->albums[i].album = NULL;
        getStarredStruct->albums[i].artist = NULL;
        getStarredStruct->albums[i].year = 0;
        getStarredStruct->albums[i].genre = NULL;
        getStarredStruct->albums[i].coverArtId = NULL;
        getStarredStruct->albums[i].starred = NULL;
        getStarredStruct->albums[i].duration = 0;
        getStarredStruct->albums[i].artistId = NULL;
        getStarredStruct->albums[i].songCount = 0;

        // Make an object from this current index in the array
        cJSON* array_album_root = cJSON_GetArrayItem(album_root, i);
        if (array_album_root != NULL) {
            cJSON* current_album_id = cJSON_GetObjectItemCaseSensitive(array_album_root, "id");
            if (cJSON_IsString(current_album_id) && current_album_id->valuestring != NULL) {
                getStarredStruct->albums[i].id = strdup(current_album_id->valuestring);
            }

            cJSON* current_album_title = cJSON_GetObjectItemCaseSensitive(array_album_root, "title");
            if (cJSON_IsString(current_album_title) && current_album_title->valuestring != NULL) {
                getStarredStruct->albums[i].title = strdup(current_album_title->valuestring);
            }

            cJSON* current_album_name = cJSON_GetObjectItemCaseSensitive(array_album_root, "name");
            if (cJSON_IsString(current_album_name) && current_album_name->valuestring != NULL) {
                getStarredStruct->albums[i].name = strdup(current_album_name->valuestring);
            }

            cJSON* current_album_album = cJSON_GetObjectItemCaseSensitive(array_album_root, "album");
            if (cJSON_IsString(current_album_album) && current_album_album->valuestring != NULL) {
                getStarredStruct->albums[i].album = strdup(current_album_album->valuestring);
            }

            cJSON* current_album_artist = cJSON_GetObjectItemCaseSensitive(array_album_root, "artist");
            if (cJSON_IsString(current_album_artist) && current_album_artist->valuestring != NULL) {
                getStarredStruct->albums[i].artist = strdup(current_album_artist->valuestring);
            }

            cJSON* current_album_year = cJSON_GetObjectItemCaseSensitive(array_album_root, "year");
            if (cJSON_IsNumber(current_album_year)) {
                getStarredStruct->albums[i].year = current_album_year->valueint;
            }

            cJSON* current_album_genre = cJSON_GetObjectItemCaseSensitive(array_album_root, "genre");
            if (cJSON_IsString(current_album_genre) && current_album_genre->valuestring != NULL) {
                getStarredStruct->albums[i].genre = strdup(current_album_genre->valuestring);
            }

            cJSON* current_album_coverArtId = cJSON_GetObjectItemCaseSensitive(array_album_root, "coverArt");
            if (cJSON_IsString(current_album_coverArtId) && current_album_coverArtId->valuestring != NULL) {
                getStarredStruct->albums[i].coverArtId = strdup(current_album_coverArtId->valuestring);
            }

            cJSON* current_album_starred = cJSON_GetObjectItemCaseSensitive(array_album_root, "starred");
            if (cJSON_IsString(current_album_starred) && current_album_starred->valuestring != NULL) {
                getStarredStruct->albums[i].starred = strdup(current_album_starred->valuestring);
            }

            cJSON* current_album_duration = cJSON_GetObjectItemCaseSensitive(array_album_root, "duration");
            if (cJSON_IsNumber(current_album_duration)) {
                getStarredStruct->albums[i].duration = current_album_duration->valuedouble;
            }

            cJSON* current_album_artistId = cJSON_GetObjectItemCaseSensitive(array_album_root, "artistId");
            if (cJSON_IsString(current_album_artistId) && current_album_artistId->valuestring != NULL) {
                getStarredStruct->albums[i].artistId = strdup(current_album_artistId->valuestring);
            }

            cJSON* current_album_songCount = cJSON_GetObjectItemCaseSensitive(array_album_root, "songCount");
            if (cJSON_IsNumber(current_album_songCount)) {
                getStarredStruct->albums[i].songCount = current_album_songCount->valueint;
            }
        }
    }

    // Go through the song array
    for (size_t i = 0; i < getStarredStruct->songCount; i++) {
        // Initialize variables in this struct
        getStarredStruct->songs[i].id = NULL;
        getStarredStruct->songs[i].title = NULL;
        getStarredStruct->songs[i].album = NULL;
        getStarredStruct->songs[i].artist = NULL;
        getStarredStruct->songs[i].track = 0;
        getStarredStruct->songs[i].year = 0;
        getStarredStruct->songs[i].genre = NULL;
        getStarredStruct->songs[i].coverArtId = NULL;
        getStarredStruct->songs[i].size = 0;
        getStarredStruct->songs[i].starred = NULL;
        getStarredStruct->songs[i].duration = 0;
        getStarredStruct->songs[i].bitRate = 0;
        getStarredStruct->songs[i].playCount = 0;
        getStarredStruct->songs[i].albumId = NULL;
        getStarredStruct->songs[i].artistId = NULL;
        getStarredStruct->songs[i].channelCount = 0;
        getStarredStruct->songs[i].sampleRate = 0;

        // Make an object from this current index in the array
        cJSON* array_song_root = cJSON_GetArrayItem(song_root, i);
        if (array_song_root != NULL) {
            cJSON* current_song_id = cJSON_GetObjectItemCaseSensitive(array_song_root, "id");
            if (cJSON_IsString(current_song_id) && current_song_id->valuestring != NULL) {
                getStarredStruct->songs[i].id = strdup(current_song_id->valuestring);
            }

            cJSON* current_song_title = cJSON_GetObjectItemCaseSensitive(array_song_root, "title");
            if (cJSON_IsString(current_song_title) && current_song_title->valuestring != NULL) {
                getStarredStruct->songs[i].title = strdup(current_song_title->valuestring);
            }

            cJSON* current_song_album = cJSON_GetObjectItemCaseSensitive(array_song_root, "album");
            if (cJSON_IsString(current_song_album) && current_song_album->valuestring != NULL) {
                getStarredStruct->songs[i].album = strdup(current_song_album->valuestring);
            }

            cJSON* current_song_artist = cJSON_GetObjectItemCaseSensitive(array_song_root, "artist");
            if (cJSON_IsString(current_song_artist) && current_song_artist->valuestring != NULL) {
                getStarredStruct->songs[i].artist = strdup(current_song_artist->valuestring);
            }

            cJSON* current_song_track = cJSON_GetObjectItemCaseSensitive(array_song_root, "track");
            if (cJSON_IsNumber(current_song_track)) {
                getStarredStruct->songs[i].track = current_song_track->valueint;
            }

            cJSON* current_song_year = cJSON_GetObjectItemCaseSensitive(array_song_root, "year");
            if (cJSON_IsNumber(current_song_year)) {
                getStarredStruct->songs[i].year = current_song_year->valueint;
            }

            cJSON* current_song_genre = cJSON_GetObjectItemCaseSensitive(array_song_root, "genre");
            if (cJSON_IsString(current_song_genre) && current_song_genre->valuestring != NULL) {
                getStarredStruct->songs[i].genre = strdup(current_song_genre->valuestring);
            }

            cJSON* current_song_coverArtId = cJSON_GetObjectItemCaseSensitive(array_song_root, "coverArt");
            if (cJSON_IsString(current_song_coverArtId) && current_song_coverArtId->valuestring != NULL) {
                getStarredStruct->songs[i].coverArtId = strdup(current_song_coverArtId->valuestring);
            }

            cJSON* current_song_size = cJSON_GetObjectItemCaseSensitive(array_song_root, "size");
            if (cJSON_IsNumber(current_song_size)) {
                getStarredStruct->songs[i].size = current_song_size->valuedouble;
            }

            cJSON* current_song_starred = cJSON_GetObjectItemCaseSensitive(array_song_root, "starred");
            if (cJSON_IsString(current_song_starred) && current_song_starred->valuestring != NULL) {
                getStarredStruct->songs[i].starred = strdup(current_song_starred->valuestring);
            }

            cJSON* current_song_duration = cJSON_GetObjectItemCaseSensitive(array_song_root, "duration");
            if (cJSON_IsNumber(current_song_duration)) {
                getStarredStruct->songs[i].duration = current_song_duration->valuedouble;
            }

            cJSON* current_song_bitrate = cJSON_GetObjectItemCaseSensitive(array_song_root, "bitRate");
            if (cJSON_IsNumber(current_song_bitrate)) {
                getStarredStruct->songs[i].bitRate = current_song_bitrate->valueint;
            }

            cJSON* current_song_playCount = cJSON_GetObjectItemCaseSensitive(array_song_root, "playCount");
            if (cJSON_IsNumber(current_song_playCount)) {
                getStarredStruct->songs[i].playCount = current_song_playCount->valueint;
            }

            cJSON* current_song_albumId = cJSON_GetObjectItemCaseSensitive(array_song_root, "albumId");
            if (cJSON_IsString(current_song_albumId) && current_song_albumId->valuestring != NULL) {
                getStarredStruct->songs[i].albumId = strdup(current_song_albumId->valuestring);
            }

            cJSON* current_song_artistId = cJSON_GetObjectItemCaseSensitive(array_song_root, "artistId");
            if (cJSON_IsString(current_song_artistId) && current_song_artistId->valuestring != NULL) {
                getStarredStruct->songs[i].artistId = strdup(current_song_artistId->valuestring);
            }

            cJSON* current_song_channelCount = cJSON_GetObjectItemCaseSensitive(array_song_root, "channelCount");
            if (cJSON_IsNumber(current_song_channelCount)) {
                getStarredStruct->songs[i].channelCount = current_song_channelCount->valueint;
            }

            cJSON* current_song_sampleRate = cJSON_GetObjectItemCaseSensitive(array_song_root, "samplingRate");
            if (cJSON_IsNumber(current_song_sampleRate)) {
                getStarredStruct->songs[i].sampleRate = current_song_sampleRate->valueint;
            }
        }
    }

    cJSON_Delete(root);
    return 0;
}

// Free the dynamically allocated elements of the opensubsonic_getStarred_struct, opensubsonic_getStarred_artist_struct, opensubsonic_getStarred_album_struct, and opensubsonic_getStarred_song_struct structs
void opensubsonic_getStarred_struct_free(opensubsonic_getStarred_struct* getStarredStruct) {
    logger_log_general(__func__, "Freeing /getStarred endpoint heap objects.");
    if (getStarredStruct->status) { free(getStarredStruct->status); }
    if (getStarredStruct->errorMessage) { free(getStarredStruct->errorMessage); }
    for (size_t i = 0; i < getStarredStruct->artistCount; i++) {
        if (getStarredStruct->artists[i].id) { free(getStarredStruct->artists[i].id); }
        if (getStarredStruct->artists[i].name) { free(getStarredStruct->artists[i].name); }
        if (getStarredStruct->artists[i].starred) { free(getStarredStruct->artists[i].starred); }
    }
    for (size_t i = 0; i < getStarredStruct->albumCount; i++) {
        if (getStarredStruct->albums[i].id) { free(getStarredStruct->albums[i].id); }
        if (getStarredStruct->albums[i].title) { free(getStarredStruct->albums[i].title); }
        if (getStarredStruct->albums[i].name) { free(getStarredStruct->albums[i].name); }
        if (getStarredStruct->albums[i].album) { free(getStarredStruct->albums[i].album); }
        if (getStarredStruct->albums[i].artist) { free(getStarredStruct->albums[i].artist); }
        if (getStarredStruct->albums[i].genre) { free(getStarredStruct->albums[i].genre); }
        if (getStarredStruct->albums[i].coverArtId) { free(getStarredStruct->albums[i].coverArtId); }
        if (getStarredStruct->albums[i].starred) { free(getStarredStruct->albums[i].starred); }
        if (getStarredStruct->albums[i].artistId) { free(getStarredStruct->albums[i].artistId); }
    }
    for (size_t i = 0; i < getStarredStruct->songCount; i++) {
        if (getStarredStruct->songs[i].id) { free(getStarredStruct->songs[i].id); }
        if (getStarredStruct->songs[i].title) { free(getStarredStruct->songs[i].title); }
        if (getStarredStruct->songs[i].album) { free(getStarredStruct->songs[i].album); }
        if (getStarredStruct->songs[i].artist) { free(getStarredStruct->songs[i].artist); }
        if (getStarredStruct->songs[i].genre) { free(getStarredStruct->songs[i].genre); }
        if (getStarredStruct->songs[i].coverArtId) { free(getStarredStruct->songs[i].coverArtId); }
        if (getStarredStruct->songs[i].starred) { free(getStarredStruct->songs[i].starred); }
        if (getStarredStruct->songs[i].albumId) { free(getStarredStruct->songs[i].albumId); }
        if (getStarredStruct->songs[i].artistId) { free(getStarredStruct->songs[i].artistId); }
    }
    if (getStarredStruct->artists) { free(getStarredStruct->artists); }
    if (getStarredStruct->albums) { free(getStarredStruct->albums); }
    if (getStarredStruct->songs) { free(getStarredStruct->songs); }
}
