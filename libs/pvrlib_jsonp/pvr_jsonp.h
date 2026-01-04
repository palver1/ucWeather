/* Made by Palver. 08.11.2025. C99
 *
 */

#ifndef PVR_JSONP_H
#define PVR_JSONP_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

/* Searches for a key. Returns index or -1. */
static inline int PVR_json_find_key(char buffer_json[], char key[]) {
    bool is_str_found = false;
    int first_index;
    size_t i_buffer = 0;
    size_t i_str    = 0;

    const size_t key_len        = strlen(key);
    const char   key_syntax[]   = "\":";
    const size_t key_syntax_len = strlen(key_syntax);
    char key_with_colon[key_len + key_syntax_len + 1];

    snprintf(key_with_colon, key_len + key_syntax_len + 1, "%s%s", key, key_syntax); // looking for 'key":'

    while (buffer_json[i_buffer] != '\0') {
        if (buffer_json[i_buffer] == key_with_colon[i_str]) {
            if (i_str == 0) {
                first_index = i_buffer;
            }
            
            i_str++;
        } else {i_str = 0;}

        if ((i_str + 1) == key_len + key_syntax_len) {
            // return first_index;
            return i_buffer; // return index of a colon
        }

        i_buffer++;
    }

    return -1; // failure returns -1 instead of 0 because zero is a valid index
}

/* Returns a value by a key. */
static inline char *PVR_json_get_value(char buffer_json[], char key[]) {
    int index_key = PVR_json_find_key(buffer_json, key);
    int val_index_start;
    int val_index_end;

    if (index_key != -1) {
        int i = index_key;

        /* Define the starting index */
        while (buffer_json[i] != ':' && buffer_json[i] != '\0') {
            i++;
        }
        if (buffer_json[i] != ':') {return NULL;}

        while (buffer_json[i] == ':' || // Exclude spaces and syntax
               buffer_json[i] == ' ' ||
               buffer_json[i] == '"' ||
               buffer_json[i] == '{' ||
               buffer_json[i] == '[' ||
               buffer_json[i] == '(') {
            i++ ;
        }
        val_index_start = i;

        /* Define the ending index */
        while (buffer_json[i+1] != '"'  &&
               buffer_json[i+1] != ','  &&
               buffer_json[i+1] != '}'  &&
               buffer_json[i+1] != ']'  &&
               buffer_json[i+1] != ')'  &&
               buffer_json[i+1] != '\0' &&
               buffer_json[i+1] != '\n') {
            i++;
        }
        val_index_end = i;

        /* Obtain the value */
        int value_size = (val_index_end - val_index_start) + 2; 
        if (!value_size) {return NULL;}
        char *value = (char*)malloc(value_size);
        if (value == NULL) {perror("Allocation failed"); exit(EXIT_FAILURE);}

        strncpy(value, buffer_json+val_index_start, value_size-1);
        value[value_size-1] = '\0';   

        return value;
    } else { return NULL; }
}

static inline int kek(int num) { return num * 2; }
#endif
