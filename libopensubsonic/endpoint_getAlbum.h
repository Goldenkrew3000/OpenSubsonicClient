#ifndef _ENDPOINT_GETALBUM_H
#define _ENDPOINT_GETALBUM_H

typedef struct {
    char* id;
    char* title;
    int track;
    char* coverArt;
    long filesize;
    long duration;
    int playCount;
} opensubsonic_getAlbum_songs_struct;

typedef struct {
    char* status;
    int errorCode;
    char* errorMessage;
    char* albumId;
    char* albumName;
    char* albumArtist;
    char* albumArtistId;
    char* coverArtId;
    int songCount;
    long duration;
    int playCount;
    int yearReleased;
    char* genre; // NOTE: Does not seem to match up with the genre assigned in getArtist TODO
    opensubsonic_getAlbum_songs_struct* songs;
} opensubsonic_getAlbum_struct;

int opensubsonic_getAlbum_parse(char* data, opensubsonic_getAlbum_struct* getAlbumStruct);
void opensubsonic_getAlbum_struct_free(opensubsonic_getAlbum_struct* getAlbumStruct);

#endif
