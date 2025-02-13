#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
#include "libopensubsonic/crypto.h"
#include "libopensubsonic/httpclient.h"
#include "libopensubsonic/endpoint_ping.h"
#include "libopensubsonic/endpoint_getArtists.h"
#include "libopensubsonic/endpoint_getArtist.h"

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
    logger_log_title(__func__, "OpenSubsonic Client");
    int rc = 0;

    // Check lengths of user defined settings to avoid buffer overflows
    // TODO

    // Allocate space on the heap for the login token and salt
    openSubSonicLoginSalt = malloc(16 + 1);
    openSubSonicLoginToken = malloc(32 + 1);

    // Generate the login token and salt
    crypto_secure_generate_salt(openSubSonicLoginSalt);
    crypto_secure_generate_token(openSubSonicPassword, openSubSonicLoginSalt, openSubSonicLoginToken);
    logger_log_important(__func__, "Login Salt: %s", openSubSonicLoginSalt);
    logger_log_important(__func__, "Login Token: %s", openSubSonicLoginToken);

    // Contact the /ping endpoint to test login
    logger_log_general(__func__, "Attempting login to OpenSubsonic server at %s://%s...", openSubSonicProtocol, openSubSonicServer);
    char* opensubsonic_ping_res;
    opensubsonic_ping_struct pingStruct;
    for (size_t retry = 0; retry < 3; retry++) {
        rc = opensubsonic_ping(openSubSonicProtocol, openSubSonicServer, openSubSonicUsername, openSubSonicLoginToken, openSubSonicLoginSalt, openSubSonicVersion, openSubSonicClientName, &opensubsonic_ping_res);
        if (rc == 600) {
            if (retry < 2) {
                logger_log_error(__func__, "Retrying...");
            } else {
                logger_log_error(__func__, "Critical error has occured while attempting to download, exiting.");
                exit(EXIT_FAILURE);
            }
        } else {
            break;
        }
    }
    rc = opensubsonic_ping_parse(opensubsonic_ping_res, &pingStruct);
    if (rc != 0) {
        logger_log_error(__func__, "Critical error has occured while attempting to parse JSON.");
        exit(EXIT_FAILURE);
    }
    free(opensubsonic_ping_res);

    logger_log_general(__func__, "Server: %s %s (%s)", pingStruct.serverType, pingStruct.version, pingStruct.serverVersion);
    if (pingStruct.error) {
        logger_log_error(__func__, "There was an error logging in (Code %d): %s\nExiting...", pingStruct.errorCode, pingStruct.errorMessage);
        exit(EXIT_FAILURE);
    }

    // Fetch all artists from the /getArtists endpoint
    logger_log_general(__func__, "Fetching a list of artists from the /getArtists endpoint...");
    char* opensubsonic_getArtists_res;
    opensubsonic_getArtists_struct getArtistsStruct;
    for (size_t retry = 0; retry < 3; retry++) {
        rc = opensubsonic_getArtists(openSubSonicProtocol, openSubSonicServer, openSubSonicUsername, openSubSonicLoginToken, openSubSonicLoginSalt, openSubSonicVersion, openSubSonicClientName, &opensubsonic_getArtists_res);
        if (rc == 600) {
            if (retry < 2) {
                logger_log_error(__func__, "Retrying...");
            } else {
                logger_log_error(__func__, "Critical error has occured while attempting to download, exiting.");
                exit(EXIT_FAILURE);
            }
        } else {
            break;
        }
    }
    rc = opensubsonic_getArtists_parse(opensubsonic_getArtists_res, &getArtistsStruct);
    if (rc != 0) {
        logger_log_error(__func__, "Critical error has occured while attempting to parse JSON.");
        exit(EXIT_FAILURE);
    }
    free(opensubsonic_getArtists_res);

    printf("Status: %s\nArtists: %d\nLast modified: %ld\n", getArtistsStruct.status, getArtistsStruct.artistCount, getArtistsStruct.lastModified);
    for (int i = 0; i < getArtistsStruct.artistCount; i++){
    printf("%s %s %s %d\n",
        getArtistsStruct.artists[i].name, getArtistsStruct.artists[i].id,
        getArtistsStruct.artists[i].coverArt, getArtistsStruct.artists[i].albumCount);
    }

    char* lmao = NULL;
    char* artist_id = "95a5c13aa2ebd2fa5f8434880215bdfc";
    opensubsonic_getArtist(openSubSonicProtocol, openSubSonicServer, openSubSonicUsername, openSubSonicLoginToken, openSubSonicLoginSalt, openSubSonicVersion, openSubSonicClientName, artist_id, &lmao);

    opensubsonic_getArtist_struct getArtistStruct;
    opensubsonic_getArtist_parse(lmao, &getArtistStruct);
    for (size_t i = 0; i < 3; i++) {
        printf("%s\n", getArtistStruct.albums[i].id);
    }





    /*
    binary_response_struct binary_res;
    char* id = "1923f47530cf919848414e79dbbe7f84";
    char* orig = NULL;
    char* scale_a = NULL;
    char* scale_b = NULL;
    opensubsonic_getCoverArt(openSubSonicProtocol, openSubSonicServer, openSubSonicUsername, openSubSonicLoginToken, openSubSonicLoginSalt, openSubSonicVersion, openSubSonicClientName, id, &binary_res);
    */
    //opensubsonic_getCoverArtScaled(openSubSonicProtocol, openSubSonicServer, openSubSonicUsername, openSubSonicLoginToken, openSubSonicLoginSalt, openSubSonicVersion, openSubSonicClientName, id, 600, &scale_a);
    //opensubsonic_getCoverArtScaled(openSubSonicProtocol, openSubSonicServer, openSubSonicUsername, openSubSonicLoginToken, openSubSonicLoginSalt, openSubSonicVersion, openSubSonicClientName, id, 250, &scale_b);
    //printf("%d %d %d\n", sizeof(orig), sizeof(scale_a), sizeof(scale_b));
    //printf("Final image size: %ld\n", binary_res.size);

    // fifo test











    logger_log_title(__func__, "Performing exit sequence.");

    // Free structs
    opensubsonic_ping_struct_free(&pingStruct);
    opensubsonic_getArtists_struct_free(&getArtistsStruct);

    // Free login salt and token
    free(openSubSonicLoginSalt);
    free(openSubSonicLoginToken);
}
//https://www.subsonic.org/pages/api.jsp
