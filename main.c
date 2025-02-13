#include <stdio.h>
#include <stdlib.h>
#include "libopensubsonic/crypto.h"
#include "libopensubsonic/httpclient.h"
#include "libopensubsonic/endpoint_ping.h"
#include "libopensubsonic/endpoint_getArtists.h"

// 149 bytes in total (without padding)
const char* openSubSonicUsername = "admin"; // 32 byte limit
const char* openSubSonicPassword = "password"; // 32 byte limit
const char* openSubSonicServer = "192.168.5.250:4533"; // 38 byte limit (32 for the domain, 1 for colon, 5 for the port)
const char* openSubSonicProtocol = "http"; // 5 byte limit (http / https)
const char* openSubSonicVersion = "1.8.0"; // 10 byte limit
const char* openSubSonicClientName = "MalextyClient"; // 32 byte limit

char* openSubSonicLoginSalt = NULL;
char* openSubSonicLoginToken = NULL;

int main() {
    printf("OpenSubSonic Client\n");

    // Check lengths of user defined settings to avoid buffer overflows


    // Allocate space on the heap for the login token and salt
    openSubSonicLoginSalt = malloc(16 + 1);
    openSubSonicLoginToken = malloc(32 + 1);

    // Generate the login token and salt
    crypto_secure_generate_salt(openSubSonicLoginSalt);
    crypto_secure_generate_token(openSubSonicPassword, openSubSonicLoginSalt, openSubSonicLoginToken);
    printf("Login Salt: %s\n", openSubSonicLoginSalt);
    printf("Login Token: %s\n", openSubSonicLoginToken);

    // Contact the /ping endpoint to test login
    printf("Attempting login to OpenSubSonic server at %s://%s...\n", openSubSonicProtocol, openSubSonicServer);
    char* opensubsonic_ping_res;
    opensubsonic_ping(openSubSonicProtocol, openSubSonicServer, openSubSonicUsername, openSubSonicLoginToken, openSubSonicLoginSalt, openSubSonicVersion, openSubSonicClientName, &opensubsonic_ping_res);
    opensubsonic_ping_struct pingStruct;
    opensubsonic_ping_parse(opensubsonic_ping_res, &pingStruct);
    free(opensubsonic_ping_res);
    printf("Server: %s %s (%s)\nOpenSubsonic: %d\n", pingStruct.serverType, pingStruct.version, pingStruct.serverVersion, pingStruct.openSubsonicCapable);
    if (pingStruct.error) {
        printf("There was an error logging in (Code %d): %s\n", pingStruct.errorCode, pingStruct.errorMessage);
        exit(EXIT_FAILURE);
    }

    // Fetch all artists from the /getArtists endpoint
    printf("Fetching list of artists...\n");
    char* opensubsonic_getArtists_res;
    opensubsonic_getArtists(openSubSonicProtocol, openSubSonicServer, openSubSonicUsername, openSubSonicLoginToken, openSubSonicLoginSalt, openSubSonicVersion, openSubSonicClientName, &opensubsonic_getArtists_res);
    opensubsonic_getArtists_struct getArtistsStruct;
    opensubsonic_getArtists_parse(opensubsonic_getArtists_res, &getArtistsStruct);
    free(opensubsonic_getArtists_res);

    printf("Status: %s\nArtists: %d\nLast modified: %ld\n", getArtistsStruct.status, getArtistsStruct.artistCount, getArtistsStruct.lastModified);
    for (int i = 0; i < getArtistsStruct.artistCount; i++){
    printf("%s %s %s %d\n",
        getArtistsStruct.artists[i].name, getArtistsStruct.artists[i].id,
        getArtistsStruct.artists[i].coverArt, getArtistsStruct.artists[i].albumCount);
    }











    // Free structs
    opensubsonic_ping_struct_free(&pingStruct);
    opensubsonic_getArtists_struct_free(&getArtistsStruct);
}
