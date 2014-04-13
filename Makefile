CC=g++
CFLAGS=-c -Wall
EXECUTABLE=main
ZWAVE_INCLUDES=
ZWAVE_BASE_DIR=../open-zwave-read-only
ZWAVE_INCLUDES= -I $(ZWAVE_BASE_DIR)/cpp/src -I $(ZWAVE_BASE_DIR)/cpp/src/command_classes/ -I $(ZWAVE_BASE_DIR)/cpp/src/value_classes/ \
	-I $(ZWAVE_BASE_DIR)/cpp/src/platform/ -I $(ZWAVE_BASE_DIR)/cpp/src/platform/unix -I $(ZWAVE_BASE_DIR)/cpp/tinyxml/ -I $(ZWAVE_BASE_DIR)/cpp/hidapi/hidapi/ \
	-I $(ZWAVE_BASE_DIR)/cpp/examples/ -I $(ZWAVE_BASE_DIR)/cpp/build/

all: main.o
	$(CC) main.o -lopenzwave -o main

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp -lopenzwave $(ZWAVE_INCLUDES)

#$(EXECUTABLE):
	#$(MAKE) -C $(ZWAVE_BASE_DIR)/cpp/build/ -$(MAKEFLAGS)
