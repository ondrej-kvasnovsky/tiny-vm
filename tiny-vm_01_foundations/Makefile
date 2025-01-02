CC = gcc
CFLAGS = -Wall -pthread
LDFLAGS = -pthread

# Directory structure
BUILD_DIR = build
SRC_DIR = .

# Source files and their corresponding object files in build directory
SRCS = tiny_vm.c main.c
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)
TARGET = $(BUILD_DIR)/tiny_vm

.PHONY: all clean mkdir

all: mkdir $(TARGET)

# Create build directory if it doesn't exist
mkdir:
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Pattern rule for object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c tiny_vm.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)