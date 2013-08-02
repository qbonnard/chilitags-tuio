# should be either OSC_HOST_BIG_ENDIAN or OSC_HOST_LITTLE_ENDIAN
# Apple: OSC_HOST_BIG_ENDIAN
# Win32: OSC_HOST_LITTLE_ENDIAN
# i386 LinuX: OSC_HOST_LITTLE_ENDIAN

ENDIANESS=OSC_HOST_LITTLE_ENDIAN
PLATFORM=$(shell uname)

FRAMEWORKS = -lchilitags -lopencv_highgui

CHILITAGS_TUIO = ChilitagsTuio

INCLUDES = -I./TUIO -I./oscpack
CFLAGS  = -Wall -O3 
#CFLAGS  = -g -Wall -O3 
CXXFLAGS = $(CFLAGS) $(INCLUDES) -D$(ENDIANESS)

ifeq ($(PLATFORM), Darwin)
	FRAMEWORKS =  -framework chilitags -framework opencv_highgui
endif

CHILITAGS_TUIO_SOURCES = ChilitagsTuio.cpp
CHILITAGS_TUIO_OBJECTS = ChilitagsTuio.o

TUIO_SOURCES = ./TUIO/TuioServer.cpp ./TUIO/TuioTime.cpp
OSC_SOURCES = ./oscpack/osc/OscTypes.cpp ./oscpack/osc/OscOutboundPacketStream.cpp ./oscpack/osc/OscReceivedElements.cpp ./oscpack/osc/OscPrintReceivedElements.cpp ./oscpack/ip/posix/NetworkingUtils.cpp ./oscpack/ip/posix/UdpSocket.cpp

COMMON_SOURCES = $(TUIO_SOURCES) $(OSC_SOURCES)
COMMON_OBJECTS = $(COMMON_SOURCES:.cpp=.o)

all: ChilitagsTuio

ChilitagsTuio:	$(COMMON_OBJECTS) $(CHILITAGS_TUIO_OBJECTS)
	$(CXX) -o $(CHILITAGS_TUIO) $+ $(FRAMEWORKS) -lpthread

clean:
	rm -rf $(CHILITAGS_TUIO) $(COMMON_OBJECTS) $(CHILITAGS_TUIO_OBJECTS)
