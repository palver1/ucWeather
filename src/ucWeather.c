#ifdef _WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include "../libs/curl/include/curl/curl.h"
#include "../libs/pvrlib_jsonp/pvr_jsonp.h"
#include "../libs/pvrlib_string/pvr_string.h"


/* PROTOTYPES */
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
void define_weather_icon(char *weather_condition, char *weather_icon);


/* MAIN PROCESS */
int main(int argc, char *argv[]) {   
    char API[50];
    char URL[200];
    char name_country[30] = "New York";

    if (argc > 1) {strcpy(name_country, argv[1]);}

    strcpy(name_country, PVR_replace(name_country, ' ', '_'));

    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    /* MAKE URL ============================================================================== */
    FILE *file_api = fopen("out/api.key", "r"); //TODO: fix relative path problem
    if (file_api == NULL) { perror("ERROR: Unable to load the api.key file"); exit(EXIT_FAILURE); }
    
    fread(API, 50, 1, file_api);
    strcpy(API, PVR_replace(API, '\n', '\0'));

    snprintf(URL, 200, "%s%s%s%s", "http://api.weatherapi.com/v1/current.json?key=", API, "&q=", name_country);

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
    return 0;
}


size_t write_callback(char *response, size_t size, size_t nmemb, void *userdata) {
    char weather_condition[100];
    strcpy(weather_condition, PVR_json_get_value(response, "text"));
    char weather_icon[15] = "🪟";

    define_weather_icon(weather_condition, weather_icon);

    puts("-Weather------------------------------------------");
    // Structured respose
    // Location
    
    printf("🌎 %s. %s.\n", PVR_json_get_value(response, "country"), PVR_json_get_value(response, "region"));
    printf("🕑 %s\n", PVR_json_get_value(response, "last_updated"));
    printf("%s %s\n", weather_icon, weather_condition);
    printf("🌡️ %s˚c (%s)˚f [Feels %s˚c (%s˚f)]\n",
           PVR_json_get_value(response, "temp_c"),
           PVR_json_get_value(response, "temp_f"),
           PVR_json_get_value(response, "feelslike_c"),
           PVR_json_get_value(response, "feelslike_f"));
    printf("🍃 %s km\\h %s [Feels %s˚c (%s˚f)]\n",
           PVR_json_get_value(response, "wind_kph"),
           PVR_json_get_value(response, "wind_dir"), 
           PVR_json_get_value(response, "windchill_c"),
           PVR_json_get_value(response, "windchill_f"));
    printf("💧 %s \n", PVR_json_get_value(response, "humidity"));
    printf("☔ %s mm\n", PVR_json_get_value(response, "precip_mm"));
    printf("☁️ %s \n", PVR_json_get_value(response, "cloud"));
    printf("--------------------------------------------------");

    return nmemb;
}


