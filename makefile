SRC = src/*.c
OUT_DEBUG = -o out/debug/ucWeather.exe
OUT_RELEASE = -o out/release/ucWeather.exe
FLAGS = -Wall -Wextra -pedantic -std=c99
FLAGS_RELEASE = -std=c99

CURL_PATHS = -I D:\source_code\c\ucWeather\libs\curl\include -L D:\source_code\c\ucWeather\libs\curl\lib
CURL_FLAGS = -lcurl# -lws2_32 -lwinmm -lcrypt32 -lwldap32 -lbcrypt -lzstd -lnghttp2 -lssh2 -lssl -lcrypto -lz

make: $(SRC)
	gcc $(SRC) $(CURL_PATHS) $(CURL_FLAGS) $(OUT_DEBUG) $(FLAGS) -g

release: $(SRC)
	gcc $(SRC) $(CURL_PATHS) $(CURL_FLAGS) $(OUT_RELEASE) $(FLAGS_RELEASE)
