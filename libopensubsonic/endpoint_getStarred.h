#ifndef _ENDPOINT_GETSTARRED_H
#define _ENDPOINT_GETSTARRED_H

typedef struct {
    char* id;       // Artist ID
    char* name;     // Artist Name
    int albumCount; // Album Count
    char* starred;  // TZ of when the artist was starred
    // 'coverArt' and 'artistImageUrl' are pointless here
} opensubsonic_getStarred_artist_struct;

typedef struct {
    char* id;           // Album ID
    // 'parent' and 'isDir' are kind of pointless as of now
    // 'title', 'name', and 'album' are always the same
    char* title;        // Album Title
    char* name;         // Album Name
    char* album;        // Album Album Name
    char* artist;       // Artist Name
    int year;           // Year released
    char* genre;        // Album genre
    char* coverArtId;   // Album cover art ID
    char* starred;      // TZ of starred
    long duration;      // Album Duration in seconds
    // 'playCount' and 'created' is pointless
    char* artistId;     // Artist ID
    int songCount;      // Songs in album
    // 'isVideo', 'played', 'bpm', 'comment', 'sortName', 'mediaType', 'musicBrainzId', 'channelCount', and 'samplingRate' are pointless
} opensubsonic_getStarred_album_struct;

typedef struct {
    char* id;           // Song ID
    // 'parent' seems pointless
    // 'isDir' seems pointless
    char* title;        // Song Title
    char* album;        // Song Album
    char* artist;       // Song Artist
    int track;          // Track of the album? TODO
    int year;           // Year released
    char* genre;        // Primary genre of the song
    char* coverArtId;   // Song cover art ID
    long size;          // Song filesize in bytes
    // 'contentType' and 'suffix' are pointless
    char* starred;      // TZ of when the song was starred
    long duration;      // Duration of the song in seconds
    int bitRate;        // Bitrate of the song
    // 'path' is pointless
    int playCount;      // Play count of the song
    // 'discNumber' is pointless for now
    // 'created' is pointless
    char* albumId;      // Album ID
    char* artistId;     // Artist ID
    // 'type', 'isVideo', 'played', 'bpm', 'comment', 'sortName', 'mediaType', and 'musicBrainzId' are pointless
    int channelCount;   // Channel count of the song
    int sampleRate;     // Sample rate of the song
} opensubsonic_getStarred_song_struct;

typedef struct {
    char* status;
    int errorCode;
    char* errorMessage;
    int artistCount;
    int albumCount;
    int songCount;
    opensubsonic_getStarred_artist_struct* artists;
    opensubsonic_getStarred_album_struct* albums;
    opensubsonic_getStarred_song_struct* songs;
} opensubsonic_getStarred_struct;

int opensubsonic_getStarred_parse(char* data, opensubsonic_getStarred_struct* getStarredStruct);
void opensubsonic_getStarred_struct_free(opensubsonic_getStarred_struct* getStarredStruct);

#endif
