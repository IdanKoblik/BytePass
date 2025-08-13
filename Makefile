CXX = g++
CXXFLAGS = -std=c++17 -pthread
PROTOC = protoc
PKG_CONFIG = pkg-config
PROTOC_FLAGS = -I=.
PROTO_FILES = filechunk.proto
PROTO_SRCS = filechunk.pb.cc
PROTO_HDRS = filechunk.pb.h
SRCS = $(wildcard *.cpp) $(PROTO_SRCS)
OBJS = $(SRCS:.cpp=.o)
TARGET = echo-ft

TEST_SRCS = $(wildcard test/*.cpp)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)
TEST_TARGET = test/test_runner

# Exclude main.o from tests linking
OBJS_NOT_MAIN = $(filter-out main.o,$(OBJS))

all: $(TARGET)

$(PROTO_SRCS) $(PROTO_HDRS): $(PROTO_FILES)
	$(PROTOC) $(PROTOC_FLAGS) --cpp_out=. $(PROTO_FILES)

$(TARGET): $(SRCS) $(PROTO_SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) `$(PKG_CONFIG) --cflags --libs protobuf` -o $(TARGET)

$(TEST_TARGET): $(TEST_OBJS) $(OBJS_NOT_MAIN)
	$(CXX) $(CXXFLAGS) -fprofile-arcs -ftest-coverage $(TEST_OBJS) $(OBJS_NOT_MAIN) `pkg-config --cflags --libs protobuf gtest gtest_main` -o $(TEST_TARGET)

test/%.o: test/%.cpp
	$(CXX) $(CXXFLAGS) -I. `pkg-config --cflags protobuf gtest` -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I. `pkg-config --cflags protobuf` -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o test/*.o $(PROTO_SRCS) $(PROTO_HDRS) *.gcda *.gcno

