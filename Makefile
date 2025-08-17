CXX = g++
CXXFLAGS = -std=c++17 -pthread -g -O0 -fprofile-arcs -ftest-coverage --coverage
LDFLAGS = -fprofile-arcs -ftest-coverage --coverage
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

$(TEST_TARGET): clean-coverage $(TEST_OBJS) $(OBJS_NOT_MAIN)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(TEST_OBJS) $(OBJS_NOT_MAIN) `pkg-config --cflags --libs protobuf gtest gtest_main` -o $(TEST_TARGET)

test/%.o: test/%.cpp
	$(CXX) $(CXXFLAGS) -I. `pkg-config --cflags protobuf gtest` -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I. `pkg-config --cflags protobuf` -c $< -o $@

# Separate target for building with coverage
coverage-build: clean-coverage $(PROTO_SRCS) $(PROTO_HDRS)
	$(MAKE) CXXFLAGS="$(CXXFLAGS)" LDFLAGS="$(LDFLAGS)" $(TEST_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

coverage: coverage-build
	@echo "Running tests and generating coverage..."
	./$(TEST_TARGET)
	@echo "Generating coverage report..."
	@if command -v lcov >/dev/null 2>&1; then \
		lcov --capture --directory . --output-file coverage.info \
			--rc geninfo_unexecuted_blocks=1 \
			--ignore-errors inconsistent,unused,gcov,source,mismatch; \
		lcov --remove coverage.info "*/filechunk.pb.*" "/usr/*" "*test*" --output-file coverage.info \
			--ignore-errors empty,unused; \
		if [ -s coverage.info ]; then \
			genhtml coverage.info --output-directory coverage-report \
				--ignore-errors empty; \
			echo "Coverage report generated in coverage-report/"; \
		else \
			echo "Warning: No coverage data found. Creating empty report."; \
			mkdir -p coverage-report; \
			echo "<html><body><h1>No Coverage Data</h1><p>No valid coverage data was found.</p></body></html>" > coverage-report/index.html; \
		fi; \
	else \
		echo "Error: lcov is not installed. Please install lcov to generate coverage reports."; \
		exit 1; \
	fi

clean-coverage:
	@echo "Cleaning coverage files..."
	find . -name "*.gcda" -delete 2>/dev/null || true
	find . -name "*.gcno" -delete 2>/dev/null || true
	rm -f coverage.info

clean: clean-coverage
	rm -f $(TARGET) $(TEST_TARGET) *.o test/*.o *.gcov coverage.info
	rm -rf coverage-report

debug-coverage: coverage-build
	@echo "=== Coverage Debug Info ==="
	@echo "GCC Version: $(gcc --version | head -1)"
	@echo "LCOV Version: $(lcov --version 2>/dev/null || echo 'Not installed')"
	@echo "GCOV Files found:"
	@find . -name "*.gcda" -o -name "*.gcno" | head -10
	@echo "Running test..."
	./$(TEST_TARGET)
	@echo "Coverage files after test run:"
	@find . -name "*.gcda" | head -10
	@echo "Attempting coverage capture with verbose output..."
	lcov --capture --directory . --output-file coverage-debug.info \
		--rc geninfo_unexecuted_blocks=1 \
		--ignore-errors inconsistent,unused,gcov,source,mismatch \
		|| echo "Coverage capture failed"

.PHONY: all test coverage clean clean-coverage coverage-build debug-coverage
