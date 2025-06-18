# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I$(INC_DIR)

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Source and header files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))
EXEC = $(BUILD_DIR)/minigit

# Default target
all: $(BUILD_DIR) $(EXEC)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile .cpp files into .o object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link all object files into final executable
$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Clean build directory
clean:
	rm -rf $(BUILD_DIR)
