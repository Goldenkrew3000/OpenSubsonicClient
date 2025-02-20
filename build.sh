egcc -I/usr/local/include -L/usr/local/lib \
    main.c \
    logger.c \
    external/md5.c \
    libopensubsonic/crypto.c \
    libopensubsonic/httpclient.c \
    libopensubsonic/endpoint_ping.c \
    libopensubsonic/endpoint_getArtists.c \
    libopensubsonic/endpoint_getArtist.c \
    libopensubsonic/endpoint_getAlbum.c \
    libopensubsonic/endpoint_getLyricsBySongId.c \
    -o subsonic-client -g -lcurl -lcjson
