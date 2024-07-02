##
# ADF4368
#
# @file
# @version 0.1


UNITY_ROOT := lib/Unity

CFLAGS := -g -Wall -std=c11
SRC_DIR := src
OBJECTS := $(patsubst %.c,%.o,$(wildcard $(SRC_DIR)/*.c))

TEST_DIR := test
INC_DIRS=-Isrc -I$(TEST_DIR) -I$(UNITY_ROOT)/src
TESTS := test_cb
UNITY = $(UNITY_ROOT)/src/unity.c

TEST_TARGETS:= $(patsubst %.c,%.out,$(wildcard $(TEST_DIR)/*.c))

TARGET_EXTENSION=.out

CLEANUP = rm -f

all: clean test
	@echo $(TEST_OBJECTS)

clean:
	@echo Cleaning
	@rm -f $(SRC_DIR)/*.o
	@rm -f $(TEST_DIR)/*.out
	@rm -f $(TEST_TARGETS)

$(OBJECTS): %.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@

$(SRC_DIR)/main: %: %.c $(OBJECTS)
	@$(CC) $(CFLAGS) $(INC_DIRS) $< $(OBJECTS) -o $@
	@chmod u+x $(SRC_DIR)/main

$(TEST_TARGETS): %.out: %.c $(OBJECTS)
	@echo $(TEST_TARGETS)
	@$(CC) $(CFLAGS) $(INC_DIRS) -include $(TEST_DIR)/unity_config.h -o $@ $^ $(UNITY)
	@echo ============================================
	@echo Running $<
	@echo ============================================
	-./$@

test: $(TEST_TARGETS)

# end
