# Colors
GREEN  = \033[92;5;118m
YELLOW = \033[93;5;226m
GRAY   = \033[33;2;37m
RESET  = \e[0m
RED    = \033[31m
BLUE   = \033[34m

# Compiler settings
CC      = c++
CFLAGS  = -Wall -Wextra -Werror -std=c++14
RM      = rm -f

# Paths
LIB_DIR = ../libftpp
LIB     = $(LIB_DIR)/libftpp.a

# Include paths
INC     = -I$(LIB_DIR) -I$(LIB_DIR)/inc

# Automatically detect all main_*.cpp files
MAIN_FILES = $(wildcard main_*.cpp)

# Generate executable names: main_pool.cpp -> test_pool
TESTS = $(MAIN_FILES:main_%.cpp=test_%)

# Generate binary names for executables
BINARIES = $(MAIN_FILES:main_%.cpp=%)

# Default target - compile library and all tests
all: lib compile_all
	@echo "$(GREEN)All tests compiled successfully!$(RESET)"

# Compile library
lib:
	@echo "$(YELLOW)Compiling library...$(RESET)"
	@$(MAKE) -C $(LIB_DIR)

# Compile all tests without running
compile_all: $(BINARIES)

# Generic rule to compile any test from main_*.cpp
%: main_%.cpp $(LIB)
	@echo "$(BLUE)Compiling $@...$(RESET)"
	@$(CC) $(CFLAGS) $(INC) $< $(LIB) -o $@

# Generic rule to compile and run any test
test_%: % 
	@echo "$(GREEN)Running test_$*:$(RESET)"
	@echo "========================"
	@./$*
	@echo "========================"
	@echo ""

# Run all tests
run_all: compile_all
	@echo "$(YELLOW)Running all tests...$(RESET)\n"
	@for test in $(BINARIES); do \
		echo "$(GREEN)Running test_$$test:$(RESET)"; \
		echo "========================"; \
		./$$test; \
		echo "========================\n"; \
	done

# List available tests
list:
	@echo "$(YELLOW)Available tests:$(RESET)"
	@for test in $(MAIN_FILES); do \
		name=$$(echo $$test | sed 's/main_/test_/' | sed 's/.cpp//'); \
		echo "  $(GREEN)make $$name$(RESET) - Run $$test"; \
	done
	@echo ""
	@echo "$(YELLOW)Other commands:$(RESET)"
	@echo "  $(BLUE)make all$(RESET)      - Compile all tests"
	@echo "  $(BLUE)make run_all$(RESET)  - Run all tests"
	@echo "  $(BLUE)make clean$(RESET)    - Remove test executables"
	@echo "  $(BLUE)make fclean$(RESET)   - Clean everything"
	@echo "  $(BLUE)make re$(RESET)       - Rebuild everything"

# Clean test executables
clean:
	@$(RM) $(BINARIES)
	@echo "$(YELLOW)Test executables: $(RED)Removed!$(RESET)"

# Clean everything including library
fclean: clean
	@$(MAKE) -C $(LIB_DIR) fclean
	@echo "$(YELLOW)Library: $(RED)Removed!$(RESET)"

# Rebuild everything
re: fclean all

# Ensure library exists before compiling tests
$(LIB):
	@$(MAKE) -C $(LIB_DIR)

.PHONY: all lib compile_all run_all clean fclean re list $(TESTS)