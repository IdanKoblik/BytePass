PROGRAM_NAME = echo-ft
CC = gcc
CXX = g++
RM = rm -f

CXXFLAGS = --std=c++17 -pthread -g -O3 -MMD -MP
CPPFLAGS = -I/usr/local/include/
LDFLAGS = -L/usr/local/lib -L/usr/local/lib/boost
LDLIBS = -lrtaudio -lboost_system -lprotobuf

SRC_DIR = .
OBJ_DIR = obj
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*.pb.cc)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
OBJS := $(patsubst $(SRC_DIR)/%.pb.cc, $(OBJ_DIR)/%.pb.o, $(OBJS))
DEPS = $(OBJS:.o=.d)

TARGET = $(BUILD_DIR)/$(PROGRAM_NAME)

all: $(TARGET)

# Link objects into final binary
$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

# Compile .cpp and .pb.cc to .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.pb.o: $(SRC_DIR)/%.pb.cc
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Include dependency files
-include $(DEPS)

# Clean
clean:
	$(RM) -r $(OBJ_DIR) $(BUILD_DIR)

# Install
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/$(PROGRAM_NAME)

# Uninstall
uninstall:
	$(RM) /usr/local/bin/$(PROGRAM_NAME)

.PHONY: all clean install uninstall

