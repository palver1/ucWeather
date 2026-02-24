SRC = src/*.c
OUT_DEBUG = out/debug/ucWeather.exe
OUT_RELEASE = out/release/ucWeather.exe
FLAGS = -DDEBUG -Wall -Wextra -pedantic-errors -std=c99
FLAGS_RELEASE = -std=c99 -DNDEBUG -O2

CURL_PATHS = -I.\libs\curl\include -L.\libs\curl\lib
CURL_FLAGS = -lcurl# -lws2_32 -lwinmm -lcrypt32 -lwldap32 -lbcrypt -lzstd -lnghttp2 -lssh2 -lssl -lcrypto -lz

make: $(SRC)
	gcc $(SRC) $(CURL_PATHS) $(CURL_FLAGS) -o $(OUT_DEBUG) $(FLAGS) -g

release: $(SRC)
	gcc $(SRC) $(CURL_PATHS) $(CURL_FLAGS) -o $(OUT_RELEASE) $(FLAGS_RELEASE)

run:
	make && $(OUT_DEBUG)