void define_weather_icon(char *weather_condition, char *weather_icon) {
    if (strcmp(weather_condition, "Overcast") == 0) {
       strcpy(weather_icon, "☁️"); }
    else if (strcmp(weather_condition, "Sunny") == 0) {
       strcpy(weather_icon, "☀️"); }
    else if (strcmp(weather_condition, "Clear") == 0) {
       strcpy(weather_icon, "☀️"); }
    else if (strcmp(weather_condition, "Partly cloudy") == 0) {
       strcpy(weather_icon, "⛅"); }
    else if (strcmp(weather_condition, "Cloudy") == 0) {
       strcpy(weather_icon, "☁️"); }
    else if (strcmp(weather_condition, "Mist") == 0) {
       strcpy(weather_icon, "🌫️"); }
    else if (strcmp(weather_condition, "Patchy rain nearby") == 0) {
       strcpy(weather_icon, "🌦️"); }
    else if (strcmp(weather_condition, "Patchy snow nearby") == 0) {
       strcpy(weather_icon, "🌨️"); }
    else if (strcmp(weather_condition, "Patchy sleet nearby") == 0) {
       strcpy(weather_icon, "🌨️"); }
    else if (strcmp(weather_condition, "Patchy freezing drizzle nearby") == 0) {
       strcpy(weather_icon, "🌨️"); }
    else if (strcmp(weather_condition, "Thundery outbreaks in nearby") == 0) {
       strcpy(weather_icon, "🌩️"); }
    else if (strcmp(weather_condition, "Blowing snow") == 0) {
       strcpy(weather_icon, "🌨️"); }
    else if (strcmp(weather_condition, "Blizzard") == 0) {
       strcpy(weather_icon, "🌨️"); }
    else if (strcmp(weather_condition, "Fog") == 0) {
       strcpy(weather_icon, "🌫️"); }
    else if (strcmp(weather_condition, "Freezing fog") == 0) {
       strcpy(weather_icon, "🌫️"); }
    else if (strcmp(weather_condition, "Patchy light drizzle") == 0) {
       strcpy(weather_icon, "🌦️"); }
    else if (strcmp(weather_condition, "Light drizzle") == 0) {
       strcpy(weather_icon, "🌦️"); }
    else if (strcmp(weather_condition, "Freezing drizzle") == 0) {
       strcpy(weather_icon, "🌨️"); }
    else if (strcmp(weather_condition, "Heavy freezing drizzle") == 0) {
       strcpy(weather_icon, "🌨️"); }
    else if (strcmp(weather_condition, "Patchy light rain") == 0) {
       strcpy(weather_icon, "🌦️"); }
    else if (strcmp(weather_condition, "Light rain") == 0) {
       strcpy(weather_icon, "🌧️"); }
    else if (strcmp(weather_condition, "Moderate rain at times") == 0) {
       strcpy(weather_icon, "🌧️"); }
    else if (strcmp(weather_condition, "Moderate rain") == 0) {
       strcpy(weather_icon, "🌧️"); }
    else if (strcmp(weather_condition, "Heavy rain at times") == 0) {
       strcpy(weather_icon, "🌧️"); }
    else if (strcmp(weather_condition, "Heavy rain") == 0) {
       strcpy(weather_icon, "🌧️"); }
    else if (strcmp(weather_condition, "Light freezing rain") == 0) {
       strcpy(weather_icon, "🌨️"); }
    else if (strcmp(weather_condition, "Moderate or heavy freezing rain") == 0) {
       strcpy(weather_icon, "🌨️"); }
    else if (strcmp(weather_condition, "Light sleet") == 0) {
       strcpy(weather_icon, "🌨️"); }
    else if (strcmp(weather_condition, "Moderate or heavy sleet") == 0) {
       strcpy(weather_icon, "🌨️"); }
    else if (strcmp(weather_condition, "Patchy light snow") == 0) {
       strcpy(weather_icon, "🌨️"); }
    else if (strcmp(weather_condition, "Light snow") == 0) {
       strcpy(weather_icon, "❄️"); }
    else if (strcmp(weather_condition, "Patchy moderate snow") == 0) {
       strcpy(weather_icon, "🌨️"); }
    else if (strcmp(weather_condition, "Moderate snow") == 0) {
       strcpy(weather_icon, "❄️"); }
    else if (strcmp(weather_condition, "Patchy heavy snow") == 0) {
       strcpy(weather_icon, "🌨️"); }
    else if (strcmp(weather_condition, "Heavy snow") == 0) {
       strcpy(weather_icon, "❄️"); }
    else if (strcmp(weather_condition, "Ice pellets") == 0) {
       strcpy(weather_icon, "❄️"); }
    else if (strcmp(weather_condition, "Light rain shower") == 0) {
       strcpy(weather_icon, "🌦️"); }
    else if (strcmp(weather_condition, "Moderate or heavy rain shower") == 0) {
       strcpy(weather_icon, "🌧️"); }
    else if (strcmp(weather_condition, "Torrential rain shower") == 0) {
       strcpy(weather_icon, "🌧️"); }
    else if (strcmp(weather_condition, "Light sleet showers") == 0) {
       strcpy(weather_icon, "🌨️"); }
    else if (strcmp(weather_condition, "Moderate or heavy sleet showers") == 0) {
       strcpy(weather_icon, "🌨️"); }
    else if (strcmp(weather_condition, "Light snow showers") == 0) {
       strcpy(weather_icon, "❄️"); }
    else if (strcmp(weather_condition, "Moderate or heavy snow showers") == 0) {
       strcpy(weather_icon, "❄️"); }
    else if (strcmp(weather_condition, "Light showers of ice pellets") == 0) {
       strcpy(weather_icon, "❄️"); }
    else if (strcmp(weather_condition, "Moderate or heavy showers of ice pellets") == 0) {
       strcpy(weather_icon, "❄️"); }
    else if (strcmp(weather_condition, "Patchy light rain in areastrcpy(with thunder") == 0) {
       strcpy(weather_icon, "🌩️"); }
    else if (strcmp(weather_condition, "Moderate or heavy rain in areastrcpy(with thunder") == 0) {
       strcpy(weather_icon, "🌩️"); }
    else if (strcmp(weather_condition, "Patchy light snow in areastrcpy(with thunder") == 0) {
       strcpy(weather_icon, "🌩️"); }
    else if (strcmp(weather_condition, "Moderate or heavy snow in areastrcpy(with thunder") == 0) {
       strcpy(weather_icon, "🌩️"); }
}

