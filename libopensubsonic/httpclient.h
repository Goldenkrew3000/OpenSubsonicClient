#ifndef _HTTPCLIENT_H
#define _HTTPCLIENT_H

void opensubsonic_ping(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char** response);
void opensubsonic_getArtists(const char* protocol_ptr, const char* server_ptr, const char* user_ptr, char* login_token_ptr, char* login_salt_ptr, const char* opensubsonic_version_ptr, const char* client_name_ptr, char** response);
void opensubsonic_http_json_get(char* url, char** response);

#endif
