#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "httpclient.h"
#include "../logger.h"

CURL* curl_handle;
CURLcode res;


////////////////////////////////////////////////////////////////////////////////////////////////
struct MemoryStruct {
    char *memory; // Malloc'd memory
    size_t size; // Size in bytes
};

// Callback function to handle the received data
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    // Resize the memory block to accommodate the new data
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;  // Null-terminate the string

    return realsize;
}
////////////////////////////////////////////////////////////////////////////////////////////////



// Contact the /rest/ping endpoint to test login credentials
int opensubsonic_ping(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char** response) {
    // Generate full URL (All variables here have a maximum size of 165 bytes, without padding), allocate 256 bytes, perform HTTP GET, and free URL
    int rc = 0;
    char* full_url = malloc(256);
    snprintf(full_url, 256, "%s://%s/rest/ping?u=%s&t=%s&s=%s&f=json&v=%s&c=%s", protocol_ptr, server_ptr, user_ptr, login_token_ptr, login_salt_ptr, opensubsonic_version_ptr, client_name_ptr);
    rc = opensubsonic_http_json_get(full_url, response);
    free(full_url);
    return rc;
}

// Contact the /rest/getArtists endpoint
int opensubsonic_getArtists(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char** response) {
    // Generate full URL, perform HTTP GET, and free the full URL
    int rc = 0;
    char* full_url = malloc(256);
    snprintf(full_url, 256, "%s://%s/rest/getArtists?u=%s&t=%s&s=%s&f=json&v=%s&c=%s", protocol_ptr, server_ptr, user_ptr, login_token_ptr, login_salt_ptr, opensubsonic_version_ptr, client_name_ptr);
    rc = opensubsonic_http_json_get(full_url, response);
    free(full_url);
    return rc;
}

// Contact the /rest/getArtist endpoint
int opensubsonic_getArtist(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char* id, char** response) {
    // Generate full URL, perform HTTP GET, and free the full URL
    int rc = 0;
    char* full_url = malloc(256);
    snprintf(full_url, 256, "%s://%s/rest/getArtist?u=%s&t=%s&s=%s&f=json&v=%s&c=%s&id=%s", protocol_ptr, server_ptr, user_ptr, login_token_ptr, login_salt_ptr, opensubsonic_version_ptr, client_name_ptr, id);
    rc = opensubsonic_http_json_get(full_url, response);
    free(full_url);
    return rc;
}

// Contact the /rest/getAlbum endpoint
int opensubsonic_getAlbum(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char* id, char** response) {
    // Generate full URL, perform HTTP GET, and free the full URL
    int rc = 0;
    char* full_url = malloc(256);
    snprintf(full_url, 256, "%s://%s/rest/getAlbum?u=%s&t=%s&s=%s&f=json&v=%s&c=%s&id=%s", protocol_ptr, server_ptr, user_ptr, login_token_ptr, login_salt_ptr, opensubsonic_version_ptr, client_name_ptr, id);
    rc = opensubsonic_http_json_get(full_url, response);
    free(full_url);
    return rc;
}


// Contact the /rest/getLyricsBySongId endpoint
int opensubsonic_getLyricsBySongId(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char* id, char** response) {
    // Generate full URL, perform HTTP GET, and free the full URL
    int rc = 0;
    char* full_url = malloc(256);
    snprintf(full_url, 256, "%s://%s/rest/getLyricsBySongId?u=%s&t=%s&s=%s&f=json&v=%s&c=%s&id=%s", protocol_ptr, server_ptr, user_ptr, login_token_ptr, login_salt_ptr, opensubsonic_version_ptr, client_name_ptr, id);
    rc = opensubsonic_http_json_get(full_url, response);
    free(full_url);
    return rc;
}




