CC=g++
CFLAGS=-c -Wall
EXECUTABLE=zwave-interface
BOOST_INCLUDES= -I ./boost_1_55_0
ZWAVE_BASE_DIR=../open-zwave-read-only
ZWAVE_INCLUDES= -I $(ZWAVE_BASE_DIR)/cpp/src -I $(ZWAVE_BASE_DIR)/cpp/src/command_classes/ -I $(ZWAVE_BASE_DIR)/cpp/src/value_classes/ \
	-I $(ZWAVE_BASE_DIR)/cpp/src/platform/ -I $(ZWAVE_BASE_DIR)/cpp/src/platform/unix -I $(ZWAVE_BASE_DIR)/cpp/tinyxml/ -I $(ZWAVE_BASE_DIR)/cpp/hidapi/hidapi/ \
	-I $(ZWAVE_BASE_DIR)/cpp/examples/ -I $(ZWAVE_BASE_DIR)/cpp/build/
INCLUDES=$(ZWAVE_INCLUDES) $(BOOST_INCLUDES)

all: main.o Common.o TelnetServer.o CommandParser.o EventProcessor.o LightsController.o MyNode.o TimeService.o
	$(CC) $? -lopenzwave -l:/usr/local/include/libboost_system.so -o $(EXECUTABLE)

main.o: main.cpp
	$(CC) $(CFLAGS) $< -lopenzwave $(INCLUDES)

%.o: %.cpp %.h
	$(CC) $(CFLAGS) $< -lopenzwave $(INCLUDES)

clean:
	rm -f $(EXECUTABLE) *.o *.h.gch
