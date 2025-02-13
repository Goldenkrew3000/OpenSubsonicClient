gcc -I/usr/pkg/include -L/usr/pkg/lib \
    main.c \
    external/md5.c \
    libopensubsonic/crypto.c \
    libopensubsonic/httpclient.c \
    libopensubsonic/endpoint_ping.c \
    libopensubsonic/endpoint_getArtists.c \
    -o subsonic-client -g -lcurl -lcjson
