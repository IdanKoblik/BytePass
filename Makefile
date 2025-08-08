CXX = g++
CXXFLAGS = -std=c++17 -pthread
PROTOC = protoc
PKG_CONFIG = pkg-config
PROTOC_FLAGS = -I=.
PROTO_FILES = filechunk.proto
PROTO_SRCS = filechunk.pb.cc
PROTO_HDRS = filechunk.pb.h
SRCS = driver.cpp main.cpp server.cpp $(PROTO_SRCS)
OBJS = $(SRCS:.cpp=.o)
TARGET = echo-ft

all: $(TARGET)

$(PROTO_SRCS) $(PROTO_HDRS): $(PROTO_FILES)
	$(PROTOC) $(PROTOC_FLAGS) --cpp_out=. $(PROTO_FILES)

$(TARGET): $(SRCS) $(PROTO_SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) `$(PKG_CONFIG) --cflags --libs protobuf` -o $(TARGET)

clean:
	rm -f $(TARGET) *.o $(PROTO_SRCS) $(PROTO_HDRS)
