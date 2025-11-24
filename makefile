SRC = src/ucWeather.c
OUT = -o out/ucWeather.exe
FLAGS = -Wall -Wextra -pedantic -std=c11 -g

CURL_PATHS = -I D:\source_code\c\ucWeather\libs\curl\include -L D:\source_code\c\ucWeather\libs\curl\lib
CURL_FLAGS = -lcurl# -lws2_32 -lwinmm -lcrypt32 -lwldap32 -lbcrypt -lzstd -lnghttp2 -lssh2 -lssl -lcrypto -lz

make: $(SRC)
	gcc $(SRC) $(CURL_PATHS) $(CURL_FLAGS) $(OUT) $(FLAGS)


