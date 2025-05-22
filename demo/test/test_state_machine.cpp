/**
 * @file test_state_machine.cpp
 * @brief Extended tests for StateMachine
 */

#include "../libftpp.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

const std::string GREEN = "\033[32m";
const std::string RED = "\033[31m";
const std::string YELLOW = "\033[33m";
const std::string CYAN = "\033[36m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string RESET = "\033[0m";

void printResult(const std::string& testName, bool passed) {
    std::cout << testName << ": " 
              << (passed ? GREEN + "✓ PASS" : RED + "✗ FAIL") 
              << RESET << std::endl;
}

// Player states for game character
enum class PlayerState {
    IDLE,
    WALKING,
    RUNNING,
    JUMPING,
    ATTACKING,
    DEAD
};

std::string stateToString(PlayerState state) {
    switch (state) {
        case PlayerState::IDLE: return "IDLE";
        case PlayerState::WALKING: return "WALKING";
        case PlayerState::RUNNING: return "RUNNING";
        case PlayerState::JUMPING: return "JUMPING";
        case PlayerState::ATTACKING: return "ATTACKING";
        case PlayerState::DEAD: return "DEAD";
        default: return "UNKNOWN";
    }
}

std::string getStateColor(PlayerState state) {
    switch (state) {
        case PlayerState::IDLE: return CYAN;
        case PlayerState::WALKING: return BLUE;
        case PlayerState::RUNNING: return GREEN;
        case PlayerState::JUMPING: return YELLOW;
        case PlayerState::ATTACKING: return RED;
        case PlayerState::DEAD: return MAGENTA;
        default: return RESET;
    }
}

// Connection states for network
enum class ConnectionState {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    RECONNECTING,
    ERROR
};

std::string connectionStateToString(ConnectionState state) {
    switch (state) {
        case ConnectionState::DISCONNECTED: return "DISCONNECTED";
        case ConnectionState::CONNECTING: return "CONNECTING";
        case ConnectionState::CONNECTED: return "CONNECTED";
        case ConnectionState::RECONNECTING: return "RECONNECTING";
        case ConnectionState::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void testBasicStateMachine() {
    std::cout << "\n--- Test 1: Basic State Machine ---\n";
    
    StateMachine<PlayerState> player;
    std::string currentState = "unknown";
    
    // Add states
    player.addState(PlayerState::IDLE);
    player.addState(PlayerState::WALKING);
    player.addState(PlayerState::RUNNING);
    player.addState(PlayerState::ATTACKING);
    player.addState(PlayerState::DEAD);
    
    // Complex state actions with game logic
    player.addAction(PlayerState::IDLE, [&]() {
        stamina = std::min(100, stamina + 2);  // Recover stamina while idle
        actionLog.push_back("IDLE: Resting and recovering stamina");
    });
    
    player.addAction(PlayerState::WALKING, [&]() {
        stamina = std::max(0, stamina - 1);  // Walking uses little stamina
        actionLog.push_back("WALKING: Moving slowly, conserving energy");
    });
    
    player.addAction(PlayerState::RUNNING, [&]() {
        stamina = std::max(0, stamina - 3);  // Running uses more stamina
        actionLog.push_back("RUNNING: Moving fast, using stamina");
        if (stamina == 0) {
            actionLog.push_back("RUNNING: Out of stamina, can't run anymore");
        }
    });
    
    player.addAction(PlayerState::ATTACKING, [&]() {
        if (hasWeapon) {
            stamina = std::max(0, stamina - 5);  // Attacking uses stamina
            actionLog.push_back("ATTACKING: Swinging weapon");
        } else {
            actionLog.push_back("ATTACKING: Punching (no weapon)");
        }
    });
    
    player.addAction(PlayerState::DEAD, [&]() {
        actionLog.push_back("DEAD: Game over");
    });
    
    // Complex transitions with conditions
    player.addTransition(PlayerState::IDLE, PlayerState::WALKING, [&]() {
        actionLog.push_back("TRANSITION: Starting to walk");
    });
    
    player.addTransition(PlayerState::WALKING, PlayerState::RUNNING, [&]() {
        if (stamina > 10) {
            actionLog.push_back("TRANSITION: Has enough stamina to run");
        } else {
            actionLog.push_back("TRANSITION: Low stamina but trying to run");
        }
    });
    
    player.addTransition(PlayerState::RUNNING, PlayerState::WALKING, [&]() {
        actionLog.push_back("TRANSITION: Slowing down from running");
    });
    
    player.addTransition(PlayerState::IDLE, PlayerState::ATTACKING, [&]() {
        if (hasWeapon) {
            actionLog.push_back("TRANSITION: Drawing weapon to attack");
        } else {
            actionLog.push_back("TRANSITION: Preparing to fight barehanded");
        }
    });
    
    player.addTransition(PlayerState::ATTACKING, PlayerState::IDLE, [&]() {
        actionLog.push_back("TRANSITION: Finishing attack, returning to ready stance");
    });
    
    // Death transition from any state
    player.addTransition(PlayerState::WALKING, PlayerState::DEAD, [&]() {
        actionLog.push_back("TRANSITION: Died while walking");
    });
    
    std::cout << "Simulating complex gameplay with state-dependent logic:" << std::endl;
    std::cout << "Initial stats - Health: " << health << ", Stamina: " << stamina 
              << ", Has weapon: " << (hasWeapon ? "Yes" : "No") << std::endl;
    
    // Simulate gameplay sequence
    player.update();  // Start idle
    std::cout << "Stamina after idle: " << stamina << std::endl;
    
    player.transitionTo(PlayerState::WALKING);
    player.update();
    std::cout << "Stamina after walking: " << stamina << std::endl;
    
    player.transitionTo(PlayerState::RUNNING);
    player.update();
    std::cout << "Stamina after running: " << stamina << std::endl;
    
    player.transitionTo(PlayerState::WALKING);
    player.update();
    std::cout << "Stamina after slowing down: " << stamina << std::endl;
    
    player.transitionTo(PlayerState::IDLE);
    player.update();
    std::cout << "Stamina after resting: " << stamina << std::endl;
    
    player.transitionTo(PlayerState::ATTACKING);
    player.update();
    std::cout << "Stamina after attack: " << stamina << std::endl;
    
    player.transitionTo(PlayerState::IDLE);
    player.update();
    
    std::cout << "\nAction log:" << std::endl;
    for (const auto& action : actionLog) {
        std::cout << "  - " << action << std::endl;
    }
    
    bool passed = (stamina < 100 && stamina > 80 && actionLog.size() >= 10);
    printResult("State Machine with Complex Logic", passed);
}

void testStateMachineErrorHandling() {
    std::cout << "\n--- Test 6: State Machine Error Handling ---\n";
    
    StateMachine<PlayerState> player;
    
    // Setup basic states
    player.addState(PlayerState::IDLE);
    player.addState(PlayerState::WALKING);
    
    player.addAction(PlayerState::IDLE, []() {
        std::cout << "  Player is idle" << std::endl;
    });
    
    player.addAction(PlayerState::WALKING, []() {
        std::cout << "  Player is walking" << std::endl;
    });
    
    // Test 1: Try to transition to non-existent state
    std::cout << "Test 1: Transition to non-existent state" << std::endl;
    bool error1_caught = false;
    try {
        player.transitionTo(PlayerState::RUNNING);  // RUNNING was not added
    } catch (const std::exception& e) {
        std::cout << "  Correctly caught error: " << e.what() << std::endl;
        error1_caught = true;
    }
    
    // Test 2: Try to add transition with non-existent states
    std::cout << "\nTest 2: Add transition with non-existent state" << std::endl;
    bool error2_caught = false;
    try {
        player.addTransition(PlayerState::IDLE, PlayerState::ATTACKING, []() {
            // ATTACKING state doesn't exist
        });
    } catch (const std::exception& e) {
        std::cout << "  Correctly caught error: " << e.what() << std::endl;
        error2_caught = true;
    }
    
    // Test 3: Try to add action for non-existent state
    std::cout << "\nTest 3: Add action for non-existent state" << std::endl;
    bool error3_caught = false;
    try {
        player.addAction(PlayerState::JUMPING, []() {
            // JUMPING state doesn't exist
        });
    } catch (const std::exception& e) {
        std::cout << "  Correctly caught error: " << e.what() << std::endl;
        error3_caught = true;
    }
    
    // Test 4: Try to update state without actions
    std::cout << "\nTest 4: Update state without defined action" << std::endl;
    player.addState(PlayerState::RUNNING);  // Add state but no action
    bool error4_caught = false;
    try {
        player.transitionTo(PlayerState::RUNNING);  // This should work
        player.update();  // This should fail (no action defined)
    } catch (const std::exception& e) {
        std::cout << "  Correctly caught error: " << e.what() << std::endl;
        error4_caught = true;
    }
    
    // Verify the state machine still works correctly after errors
    std::cout << "\nTest 5: Verify state machine still works after errors" << std::endl;
    player.addTransition(PlayerState::IDLE, PlayerState::WALKING, []() {
        std::cout << "  Valid transition executed" << std::endl;
    });
    
    bool normal_operation = false;
    try {
        player.transitionTo(PlayerState::IDLE);  // Reset to known state
        player.update();  // Should work
        player.transitionTo(PlayerState::WALKING);  // Should work
        player.update();  // Should work
        normal_operation = true;
        std::cout << "  State machine operates normally after errors" << std::endl;
    } catch (...) {
        std::cout << "  ERROR: State machine broken after error handling" << std::endl;
    }
    
    bool passed = error1_caught && error2_caught && error3_caught && 
                  error4_caught && normal_operation;
    
    printResult("State Machine Error Handling", passed);
}

int main() {
    std::cout << CYAN << "StateMachine Extended Tests" << RESET << std::endl;
    std::cout << "===========================" << std::endl;
    
    testBasicStateMachine();
    testStateTransitions();
    testInvalidTransitions();
    testNetworkConnectionStateMachine();
    testStateMachineWithComplexLogic();
    testStateMachineErrorHandling();
    
    std::cout << "\n" << YELLOW << "StateMachine tests completed!" << RESET << std::endl;
    
    return 0;
}(PlayerState::WALKING);
    player.addState(PlayerState::RUNNING);
    player.addState(PlayerState::JUMPING);
    
    std::cout << "Added states: IDLE, WALKING, RUNNING, JUMPING" << std::endl;
    
    // Add state actions
    player.addAction(PlayerState::IDLE, [&currentState]() {
        currentState = "IDLE";
        std::cout << "  🧍 Player is standing idle" << std::endl;
    });
    
    player.addAction(PlayerState::WALKING, [&currentState]() {
        currentState = "WALKING";
        std::cout << "  🚶 Player is walking" << std::endl;
    });
    
    player.addAction(PlayerState::RUNNING, [&currentState]() {
        currentState = "RUNNING";
        std::cout << "  🏃 Player is running" << std::endl;
    });
    
    player.addAction(PlayerState::JUMPING, [&currentState]() {
        currentState = "JUMPING";
        std::cout << "  🦘 Player is jumping" << std::endl;
    });
    
    // Test initial state
    std::cout << "\nInitial state:" << std::endl;
    player.update();
    
    bool passed = (currentState == "IDLE");  // First added state becomes initial
    printResult("Basic State Machine", passed);
}

void testStateTransitions() {
    std::cout << "\n--- Test 2: State Transitions ---\n";
    
    StateMachine<PlayerState> player;
    std::vector<std::string> transitionLog;
    std::string currentState = "unknown";
    
    // Setup states
    player.addState(PlayerState::IDLE);
    player.addState(PlayerState::WALKING);
    player.addState(PlayerState::RUNNING);
    player.addState(PlayerState::JUMPING);
    
    // Add state actions
    player.addAction(PlayerState::IDLE, [&currentState]() {
        currentState = "IDLE";
    });
    
    player.addAction(PlayerState::WALKING, [&currentState]() {
        currentState = "WALKING";
    });
    
    player.addAction(PlayerState::RUNNING, [&currentState]() {
        currentState = "RUNNING";
    });
    
    player.addAction(PlayerState::JUMPING, [&currentState]() {
        currentState = "JUMPING";
    });
    
    // Add transitions with actions
    player.addTransition(PlayerState::IDLE, PlayerState::WALKING, [&transitionLog]() {
        transitionLog.push_back("IDLE->WALKING");
        std::cout << "  Transition: Starting to walk" << std::endl;
    });
    
    player.addTransition(PlayerState::WALKING, PlayerState::RUNNING, [&transitionLog]() {
        transitionLog.push_back("WALKING->RUNNING");
        std::cout << "  Transition: Speeding up to run" << std::endl;
    });
    
    player.addTransition(PlayerState::RUNNING, PlayerState::JUMPING, [&transitionLog]() {
        transitionLog.push_back("RUNNING->JUMPING");
        std::cout << "  Transition: Jumping while running" << std::endl;
    });
    
    player.addTransition(PlayerState::JUMPING, PlayerState::IDLE, [&transitionLog]() {
        transitionLog.push_back("JUMPING->IDLE");
        std::cout << "  Transition: Landing and stopping" << std::endl;
    });
    
    std::cout << "Performing state transitions:" << std::endl;
    
    // Initial state
    player.update();
    std::cout << "Current: " << currentState << std::endl;
    
    // Sequence of transitions
    player.transitionTo(PlayerState::WALKING);
    player.update();
    std::cout << "Current: " << currentState << std::endl;
    
    player.transitionTo(PlayerState::RUNNING);
    player.update();
    std::cout << "Current: " << currentState << std::endl;
    
    player.transitionTo(PlayerState::JUMPING);
    player.update();
    std::cout << "Current: " << currentState << std::endl;
    
    player.transitionTo(PlayerState::IDLE);
    player.update();
    std::cout << "Current: " << currentState << std::endl;
    
    std::cout << "\nTransition log:" << std::endl;
    for (const auto& transition : transitionLog) {
        std::cout << "  - " << transition << std::endl;
    }
    
    bool passed = (transitionLog.size() == 4 && 
                   currentState == "IDLE" && 
                   transitionLog[0] == "IDLE->WALKING" && 
                   transitionLog[3] == "JUMPING->IDLE");
    
    printResult("State Transitions", passed);
}

void testInvalidTransitions() {
    std::cout << "\n--- Test 3: Invalid Transitions ---\n";
    
    StateMachine<PlayerState> player;
    std::string currentState = "unknown";
    
    // Setup states
    player.addState(PlayerState::IDLE);
    player.addState(PlayerState::WALKING);
    player.addState(PlayerState::DEAD);
    
    player.addAction(PlayerState::IDLE, [&currentState]() {
        currentState = "IDLE";
    });
    
    player.addAction(PlayerState::WALKING, [&currentState]() {
        currentState = "WALKING";
    });
    
    player.addAction(PlayerState::DEAD, [&currentState]() {
        currentState = "DEAD";
    });
    
    // Only add specific transitions (not all combinations)
    player.addTransition(PlayerState::IDLE, PlayerState::WALKING, []() {
        std::cout << "  Valid transition: IDLE -> WALKING" << std::endl;
    });
    
    player.addTransition(PlayerState::WALKING, PlayerState::DEAD, []() {
        std::cout << "  Valid transition: WALKING -> DEAD" << std::endl;
    });
    
    // Test valid transition
    std::cout << "Testing valid transition (IDLE -> WALKING):" << std::endl;
    player.update();  // Start in IDLE
    player.transitionTo(PlayerState::WALKING);
    player.update();
    bool validWorked = (currentState == "WALKING");
    
    // Test invalid transition
    std::cout << "\nTesting invalid transition (WALKING -> IDLE):" << std::endl;
    bool exceptionCaught = false;
    try {
        player.transitionTo(PlayerState::IDLE);  // No transition defined
    } catch (const std::exception& e) {
        std::cout << "  Correctly caught exception: " << e.what() << std::endl;
        exceptionCaught = true;
    }
    
    // State should remain unchanged after failed transition
    player.update();
    bool stateUnchanged = (currentState == "WALKING");
    
    bool passed = validWorked && exceptionCaught && stateUnchanged;
    printResult("Invalid Transitions", passed);
}

void testNetworkConnectionStateMachine() {
    std::cout << "\n--- Test 4: Network Connection State Machine ---\n";
    
    StateMachine<ConnectionState> network;
    std::string connectionStatus = "unknown";
    int connectionAttempts = 0;
    bool errorOccurred = false;
    
    // Add all connection states
    network.addState(ConnectionState::DISCONNECTED);
    network.addState(ConnectionState::CONNECTING);
    network.addState(ConnectionState::CONNECTED);
    network.addState(ConnectionState::RECONNECTING);
    network.addState(ConnectionState::ERROR);
    
    // State actions
    network.addAction(ConnectionState::DISCONNECTED, [&connectionStatus]() {
        connectionStatus = "DISCONNECTED";
        std::cout << "  📵 Network disconnected" << std::endl;
    });
    
    network.addAction(ConnectionState::CONNECTING, [&connectionStatus, &connectionAttempts]() {
        connectionStatus = "CONNECTING";
        connectionAttempts++;
        std::cout << "  🔄 Connecting... (attempt " << connectionAttempts << ")" << std::endl;
    });
    
    network.addAction(ConnectionState::CONNECTED, [&connectionStatus]() {
        connectionStatus = "CONNECTED";
        std::cout << "  ✅ Connected to network" << std::endl;
    });
    
    network.addAction(ConnectionState::RECONNECTING, [&connectionStatus]() {
        connectionStatus = "RECONNECTING";
        std::cout << "  🔄 Reconnecting..." << std::endl;
    });
    
    network.addAction(ConnectionState::ERROR, [&connectionStatus, &errorOccurred]() {
        connectionStatus = "ERROR";
        errorOccurred = true;
        std::cout << "  ❌ Connection error" << std::endl;
    });
    
    // Add transitions
    network.addTransition(ConnectionState::DISCONNECTED, ConnectionState::CONNECTING, []() {
        std::cout << "    Starting connection process..." << std::endl;
    });
    
    network.addTransition(ConnectionState::CONNECTING, ConnectionState::CONNECTED, []() {
        std::cout << "    Connection established!" << std::endl;
    });
    
    network.addTransition(ConnectionState::CONNECTING, ConnectionState::ERROR, []() {
        std::cout << "    Connection failed!" << std::endl;
    });
    
    network.addTransition(ConnectionState::CONNECTED, ConnectionState::RECONNECTING, []() {
        std::cout << "    Connection lost, attempting to reconnect..." << std::endl;
    });
    
    network.addTransition(ConnectionState::RECONNECTING, ConnectionState::CONNECTED, []() {
        std::cout << "    Reconnection successful!" << std::endl;
    });
    
    network.addTransition(ConnectionState::RECONNECTING, ConnectionState::ERROR, []() {
        std::cout << "    Reconnection failed!" << std::endl;
    });
    
    network.addTransition(ConnectionState::ERROR, ConnectionState::DISCONNECTED, []() {
        std::cout << "    Resetting to disconnected state" << std::endl;
    });
    
    std::cout << "Simulating network connection lifecycle:" << std::endl;
    
    // Start disconnected
    network.update();
    
    // Connect
    network.transitionTo(ConnectionState::CONNECTING);
    network.update();
    
    // Connection succeeds
    network.transitionTo(ConnectionState::CONNECTED);
    network.update();
    
    // Connection drops, try to reconnect
    network.transitionTo(ConnectionState::RECONNECTING);
    network.update();
    
    // Reconnection succeeds
    network.transitionTo(ConnectionState::CONNECTED);
    network.update();
    
    // Simulate another drop and failure
    network.transitionTo(ConnectionState::RECONNECTING);
    network.update();
    
    // This time reconnection fails
    network.transitionTo(ConnectionState::ERROR);
    network.update();
    
    // Reset
    network.transitionTo(ConnectionState::DISCONNECTED);
    network.update();
    
    bool passed = (connectionStatus == "DISCONNECTED" && 
                   connectionAttempts == 1 && 
                   errorOccurred == true);
    
    printResult("Network Connection State Machine", passed);
}

void testStateMachineWithComplexLogic() {
    std::cout << "\n--- Test 5: State Machine with Complex Logic ---\n";
    
    StateMachine<PlayerState> player;
    
    // Game state variables
    int health = 100;
    int stamina = 100;
    bool hasWeapon = true;
    std::vector<std::string> actionLog;
    
    // Add states
    player.addState(PlayerState::IDLE);
    player.addState