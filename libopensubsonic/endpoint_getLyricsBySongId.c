#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "../logger.h"
#include "endpoint_getLyricsBySongId.h"

// Parse the JSON returned from the /rest/getLyricsBySongId endpoint (OpenSubsonic ONLY)
// Returns 1 if failure occured, else 0
int opensubsonic_getLyricsBySongId_parse(char* data, opensubsonic_getLyricsBySongId_struct* getLyricsBySongIdStruct) {
    //
}

void opensubsonic_getLyricsBySongId_struct_free(opensubsonic_getLyricsBySongId_struct* getLyricsBySongIdStruct) {
    //
}
