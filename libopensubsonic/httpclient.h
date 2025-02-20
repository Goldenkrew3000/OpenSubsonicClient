#ifndef _HTTPCLIENT_H
#define _HTTPCLIENT_H
#include <stdio.h>

typedef struct {
    char* memory;
    size_t size;
} binary_response_struct;

int opensubsonic_ping(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char** response);
int opensubsonic_getArtists(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char** response);
int opensubsonic_getArtist(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char* id, char** response);
int opensubsonic_getAlbum(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char* id, char** response);
int opensubsonic_getLyricsBySongId(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char* id, char** response);
void opensubsonic_getCoverArt(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char* id, binary_response_struct* response);
void opensubsonic_getCoverArtScaled(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char* id, int scale, char** response);
int opensubsonic_http_json_get(char* url, char** response);
int opensubsonic_http_binary_get(char* url, binary_response_struct* response);

#endif