// TODO COVER ART - Returns JSON on error.
// {"subsonic-response":{"status":"failed","version":"1.16.1","type":"navidrome","serverVersion":"0.53.1-FREEBSD (1ba390a)","openSubsonic":true,"error":{"code":70,"message":"Artwork not found"}}}
// Contact the /rest/getCoverArt endpoint (Returns binary data)
void opensubsonic_getCoverArt(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char* id, binary_response_struct* response) {
    // Generate full URL, perform HTTP GET, and free the full URL
    char* full_url = malloc(256);
    snprintf(full_url, 256, "%s://%s/rest/getCoverArt?u=%s&t=%s&s=%s&f=json&v=%s&c=%s&id=%s", protocol_ptr, server_ptr, user_ptr, login_token_ptr, login_salt_ptr, opensubsonic_version_ptr, client_name_ptr, id);
    opensubsonic_http_binary_get(full_url, response);
    free(full_url);
}

// Contact the /rest/getCoverArt endpoint (Scale is in pixels) (Returns binary data)
void opensubsonic_getCoverArtScaled(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char* id, int scale, char** response) {
    // Generate full URL, perform HTTP GET, and free the full URL
    char* full_url = malloc(256);
    snprintf(full_url, 256, "%s://%s/rest/getCoverArt?u=%s&t=%s&s=%s&f=json&v=%s&c=%s&id=%s&size=%d", protocol_ptr, server_ptr, user_ptr, login_token_ptr, login_salt_ptr, opensubsonic_version_ptr, client_name_ptr, id, scale);
    opensubsonic_http_json_get(full_url, response);
    free(full_url);
}

// Perform HTTP GET for JSON
// Returns http code normally, or 600 if an error occured
int opensubsonic_http_json_get(char* url, char** response) {
    // Allocate initial memory
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_handle = curl_easy_init();
    if (curl_handle) {
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "malexty_opensubsonic_client/1.0");
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl_handle, CURLOPT_TCP_KEEPALIVE,0L);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        res = curl_easy_perform(curl_handle);

        if (res != CURLE_OK) {
            logger_log_error(__func__, "curl_easy_perform() did not return CURLE_OK.");
            curl_easy_cleanup(curl_handle);
            return 600;
        }
    } else {
        logger_log_error(__func__, "curl_easy_init() failed.");
        return 600;
    }
    curl_easy_cleanup(curl_handle);

    // Malloc the storage pointer, copy the write callback memory to it, and free CURL's write callback memory
    *response = malloc(chunk.size);
    memcpy(*response, chunk.memory, chunk.size);
    free(chunk.memory);
}

// Perform HTTP GET for binary data
// Returns http code normally, or 600 if an error occured
// (600 is out of the possible http codes: https://en.wikipedia.org/wiki/List_of_HTTP_status_codes)
// A lot of these functions return JSON if failure, or binary data if successful. Response code 200 either way. TODO Check with content type (application/json 또는 image/jpeg)
int opensubsonic_http_binary_get(char* url, binary_response_struct* response) {
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_handle = curl_easy_init();
    if (curl_handle) {
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "malexty_opensubsonic_client/1.0");
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl_handle, CURLOPT_TCP_KEEPALIVE,0L);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        res = curl_easy_perform(curl_handle);

        if (res != CURLE_OK) {
            logger_log_error(__func__, "curl_easy_perform() did not return CURLE_OK.");
            curl_easy_cleanup(curl_handle);
            return 600;
        }

        char* ct = NULL;
        long responseCode = 0;
        curl_easy_getinfo(curl_handle, CURLINFO_CONTENT_TYPE, &ct);
        curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &responseCode);
              if(!res && ct) {
                printf("Content-Type: %s\n", ct);
              }
    } else {
        logger_log_error(__func__, "curl_easy_init() failed.");
        return 600;
    }
    curl_easy_cleanup(curl_handle);
    printf("%s\n", chunk.memory);

    // Malloc the storage pointer, copy the write callback memory to it, and free CURL's write callback memory
    response->size = chunk.size;
    response->memory = malloc(chunk.size);
    memcpy(response->memory, chunk.memory, chunk.size);
    free(chunk.memory);
}
