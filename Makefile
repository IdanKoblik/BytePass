CXX = g++
CXXFLAGS = -std=c++17 -pthread -O0 -fprofile-arcs -ftest-coverage
LDFLAGS = -fprofile-arcs -ftest-coverage
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

OBJS_NOT_MAIN = $(filter-out main.o,$(OBJS))

all: $(PROTO_SRCS) $(PROTO_HDRS) $(TARGET)

$(PROTO_SRCS) $(PROTO_HDRS): $(PROTO_FILES)
	$(PROTOC) $(PROTOC_FLAGS) --cpp_out=. $(PROTO_FILES)

$(TARGET): $(SRCS) $(PROTO_SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) `$(PKG_CONFIG) --cflags --libs protobuf` -o $(TARGET)

$(TEST_TARGET): $(TEST_OBJS) $(OBJS_NOT_MAIN)
	$(CXX) $(CXXFLAGS) $(TEST_OBJS) $(OBJS_NOT_MAIN) `pkg-config --cflags --libs protobuf gtest gtest_main` -o $(TEST_TARGET)

test/%.o: test/%.cpp
	$(CXX) $(CXXFLAGS) -I. `pkg-config --cflags protobuf gtest` -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I. `pkg-config --cflags protobuf` -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

coverage: test
	lcov --capture --directory . --output-file coverage.info \
	     --rc geninfo_unexecuted_blocks=1 \
	     --ignore-errors inconsistent,unused
	# Remove generated protobuf + system headers
	lcov --remove coverage.info "*/filechunk.pb.*" "/usr/*" --output-file coverage.info
	genhtml coverage.info --output-directory coverage-report
	find . -name "*.gcda" -o -name "*.gcno" -delete

clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o test/*.o *.gcda *.gcno *.gcov coverage.info
	rm -rf coverage-report

