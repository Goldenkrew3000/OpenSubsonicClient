#include <stdio.h>
#include "crypto.h"
#include "../external/md5.h"

#if __NetBSD__ // NetBSD만
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

// Use arc4random() to generate cryptographically secure bytes. Should work on all BSD-style systems
// unsigned char bytes[8] crypto_secure_arc4random_generate(bytes, 8);
void crypto_secure_arc4random_generate(unsigned char* bytes, size_t length) {
    for (size_t i = 0; i < length; i++) {
        bytes[i] = arc4random() & 0xFF;
    }
}

// Use the arandom sysctl on NetBSD to generate cryptographically secure bytes.
// unsigned char bytes[8] crypto_secure_netbsd_arandom_generate(bytes, 8);
#if __NetBSD__
void crypto_secure_netbsd_arandom_generate(unsigned char* bytes, size_t length) {
    // Setup the sysctl MIB for kern.arandom
    int mib[2];
    mib[0] = CTL_KERN;
    mib[1] = KERN_ARND;

    // Read random bytes
    if (sysctl(mib, 2, bytes, &length, NULL, 0) == -1) {
        printf("Sysctl error\n");
        exit(EXIT_FAILURE);
    }
}
#endif

// Generate an 8 byte / 16 letter hex representation salt
// Returns 17 bytes (16 bytes + 1 padding)
void crypto_secure_generate_salt(char* salt_ptr) {
    uint8_t salt_bytes[8];

    // Generate cryptographically secure salt bytes using OS-native functions
    #if __NetBSD__
    // Use NetBSD native crypto function
    crypto_secure_netbsd_arandom_generate(salt_bytes, 8);
    #else
    // Use BSD/Linux crypto function (This does NOT check for Win32, do NOT compile outside of a BSD/Linux environment)
    crypto_secure_arc4random_generate(salt_bytes, 8);
    #endif

    // Convert to a string hex representation
    snprintf(salt_ptr, 17, "%02x%02x%02x%02x%02x%02x%02x%02x",
        salt_bytes[0], salt_bytes[1], salt_bytes[2], salt_bytes[3],
        salt_bytes[4], salt_bytes[5], salt_bytes[6], salt_bytes[7]);
}

// Generate an MD5 checksum in string hex representation of the account password and salt
// Returns 33 bytes (32 bytes + 1 padding)
void crypto_secure_generate_token(const char* accountPassword_ptr, char* salt_ptr, char* token_ptr) {
    uint8_t md5_bytes[16];
    uint8_t token_plaintext[32 + 16 + 1]; // Password + Salt + Padding

    // Check if the password is over the allowed length (avoid buffer overflow)
    if (strlen(accountPassword_ptr) > 32) {
        printf("Password is above the allowed length, exiting.\n");
        exit(EXIT_FAILURE);
    }

    // Concatenate account password and salt into single string
    snprintf(token_plaintext, sizeof(token_plaintext) / sizeof(char), "%s%s", accountPassword_ptr, salt_ptr);

    // Generate an MD5 checksum of the plaintext token
    md5String(token_plaintext, md5_bytes);

    // Convert the MD5 checksum bytes into string hex representation
    snprintf(token_ptr, 33, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
        md5_bytes[0], md5_bytes[1], md5_bytes[2], md5_bytes[3],
        md5_bytes[4], md5_bytes[5], md5_bytes[6], md5_bytes[7],
        md5_bytes[8], md5_bytes[9], md5_bytes[10], md5_bytes[11],
        md5_bytes[12], md5_bytes[13], md5_bytes[14], md5_bytes[15]);
}
