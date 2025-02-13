#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "httpclient.h"

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
void opensubsonic_ping(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char** response) {
    // Generate full URL (All variables here have a maximum size of 165 bytes, without padding), allocate 256 bytes
    char* full_url = malloc(256);
    snprintf(full_url, 256, "%s://%s/rest/ping?u=%s&t=%s&s=%s&f=json&v=%s&c=%s", protocol_ptr, server_ptr, user_ptr, login_token_ptr, login_salt_ptr, opensubsonic_version_ptr, client_name_ptr);

    // Perform HTTP GET
    opensubsonic_http_json_get(full_url, response);

    // Free full URL
    free(full_url);
}

// Contact the /rest/getArtists endpoint
void opensubsonic_getArtists(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char** response) {
    // Generate full URL, perform HTTP GET, and free the full URL
    char* full_url = malloc(256);
    snprintf(full_url, 256, "%s://%s/rest/getArtists?u=%s&t=%s&s=%s&f=json&v=%s&c=%s", protocol_ptr, server_ptr, user_ptr, login_token_ptr, login_salt_ptr, opensubsonic_version_ptr, client_name_ptr);
    opensubsonic_http_json_get(full_url, response);
    free(full_url);
}

// Perform HTTP GET for JSON
void opensubsonic_http_json_get(char* url, char** response) {
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);  // Initially allocate 1 byte
    chunk.size = 0;


    curl_handle = curl_easy_init();
    if (curl_handle) {
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "useragent");
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 50L);
        curl_easy_setopt(curl_handle, CURLOPT_TCP_KEEPALIVE,0L);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        res = curl_easy_perform(curl_handle);

        if (res != CURLE_OK) {
            printf("There was an error in opensonic_http_json_get()\n");
            exit(EXIT_FAILURE);
        }
    }
    curl_easy_cleanup(curl_handle);

    // Malloc the storage pointer, copy the write callback memory to it, and free CURL's write callback memory
    *response = malloc(chunk.size);
    memcpy(*response, chunk.memory, chunk.size);
    free(chunk.memory);
}
