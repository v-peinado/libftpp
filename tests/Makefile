# Tests Makefile for libftpp
# This Makefile handles compilation and execution of individual tests

# Style colors
GREEN	= \033[92;5;118m
YELLOW	= \033[93;5;226m
GRAY	= \033[33;2;37m
RESET	= \e[0m
CURSIVE	= \033[33;3m
RED		= \033[31m
BLUE	= \033[34m

# Directories
LIB_DIR		= ..
TEST_SRC_DIR	= src
TEST_BIN_DIR	= bin
TEST_OBJ_DIR	= obj

# Library settings
LIB_NAME	= libftpp.a
LIB_PATH	= $(LIB_DIR)/$(LIB_NAME)

# Compiler settings
CC		= c++
CFLAGS		= -std=c++20 -Wall -Wextra -Werror
# CFLAGS	+= -g3 -fsanitize=address
RM		= rm -f

# Include paths
INC		= -I $(LIB_DIR) -I $(LIB_DIR)/include

# Test source files (using original main_*.cpp naming)
TEST_SOURCES	= $(wildcard $(TEST_SRC_DIR)/main_*.cpp)
TEST_NAMES	= $(basename $(notdir $(TEST_SOURCES)))
TEST_BINS	= $(addprefix $(TEST_BIN_DIR)/, $(TEST_NAMES))

# Default target
all: $(LIB_PATH) dirs $(TEST_BINS)
	@printf "$(GREEN)All tests compiled successfully!$(RESET)\n"

# Create necessary directories
dirs:
	@mkdir -p $(TEST_BIN_DIR)
	@mkdir -p $(TEST_OBJ_DIR)

# Library dependency - compile library if it doesn't exist or is out of date
$(LIB_PATH):
	@printf "$(YELLOW)Compiling library...$(RESET)\n"
	@$(MAKE) -C $(LIB_DIR)

# Generic rule for compiling test binaries
$(TEST_BIN_DIR)/%: $(TEST_SRC_DIR)/%.cpp $(LIB_PATH) | dirs
	@printf "$(BLUE)Compiling $*...$(RESET)\n"
	@$(CC) $(CFLAGS) $(INC) $< $(LIB_PATH) -o $@

# Individual test execution rules
test_pool: $(TEST_BIN_DIR)/main_pool
	@printf "$(GREEN)Running Pool test:$(RESET)\n"
	@./$(TEST_BIN_DIR)/main_pool

test_singleton: $(TEST_BIN_DIR)/main_singleton
	@printf "$(GREEN)Running Singleton test:$(RESET)\n"
	@./$(TEST_BIN_DIR)/main_singleton

test_state_machine: $(TEST_BIN_DIR)/main_state_machine
	@printf "$(GREEN)Running State Machine test:$(RESET)\n"
	@./$(TEST_BIN_DIR)/main_state_machine

test_observer: $(TEST_BIN_DIR)/main_observer
	@printf "$(GREEN)Running Observer test:$(RESET)\n"
	@./$(TEST_BIN_DIR)/main_observer

test_memento: $(TEST_BIN_DIR)/main_memento
	@printf "$(GREEN)Running Memento test:$(RESET)\n"
	@./$(TEST_BIN_DIR)/main_memento

test_data_buffer: $(TEST_BIN_DIR)/main_data_buffer
	@printf "$(GREEN)Running Data Buffer test:$(RESET)\n"
	@./$(TEST_BIN_DIR)/main_data_buffer

test_thread_safe_iostreamr: $(TEST_BIN_DIR)/main_thread_safe_iostream
	@printf "$(GREEN)Running test thread safe iostream test:$(RESET)\n"
	@./$(TEST_BIN_DIR)/main_thread_safe_iostream

# Run all tests
all_tests: $(TEST_BINS)
	@printf "$(GREEN)Running all tests:$(RESET)\n"
	@for test in $(TEST_BINS); do \
		printf "$(BLUE)Running $$test:$(RESET)\n"; \
		./$$test; \
		printf "\n"; \
	done

# Clean test objects and binaries
clean:
	@$(RM) -rf $(TEST_OBJ_DIR)
	@$(RM) -rf $(TEST_BIN_DIR)
	@printf "$(YELLOW)Test files: $(RED)Removed!$(RESET)\n"

# Clean everything including library
fclean: clean
	@$(MAKE) -C $(LIB_DIR) fclean
	@printf "$(YELLOW)Library: $(RED)Removed!$(RESET)\n"

# Rebuild everything
re: fclean all

# Force library rebuild
lib_rebuild:
	@$(MAKE) -C $(LIB_DIR) re

# Help target
help:
	@printf "$(GREEN)Available targets:$(RESET)\n"
	@printf "  $(BLUE)all$(RESET)               - Compile all tests\n"
	@printf "  $(BLUE)test_pool$(RESET)         - Run Pool test (main_pool)\n"
	@printf "  $(BLUE)test_singleton$(RESET)    - Run Singleton test (main_singleton)\n"
	@printf "  $(BLUE)test_state_machine$(RESET) - Run State Machine test (main_state_machine)\n"
	@printf "  $(BLUE)test_observer$(RESET)     - Run Observer test (main_observer)\n"
	@printf "  $(BLUE)test_memento$(RESET)      - Run Memento test (main_memento)\n"
	@printf "  $(BLUE)test_data_buffer$(RESET)  - Run Data Buffer test (main_data_buffer)\n"
	@printf "  $(BLUE)all_tests$(RESET)         - Run all tests sequentially\n"
	@printf "  $(BLUE)clean$(RESET)             - Clean test files\n"
	@printf "  $(BLUE)fclean$(RESET)            - Clean tests and library\n"
	@printf "  $(BLUE)re$(RESET)                - Rebuild everything\n"
	@printf "  $(BLUE)lib_rebuild$(RESET)       - Force library rebuild\n"

.PHONY: all dirs clean fclean re lib_rebuild help all_tests
.PHONY: test_pool test_singleton test_state_machine test_observer test_memento test_data_buffer