#ifndef _ENDPOINT_GETARTIST_H
#define _ENDPOINT_GETARTIST_H

typedef struct {
    char* id;
    char* title;
    char* name;
    char* album;
    char* artist;
    int yearReleased;
    char* genre;
    char* coverArt;
    long duration;
    int playCount;
    char* dateAdded;
    int songCount;
} opensubsonic_getArtist_album_struct;

typedef struct {
    char* status;
    int errorCode;
    char* errorMessage;
    char* artistId;
    char* artistName;
    char* coverArt;
    int albumCount;
    opensubsonic_getArtist_album_struct* albums;
} opensubsonic_getArtist_struct;

int opensubsonic_getArtist_parse(char* data, opensubsonic_getArtist_struct* getArtistStruct);
void opensubsonic_getArtist_struct_free(opensubsonic_getArtist_struct* getArtistStruct);

#endif
