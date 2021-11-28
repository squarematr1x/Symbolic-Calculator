CXX      := -g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g
LDFLAGS  := -L/usr/lib -lstdc++ -lm
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
TARGET   := yaasc
SRC      := $(wildcard YAASC/src/*.cpp)
OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

.PHONY: all build clean

all: build $(BUILD)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -MMD -o $@

$(BUILD)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(BUILD)/$(TARGET) $^ $(LDFLAGS)

build:
	@mkdir -p $(OBJ_DIR)

clean:
	@rm -rvf $(OBJ_DIR)
	@rm -rvf $(TEST_DIR)
