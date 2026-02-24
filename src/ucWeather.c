#ifdef _WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#include "../libs/curl/include/curl/curl.h"
#include "../libs/pvrlib_jsonp/pvr_jsonp.h"
#include "../libs/pvrlib_string/pvr_string.h"

#define VERSION "v1.0"

/* PROTOTYPES */
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);
void define_weather_icon(char *weather_condition, char *weather_icon);
void change_date_fromat(char *date);


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

    /* MAKE ROOT PATH ============================================================================== */
    /* Get exe path*/
    char exe_path[MAX_PATH];

    int exe_path_len = GetModuleFileNameA(NULL, exe_path, MAX_PATH);
    if (exe_path_len == 0) {
        perror("Error: couldn't get the program's path."); return(EXIT_FAILURE);
    } else if (exe_path_len >= MAX_PATH) {
        perror("Error: path is truncated (exe_path is too small)"); return(EXIT_FAILURE);
    }

    /* Remove .exe file from the path */
    int last_slash_index;
    for (int i = 0; i < exe_path_len; i++) {
        if (exe_path[i] == '\\') {
            last_slash_index = i+1;
        }
    }
    exe_path[last_slash_index] = '\0';

    /* Make the path to api.key file */
    char path_file_api[MAX_PATH];
    #ifdef DEBUG
    snprintf(path_file_api, MAX_PATH, "%s%s", exe_path, "..\\api.key");
    #else // RELEASE
    snprintf(path_file_api, MAX_PATH, "%s%s", exe_path, "api.key");
    #endif

    /* MAKE URL ============================================================================== */
    FILE *file_api = fopen(path_file_api, "r");
    if (file_api == NULL) {
        perror("api.key file not found. Would you like to create one?\n"); 
        printf("Yes [y] | No [n]\n");

        char answer = getchar();
        if (answer == 'y') {
            fclose(file_api);
            /* CREATE A NEW API.KEY FILE */
            file_api = fopen(path_file_api, "w");
            if (file_api == NULL) {perror("Failed to create a file"); exit(EXIT_FAILURE);}

            printf("Enter your WeatherAPI key:\n");
            char api_key[100] = {0};
            PVR_clear_nlc();
            fgets(api_key, 100, stdin);
            api_key[strcspn(api_key, "\n")] = '\0';

            size_t f_written = fwrite(api_key, sizeof(char), strlen(api_key), file_api);
            if (f_written != strlen(api_key)) {perror("Failed to write into a file"); exit(EXIT_FAILURE);}

            fclose(file_api);

            /* REOPEN FILE TO READ THE DATA */
            // check if api was entered, delete the file if didn't
            file_api = fopen(path_file_api, "r");
            const short file_content_size = 100;
            char file_content[file_content_size];
            size_t f_read = fread(file_content, sizeof(char), file_content_size, file_api);
            if (!f_read) {
                fclose(file_api);
                char str_del_api_file[MAX_PATH + 4];
                snprintf(str_del_api_file, MAX_PATH + 4, "%s%s", "del ", path_file_api);
                system(str_del_api_file);
                perror("Api key was not saved");
                exit(EXIT_FAILURE);
            }
            rewind(file_api);
        } else if (answer == 'n') {
            exit(EXIT_FAILURE);
        }
    }
    
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

    // puts("\nPress any key to close");
    // getch();

    return 0;
}


size_t write_callback(char *response, size_t size, size_t nmemb, void *userdata) {
    char weather_condition[100];
    strcpy(weather_condition, PVR_json_get_value(response, "text"));
    char weather_icon[15] = "🪟";

    define_weather_icon(weather_condition, weather_icon);

    /* Weather Data */
    char country[50];
    char region[50];
    char last_updated[50];
    char temp_c[50];
    char temp_f[50];
    char feelslike_c[50];
    char feelslike_f[50];
    char wind_kph[50];
    char wind_dir[50];
    char windchill_c[50];
    char windchill_f[50];
    char humidity[50];
    char precip_mm[50];
    char cloud[50];

    strcpy(country, PVR_json_get_value(response, "country"));
    strcpy(region, PVR_json_get_value(response, "region"));
    strcpy(last_updated, PVR_json_get_value(response, "last_updated"));
    change_date_fromat(last_updated);
    strcpy(temp_c, PVR_json_get_value(response, "temp_c"));
    strcpy(temp_f, PVR_json_get_value(response, "temp_f"));
    strcpy(feelslike_c, PVR_json_get_value(response, "feelslike_c"));
    strcpy(feelslike_f, PVR_json_get_value(response, "feelslike_f"));
    strcpy(wind_kph, PVR_json_get_value(response, "wind_kph"));
    strcpy(wind_dir, PVR_json_get_value(response, "wind_dir"));
    strcpy(windchill_c, PVR_json_get_value(response, "windchill_c"));
    strcpy(windchill_f, PVR_json_get_value(response, "windchill_f"));
    strcpy(humidity, PVR_json_get_value(response, "humidity"));
    strcpy(precip_mm, PVR_json_get_value(response, "precip_mm"));
    strcpy(cloud, PVR_json_get_value(response, "cloud"));

    char tui_title[60];
    snprintf(tui_title, 60, "%s%s%s", "-Weather ", VERSION, " ------------------------------------------");
    printf("%s\n", tui_title);
    
    printf("🌎 %s. %s.\n", country, region);
    printf("🕑 %s\n", last_updated); // TODO: change date format from "YYYY-MM-DD HH:MM" to "DD.MM.YYYY HH:MM"
    printf("%s %s\n", weather_icon, weather_condition);
    printf("🌡️ %s˚c (%s)˚f [Feels %s˚c (%s˚f)]\n",
           temp_c,
           temp_f,
           feelslike_c,
           feelslike_f);
    printf("🍃 %s km\\h %s [Feels %s˚c (%s˚f)]\n",
           wind_kph,
           wind_dir, 
           windchill_c,
           windchill_f);
    printf("💧 %s \n", humidity);
    printf("☔ %s mm\n", precip_mm);
    printf("☁️ %s \n", cloud);
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

void change_date_fromat(char *date) {
    char year[5];
    char month[3];
    char day[3];
    char hours[3];
    char minutes[3];
    
    // NOTE: Oringinal date format and indecies
    // 0123456789012345
    // YYYY-MM-DD HH:MM
    snprintf(day,     3, "%s", date+8);
    snprintf(month,   3, "%s", date+5);
    snprintf(year,    5, "%s", date+0);
    snprintf(hours,   3, "%s", date+11);
    snprintf(minutes, 3, "%s", date+14);

    snprintf(date, strlen(date)+1, "%s.%s.%s %s:%s", day, month, year, hours, minutes);
}

#define PVR_JSONP_IMPLEMENTATION
#include "../libs/pvrlib_jsonp/pvr_jsonp.h"
#define PVR_STRING_IMPLEMENTATION
#include "../libs/pvrlib_string/pvr_string.h"
