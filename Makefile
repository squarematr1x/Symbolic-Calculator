CXX := -g++
CXX_FLAGS := -std=c++17 -Wall -Wextra -g
LD_FLAGS := -L/usr/lib -lstdc++ -lm
LD_TEST_FLAGS := -L /usr/local/lib -lpthread
BUILD := build
OBJ_DIR := $(BUILD)/obj
TEST_OBJ_DIR := $(BUILD)/test_obj
APP_DIR := $(BUILD)/app
APP_TEST_DIR := $(BUILD)/test
TARGET := yaasc
GTEST_INCLUDE := -I /usr/include -I /usr/src/gtest
SRC := $(wildcard YAASC/src/*.cpp)
TEST_SRC := $(wildcard YAASC/tests/*.cpp)
GTEST_SRC := /usr/src/gtest/src/gtest_main.cc /usr/src/gtest/src/gtest-all.cc

OBJECTS := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
TEST_OBJECTS := $(filter-out $(OBJ_DIR)/YAASC/src/main.o, $(OBJECTS)) $(TEST_SRC:%.cpp=$(TEST_OBJ_DIR)/%.o)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) $(INCLUDE) -c $< -MMD -o $@

$(TEST_OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) $(INCLUDE) -c $< -MMD -o $@

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LD_FLAGS)

$(APP_TEST_DIR)/$(TARGET): $(TEST_OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) -o $(APP_TEST_DIR)/$(TARGET) $^ $(GTEST_SRC) $(GTEST_INCLUDE) $(LD_TEST_FLAGS)

.PHONY: all build run clean test run-tests

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(TEST_OBJ_DIR)

run: all
	./$(APP_DIR)/$(TARGET)

test: build $(APP_TEST_DIR)/$(TARGET)

run-tests: test
	./$(APP_TEST_DIR)/$(TARGET)

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*
	-@rm -rvf $(TEST_OBJ_DIR)/*
	-@rm -rvf $(APP_TEST_DIR)/*

