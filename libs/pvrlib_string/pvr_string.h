/* Made by Palver. 30.12.2025. C99 */

#ifndef PVR_STRING_IMPLEMENTATION
char* PVR_replace(char *str, char char_old, char char_new);
#endif

#ifdef PVR_STRING_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Replaces characters in a string. */
char* PVR_replace(char *str, char char_old, char char_new) {
    size_t str_size = strlen(str);

    for (size_t i = 0; i < str_size; i++) {
        if (str[i] == char_old) {str[i] = char_new;}
    }

    return str;
}

#endif
