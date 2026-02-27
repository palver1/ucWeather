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
void wind_dir_to_arrow(char *wind_dir);
void save_history_json(char* weather_json);
void load_history_json();
char* get_path_root();


/* MAIN PROCESS */
int main(int argc, char *argv[]) {   
    char API[50];
    char URL[200];
    char city_name[30] = "New York";

    if (argc > 1) {strcpy(city_name, argv[1]);}

    strcpy(city_name, PVR_replace(city_name, ' ', '_'));

    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    /* MAKE ROOT PATH ============================================================================== */
    /* Get exe path*/
    char* path_root = get_path_root();

    /* Make the path to api.key file */
    char path_file_api[MAX_PATH];
    #ifdef DEBUG
    snprintf(path_file_api, MAX_PATH, "%s%s", path_root, "..\\api.key");
    #else // RELEASE
    snprintf(path_file_api, MAX_PATH, "%s%s", path_root, "api.key");
    #endif

    /* MAKE URL ============================================================================== */
    FILE *file_api = fopen(path_file_api, "r");
    if (file_api == NULL) {
        perror("api.key file not found. Create one?\n"
               "Yes [y] | No [n]\n");

        char answer = tolower(getchar());
        if (answer == 'y') {
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

    snprintf(URL, 200, "%s%s%s%s", "http://api.weatherapi.com/v1/current.json?key=", API, "&q=", city_name);

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
    /* SOTRE WEATHER DATA */
    struct weather_data {
        char country[30];
        char region[30];
        char last_updated[30];
        char weather_icon[15];
        char weather_condition[100];
        char temp_c[30];
        char temp_f[30];
        char feelslike_c[30];
        char feelslike_f[30];
        char wind_kph[30];
        char wind_dir[30];
        char windchill_c[30];
        char windchill_f[30];
        char humidity[30];
        char precip_mm[30];
        char cloud[30];
    } weather_data;


    strcpy(weather_data.country, PVR_json_get_value(response, "country"));
    strcpy(weather_data.region, PVR_json_get_value(response, "region"));
    strcpy(weather_data.last_updated, PVR_json_get_value(response, "last_updated"));
    strcpy(weather_data.weather_condition, PVR_json_get_value(response, "text"));
    strcpy(weather_data.temp_c, PVR_json_get_value(response, "temp_c"));
    strcpy(weather_data.temp_f, PVR_json_get_value(response, "temp_f"));
    strcpy(weather_data.feelslike_c, PVR_json_get_value(response, "feelslike_c"));
    strcpy(weather_data.feelslike_f, PVR_json_get_value(response, "feelslike_f"));
    strcpy(weather_data.wind_kph, PVR_json_get_value(response, "wind_kph"));
    strcpy(weather_data.wind_dir, PVR_json_get_value(response, "wind_dir"));
    strcpy(weather_data.windchill_c, PVR_json_get_value(response, "windchill_c"));
    strcpy(weather_data.windchill_f, PVR_json_get_value(response, "windchill_f"));
    strcpy(weather_data.humidity, PVR_json_get_value(response, "humidity"));
    strcpy(weather_data.precip_mm, PVR_json_get_value(response, "precip_mm"));
    strcpy(weather_data.cloud, PVR_json_get_value(response, "cloud"));

    define_weather_icon(weather_data.weather_condition, weather_data.weather_icon);
    change_date_fromat(weather_data.last_updated);
    wind_dir_to_arrow(weather_data.wind_dir);

    char tui_title[60];
    snprintf(tui_title, 60, "%s%s%s", "-Weather ", VERSION, " ------------------------------------------");


    /* PRINT THE WEATHER DATA */
    printf("%s\n", tui_title);
    
    printf("🌎 %s. %s.\n", weather_data.country, weather_data.region);
    printf("🕑 %s\n", weather_data.last_updated);
    printf("%s %s\n", weather_data.weather_icon, weather_data.weather_condition);
    printf("🌡️ %s˚c (%s)˚f  Feels %s˚c (%s˚f)\n",
           weather_data.temp_c,
           weather_data.temp_f,
           weather_data.feelslike_c,
           weather_data.feelslike_f);
    printf("🍃 %s km\\h %s       Feels %s˚c (%s˚f)\n",
           weather_data.wind_kph,
           weather_data.wind_dir, 
           weather_data.windchill_c,
           weather_data.windchill_f);
    printf("💧 %s \n", weather_data.humidity);
    printf("☔ %s mm\n", weather_data.precip_mm);
    printf("☁️ %s \n", weather_data.cloud);
    printf("--------------------------------------------------");


    /* UPDATE HISTORY FILE */
    // save_history_json();

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
    else if (strcmp(weather_condition, "Patchy light rain in area strcpy(with thunder") == 0) {
        strcpy(weather_icon, "🌩️"); }
    else if (strcmp(weather_condition, "Moderate or heavy rain in area strcpy(with thunder") == 0) {
        strcpy(weather_icon, "🌩️"); }
    else if (strcmp(weather_condition, "Patchy light snow in area strcpy(with thunder") == 0) {
        strcpy(weather_icon, "🌩️"); }
    else if (strcmp(weather_condition, "Moderate or heavy snow in area strcpy(with thunder") == 0) {
        strcpy(weather_icon, "🌩️"); }
    else { // Default icon
        strcpy(weather_icon, "🪟");
    }
}

void wind_dir_to_arrow(char *wind_dir) {
    char wind_dir_arrow[10];

    if (strcmp(wind_dir, "N") == 0)    strcpy(wind_dir_arrow, "↑");
    else if (strcmp(wind_dir, "NNE") == 0) strcpy(wind_dir_arrow, "↗");
    else if (strcmp(wind_dir, "NE") == 0)  strcpy(wind_dir_arrow, "↗");
    else if (strcmp(wind_dir, "ENE") == 0) strcpy(wind_dir_arrow, "→↗");
    else if (strcmp(wind_dir, "E") == 0)   strcpy(wind_dir_arrow, "→");
    else if (strcmp(wind_dir, "ESE") == 0) strcpy(wind_dir_arrow, "→↘");
    else if (strcmp(wind_dir, "SE") == 0)  strcpy(wind_dir_arrow, "↘");
    else if (strcmp(wind_dir, "SSE") == 0) strcpy(wind_dir_arrow, "↓↘");
    else if (strcmp(wind_dir, "S") == 0)   strcpy(wind_dir_arrow, "↓");
    else if (strcmp(wind_dir, "SSW") == 0) strcpy(wind_dir_arrow, "↓↙");
    else if (strcmp(wind_dir, "SW") == 0)  strcpy(wind_dir_arrow, "↙");
    else if (strcmp(wind_dir, "WSW") == 0) strcpy(wind_dir_arrow, "←↙");
    else if (strcmp(wind_dir, "W") == 0)   strcpy(wind_dir_arrow, "←");
    else if (strcmp(wind_dir, "WNW") == 0) strcpy(wind_dir_arrow, "←↖");
    else if (strcmp(wind_dir, "NW") == 0)  strcpy(wind_dir_arrow, "↖");
    else if (strcmp(wind_dir, "NNW") == 0) strcpy(wind_dir_arrow, "↑↖");

    strcpy(wind_dir, wind_dir_arrow);
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

char* get_path_root() {
    char* exe_path = (char*)malloc(MAX_PATH);
    int exe_path_len = GetModuleFileNameA(NULL, exe_path, MAX_PATH);
    if (exe_path_len == 0) {
        perror("Error: couldn't get the program's path."); exit(EXIT_FAILURE);
    } else if (exe_path_len >= MAX_PATH) {
        perror("Error: path is truncated (exe_path is too small)"); exit(EXIT_FAILURE);
    }

    /* Remove .exe file from the path */
    int last_slash_index;
    for (int i = 0; i < exe_path_len; i++) {
        if (exe_path[i] == '\\') {
            last_slash_index = i+1;
        }
    }
    exe_path[last_slash_index] = '\0';
    
    return exe_path;
}

void save_history_json(char* weather_json) {
    #ifdef DEBUG
    char history_json_filename[] = "..\\history.json";
    #else
    char history_json_filename[] = "history.json";
    #endif

    char path_history_json[MAX_PATH];
    snprintf(path_history_json, MAX_PATH, "%s%s", get_path_root(), history_json_filename);

    /* Check if the file exists */
    bool file_exists = false;
    if (FILE *fp = fopen(path_history_json, "r")) {
        file_exists = true;
        fclose(fp);
    }

    FILE *history_json = fopen(path_history_json, "a");
    if (history_json == NULL) {perror("Failed to create history.json"); exit(EXIT_FAILURE);}

    fseek(history_json, 0, SEEK_END);
    size_t history_json_size = ftell(history_json);

    // char buffer_history_json[history_json_size];
    size_t date_size = strlen(PVR_json_get_value(weather_json, "last_updated"));
    char date_last_update[date_size];
    size_t wd_entry_size = date_size + strlen(weather_json) + 5; // number is for additional symbols (:,}\0)
    char weather_data_entry[history_json_size];

    // fread(buffer_history_json, 1, history_json_size, history_json);
    strcpy(date_last_update, PVR_json_get_value(weather_json, "last_updated"));
    snprintf(weather_data_entry, history_json_size + wd_entry_size, ", %s:%s\n}", date_last_update, weather_json);

    // TODO: append data to the file

    fclose(history_json);
}

void load_history_json() {
}


#define PVR_JSONP_IMPLEMENTATION
#include "../libs/pvrlib_jsonp/pvr_jsonp.h"
#define PVR_STRING_IMPLEMENTATION
#include "../libs/pvrlib_string/pvr_string.h"
