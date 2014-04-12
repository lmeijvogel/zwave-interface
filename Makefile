CC=g++
CFLAGS=-c -Wall
EXECUTABLE=main
BOOST_INCLUDES= -I ./boost_1_55_0
ZWAVE_BASE_DIR=../open-zwave-read-only
ZWAVE_INCLUDES= -I ./lib -I $(ZWAVE_BASE_DIR)/cpp/src -I $(ZWAVE_BASE_DIR)/cpp/src/command_classes/ -I $(ZWAVE_BASE_DIR)/cpp/src/value_classes/ \
	-I $(ZWAVE_BASE_DIR)/cpp/src/platform/ -I $(ZWAVE_BASE_DIR)/cpp/src/platform/unix -I $(ZWAVE_BASE_DIR)/cpp/tinyxml/ -I $(ZWAVE_BASE_DIR)/cpp/hidapi/hidapi/ \
	-I $(ZWAVE_BASE_DIR)/cpp/examples/ -I $(ZWAVE_BASE_DIR)/cpp/build/
INCLUDES=$(ZWAVE_INCLUDES) $(BOOST_INCLUDES)

all: main.o Common.o SocketReader.o Connection.o
	$(CC) $? -lopenzwave -l:/usr/local/include/libboost_system.so -o main

Common.o: Common.cpp Common.h
	$(CC) $(CFLAGS) $< -lopenzwave $(INCLUDES)

main.o: main.cpp
	$(CC) $(CFLAGS) $< -lopenzwave $(INCLUDES)

SocketReader.o: SocketReader.cpp SocketReader.h
	$(CC) $(CFLAGS) $< -lboost_system $(INCLUDES)

Connection.o: Connection.cpp Connection.h
	$(CC) $(CFLAGS) $< -lboost_system $(INCLUDES)

clean:
	rm -f main *.o *.h.gch
