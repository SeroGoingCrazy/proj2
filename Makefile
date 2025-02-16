CXX       = g++
CXXFLAGS  = -std=c++20 -Wall -Wextra $(addprefix -I,$(INC_DIRS)) -I/opt/homebrew/opt/googletest/include -MMD -MP
LDFLAGS   = -L/opt/homebrew/opt/googletest/lib
LDLIBS    = -lgtest -lgtest_main -pthread

TARGET_DIR = ./bin
TARGET = $(TARGET_DIR)/test

SRC_DIRS = ./src
TEST_SRC_DIRS = ./testsrc

SRCS = $(shell find $(SRC_DIRS) -name '*.cpp')
TEST_SRCS = $(shell find $(TEST_SRC_DIRS) -name '*.cpp')

OBJS = $(patsubst ./%, $(TARGET_DIR)/%, $(SRCS:.cpp=.o))
TEST_OBJS = $(patsubst ./%, $(TARGET_DIR)/%, $(TEST_SRCS:.cpp=.o))
DEPS = $(OBJS:.o=.d) $(TEST_OBJS:.o=.d)

-include $(DEPS)

INC_DIRS = ./include $(shell find $(SRC_DIRS) $(TEST_SRC_DIRS) -type d 2>/dev/null)

.PHONY: all clean test

all: $(TARGET)

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

$(TARGET): $(OBJS) $(TEST_OBJS) | $(TARGET_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(TARGET_DIR)/%.o: ./%.cpp | $(TARGET_DIR)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(TARGET_DIR)
