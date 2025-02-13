#ifndef _ENDPOINT_PING_H
#define _ENDPOINT_PING_H
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    char* status;
    char* version;
    char* serverType;
    char* serverVersion;
    bool openSubsonicCapable;
    bool error;
    int errorCode;
    char* errorMessage;
} opensubsonic_ping_struct;

int opensubsonic_ping_parse(char* data, opensubsonic_ping_struct* pingStruct);
void opensubsonic_ping_struct_free(opensubsonic_ping_struct* pingStruct);

#endif
