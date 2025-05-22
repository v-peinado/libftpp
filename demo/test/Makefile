#Colors
GREEN	=	\033[92;5;118m
YELLOW	=	\033[93;5;226m
RED		= 	\033[31m
BLUE	=	\033[34m
CYAN	=	\033[36m
MAGENTA	=	\033[35m
BOLD	=	\033[1m
RESET	=	\e[0m

#Library
LIB_PATH = ../..
LIB_NAME = libftpp.a
LIB		= $(LIB_PATH)/$(LIB_NAME)

#Compiler
CC		= c++
CFLAGS	= -std=c++20 -Wall -Wextra -Werror
LIBS	= -L $(LIB_PATH) -lftpp
RM		= rm -f

#Includes
INC		= -I $(LIB_PATH) -I $(LIB_PATH)/include/

#Test programs
TESTS	= test_data_buffer test_pool test_memento test_observer test_singleton test_state_machine

all: $(LIB) $(TESTS)

# Help target
help:
	@printf "$(BOLD)$(CYAN)libftpp Test Suite - Available Commands$(RESET)\n"
	@printf "$(CYAN)=========================================$(RESET)\n\n"
	@printf "$(BOLD)$(YELLOW)📦 LIBRARY MANAGEMENT:$(RESET)\n"
	@printf "  $(GREEN)make lib$(RESET)           - Compile only the library\n"
	@printf "  $(GREEN)make lib-clean$(RESET)     - Clean library object files (keeps libftpp.a)\n"
	@printf "  $(GREEN)make lib-fclean$(RESET)    - Remove library completely\n"
	@printf "  $(GREEN)make lib-re$(RESET)        - Rebuild library from scratch\n\n"
	@printf "$(BOLD)$(YELLOW)🧪 TEST COMPILATION:$(RESET)\n"
	@printf "  $(GREEN)make$(RESET)               - Compile library + all tests\n"
	@printf "  $(GREEN)make test_data_buffer$(RESET) - Compile DataBuffer test only\n"
	@printf "  $(GREEN)make test_pool$(RESET)     - Compile Pool test only\n"
	@printf "  $(GREEN)make test_memento$(RESET)  - Compile Memento test only\n"
	@printf "  $(GREEN)make test_observer$(RESET) - Compile Observer test only\n"
	@printf "  $(GREEN)make test_singleton$(RESET) - Compile Singleton test only\n"
	@printf "  $(GREEN)make test_state_machine$(RESET) - Compile StateMachine test only\n\n"
	@printf "$(BOLD)$(YELLOW)🚀 TEST EXECUTION:$(RESET)\n"
	@printf "  $(GREEN)make run-data-buffer$(RESET) - Run DataBuffer tests\n"
	@printf "  $(GREEN)make run-pool$(RESET)        - Run Pool tests\n"
	@printf "  $(GREEN)make run-memento$(RESET)     - Run Memento tests\n"
	@printf "  $(GREEN)make run-observer$(RESET)    - Run Observer tests\n"
	@printf "  $(GREEN)make run-singleton$(RESET)    - Run Singleton tests\n"
	@printf "  $(GREEN)make run-state-machine$(RESET) - Run StateMachine tests\n"
	@printf "  $(GREEN)make run-all$(RESET)         - Run ALL tests in sequence\n\n"
	@printf "$(BOLD)$(YELLOW)🧹 CLEANING:$(RESET)\n"
	@printf "  $(GREEN)make clean$(RESET)         - Remove test programs only\n"
	@printf "  $(GREEN)make fclean$(RESET)        - Remove tests + library\n\n"
	@printf "$(BOLD)$(YELLOW)🔄 REBUILDING:$(RESET)\n"
	@printf "  $(GREEN)make re$(RESET)            - Rebuild everything (tests + library)\n"
	@printf "  $(GREEN)make re-tests$(RESET)      - Rebuild only tests (keep library)\n"
	@printf "  $(GREEN)make fullre$(RESET)        - Full rebuild (library from scratch + tests)\n\n"
	@printf "$(BOLD)$(YELLOW)📖 COMMON WORKFLOWS:$(RESET)\n"
	@printf "  $(MAGENTA)Development cycle:$(RESET)     $(GREEN)make re-tests && make run-all$(RESET)\n"
	@printf "  $(MAGENTA)Library changes:$(RESET)       $(GREEN)make lib-re && make re-tests$(RESET)\n"
	@printf "  $(MAGENTA)Clean start:$(RESET)           $(GREEN)make fclean && make && make run-all$(RESET)\n"
	@printf "  $(MAGENTA)Quick test run:$(RESET)        $(GREEN)make run-all$(RESET)\n"
	@printf "  $(MAGENTA)Single component:$(RESET)      $(GREEN)make run-pool$(RESET) (example)\n\n"
	@printf "$(BOLD)$(YELLOW)ℹ️  EXAMPLES:$(RESET)\n"
	@printf "  $(CYAN)# Test only DataBuffer after code changes$(RESET)\n"
	@printf "  $(GREEN)make test_data_buffer && make run-data-buffer$(RESET)\n\n"
	@printf "  $(CYAN)# Full development cycle$(RESET)\n"
	@printf "  $(GREEN)make fclean && make && make run-all$(RESET)\n\n"
	@printf "  $(CYAN)# Test specific component$(RESET)\n"
	@printf "  $(GREEN)make run-pool$(RESET)\n\n"

# Compile the library first
$(LIB):
	@$(MAKE) -C $(LIB_PATH)

# Individual test programs
test_data_buffer: $(LIB) test_data_buffer.cpp
	@$(CC) $(CFLAGS) $(INC) -o $@ test_data_buffer.cpp $(LIBS)
	@printf "$(GREEN)✓ $@ compiled$(RESET)\n"

test_pool: $(LIB) test_pool.cpp
	@$(CC) $(CFLAGS) $(INC) -o $@ test_pool.cpp $(LIBS)
	@printf "$(GREEN)✓ $@ compiled$(RESET)\n"

test_memento: $(LIB) test_memento.cpp
	@$(CC) $(CFLAGS) $(INC) -o $@ test_memento.cpp $(LIBS)
	@printf "$(GREEN)✓ $@ compiled$(RESET)\n"

test_observer: $(LIB) test_observer.cpp
	@$(CC) $(CFLAGS) $(INC) -o $@ test_observer.cpp $(LIBS)
	@printf "$(GREEN)✓ $@ compiled$(RESET)\n"

test_singleton: $(LIB) test_singleton.cpp
	@$(CC) $(CFLAGS) $(INC) -o $@ test_singleton.cpp $(LIBS)
	@printf "$(GREEN)✓ $@ compiled$(RESET)\n"

test_state_machine: $(LIB) test_state_machine.cpp
	@$(CC) $(CFLAGS) $(INC) -o $@ test_state_machine.cpp $(LIBS)
	@printf "$(GREEN)✓ $@ compiled$(RESET)\n"

# Library management targets
lib: $(LIB)
	@printf "$(GREEN)Library $(LIB_NAME) ready!$(RESET)\n"

lib-clean:
	@$(MAKE) -C $(LIB_PATH) clean
	@printf "$(YELLOW)Library object files cleaned$(RESET)\n"

lib-fclean:
	@$(MAKE) -C $(LIB_PATH) fclean
	@printf "$(YELLOW)Library $(LIB_NAME) removed$(RESET)\n"

lib-re: lib-fclean lib
	@printf "$(GREEN)Library rebuilt successfully!$(RESET)\n"

# Run individual tests
run-data-buffer: test_data_buffer
	@printf "$(CYAN)=== Running DataBuffer Tests ===$(RESET)\n"
	@./test_data_buffer

run-pool: test_pool
	@printf "$(CYAN)=== Running Pool Tests ===$(RESET)\n"
	@./test_pool

run-memento: test_memento
	@printf "$(CYAN)=== Running Memento Tests ===$(RESET)\n"
	@./test_memento

run-observer: test_observer
	@printf "$(CYAN)=== Running Observer Tests ===$(RESET)\n"
	@./test_observer

run-singleton: test_singleton
	@printf "$(CYAN)=== Running Singleton Tests ===$(RESET)\n"
	@./test_singleton

run-state-machine: test_state_machine
	@printf "$(CYAN)=== Running StateMachine Tests ===$(RESET)\n"
	@./test_state_machine

# Run all tests
run-all: $(TESTS)
	@printf "$(BLUE)Running all libftpp tests...\n$(RESET)"
	@printf "$(CYAN)=== DataBuffer Tests ===$(RESET)\n"
	@./test_data_buffer
	@printf "\n$(CYAN)=== Pool Tests ===$(RESET)\n"
	@./test_pool
	@printf "\n$(CYAN)=== Memento Tests ===$(RESET)\n"
	@./test_memento
	@printf "\n$(CYAN)=== Observer Tests ===$(RESET)\n"
	@./test_observer
	@printf "\n$(CYAN)=== Singleton Tests ===$(RESET)\n"
	@./test_singleton
	@printf "\n$(CYAN)=== StateMachine Tests ===$(RESET)\n"
	@./test_state_machine
	@printf "\n$(YELLOW)All tests completed!$(RESET)\n"

# Clean only test programs
clean:
	@$(RM) $(TESTS)
	@printf "$(YELLOW)Test programs removed$(RESET)\n"

# Clean tests and library
fclean: clean lib-fclean
	@printf "$(YELLOW)Tests and library cleaned$(RESET)\n"

# Rebuild everything (tests and library)
re: fclean all

# Rebuild only tests (keep library)
re-tests: clean $(TESTS)
	@printf "$(GREEN)Tests rebuilt (library unchanged)$(RESET)\n"

# Full rebuild (library + tests)
fullre: lib-re $(TESTS)
	@printf "$(GREEN)Full rebuild completed!$(RESET)\n"

.PHONY: all clean fclean re re-tests fullre lib lib-clean lib-fclean lib-re run-data-buffer run-pool run-memento run-observer run-singleton run-state-machine run-all help