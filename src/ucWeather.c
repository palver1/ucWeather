#include <stdlib.h>
#include <stdio.h>

#include "../libs/curl/include/curl/curl.h"
#include "../libs/lib_json_parser/json_parser.h"

char API[50];
char URL[200];
char *history;

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

int main(void) {   
    /* MAKE URL ============================================================================== */
    FILE *file_api = fopen("out/api.key", "r"); //TODO: fix relative path problem
    if (file_api == NULL) { perror("ERROR: Unable to load the api.key file"); exit(EXIT_FAILURE); }
    
    fread(API, 50, 1, file_api);
    /* Remove nl-char */
    char *c = API;
    while (*c != '\n') { c++; }
    if (*c == '\n') { *c = '\0'; }

    snprintf(URL, 200, "%s%s%s", "http://api.weatherapi.com/v1/current.json?key=", API, "&q=London");

    /* SEND HTTP REQUEST ============================================================================== */
    CURL *curl;
    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if(res) {
        perror("curl failed");
        return (int)res;
    }

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, URL);                      // set up URL address
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback); // set up callback function

        /* Perform the request, res gets the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }

    /* SAVE DATA TO THE HISTORY ============================================================================== */
    puts("\n");

    curl_global_cleanup();

    return (int)res;
}


size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    puts("HERE'S THE SAVED DATA"); // DEBUG
    printf("%s", JP_get_value(ptr, "country")); // DEBUG

    return nmemb;
}
