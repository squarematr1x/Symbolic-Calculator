CXX = g++
CXXFLAGS = -std=c++17
RM = rm -rf
BUILD_DIR = ./build

all:
	make clean
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/yaasc ./YAASC/src/*.cpp

clean:
	$(RM) $(BUILD_DIR)
