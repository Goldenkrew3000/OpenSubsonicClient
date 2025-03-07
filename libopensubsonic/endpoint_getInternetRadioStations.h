#ifndef _ENDPOINT_GETINTERNETRADIOSTATIONS_H
#define _ENDPOINT_GETINTERNETRADIOSTATIONS_H

typedef struct {
    char* id;           // Unique ID of radio station (Just like the Albums / Songs etc)
    char* name;         // Radio Station Name
    char* streamUrl;    // Radio Station URL
} opensubsonic_getInternetRadioStations_internetRadioStation_struct;

typedef struct {
    char* status;
    int errorCode;
    char* errorMessage;
    int internetRadioStationsCount;
    opensubsonic_getInternetRadioStations_internetRadioStation_struct* internetRadioStations;
} opensubsonic_getInternetRadioStations_struct;

int opensubsonic_getInternetRadioStations_parse(char* data, opensubsonic_getInternetRadioStations_struct* getInternetRadioStationsStruct);
void opensubsonic_getInternetRadioStations_struct_free(opensubsonic_getInternetRadioStations_struct* getInternetRadioStationsStruct);

#endif
