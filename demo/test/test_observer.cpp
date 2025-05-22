/**
 * @file test_observer.cpp
 * @brief Extended tests for Observer
 */

#include "../libftpp.hpp"
#include <iostream>
#include <string>
#include <vector>

const std::string GREEN = "\033[32m";
const std::string RED = "\033[31m";
const std::string YELLOW = "\033[33m";
const std::string CYAN = "\033[36m";
const std::string RESET = "\033[0m";

void printResult(const std::string& testName, bool passed) {
    std::cout << testName << ": " 
              << (passed ? GREEN + "✓ PASS" : RED + "✗ FAIL") 
              << RESET << std::endl;
}

enum GameEvent {
    PLAYER_MOVE,
    PLAYER_JUMP,
    PLAYER_ATTACK,
    ENEMY_SPAWN,
    ITEM_COLLECTED,
    LEVEL_COMPLETE
};

std::string eventToString(GameEvent event) {
    switch (event) {
        case PLAYER_MOVE: return "PLAYER_MOVE";
        case PLAYER_JUMP: return "PLAYER_JUMP";
        case PLAYER_ATTACK: return "PLAYER_ATTACK";
        case ENEMY_SPAWN: return "ENEMY_SPAWN";
        case ITEM_COLLECTED: return "ITEM_COLLECTED";
        case LEVEL_COMPLETE: return "LEVEL_COMPLETE";
        default: return "UNKNOWN";
    }
}

void testBasicSubscriptionAndNotification() {
    std::cout << "\n--- Test 1: Basic Subscription and Notification ---\n";
    
    Observer<GameEvent> gameEvents;
    
    int moveCount = 0;
    int jumpCount = 0;
    
    // Subscribe to events
    gameEvents.subscribe(PLAYER_MOVE, [&moveCount]() {
        moveCount++;
        std::cout << "  Player moved! Total moves: " << moveCount << std::endl;
    });
    
    gameEvents.subscribe(PLAYER_JUMP, [&jumpCount]() {
        jumpCount++;
        std::cout << "  Player jumped! Total jumps: " << jumpCount << std::endl;
    });
    
    std::cout << "Subscribed to PLAYER_MOVE and PLAYER_JUMP events" << std::endl;
    
    // Trigger events
    std::cout << "\nTriggering events:" << std::endl;
    gameEvents.notify(PLAYER_MOVE);
    gameEvents.notify(PLAYER_MOVE);
    gameEvents.notify(PLAYER_JUMP);
    gameEvents.notify(PLAYER_MOVE);
    gameEvents.notify(PLAYER_JUMP);
    
    std::cout << "\nFinal counts - Moves: " << moveCount << ", Jumps: " << jumpCount << std::endl;
    
    bool passed = (moveCount == 3 && jumpCount == 2);
    printResult("Basic Subscription and Notification", passed);
}

void testMultipleSubscribersPerEvent() {
    std::cout << "\n--- Test 2: Multiple Subscribers Per Event ---\n";
    
    Observer<GameEvent> gameEvents;
    
    int soundEffectCount = 0;
    int animationCount = 0;
    int scoreCount = 0;
    
    // Multiple subscribers for the same event
    gameEvents.subscribe(PLAYER_ATTACK, [&soundEffectCount]() {
        soundEffectCount++;
        std::cout << "  Sound: Playing attack sound effect" << std::endl;
    });
    
    gameEvents.subscribe(PLAYER_ATTACK, [&animationCount]() {
        animationCount++;
        std::cout << "  Animation: Playing attack animation" << std::endl;
    });
    
    gameEvents.subscribe(PLAYER_ATTACK, [&scoreCount]() {
        scoreCount++;
        std::cout << "  Score: Attack recorded for combo system" << std::endl;
    });
    
    std::cout << "Added 3 subscribers to PLAYER_ATTACK event" << std::endl;
    
    // Trigger attack event
    std::cout << "\nPlayer attacks:" << std::endl;
    gameEvents.notify(PLAYER_ATTACK);
    
    std::cout << "\nPlayer attacks again:" << std::endl;
    gameEvents.notify(PLAYER_ATTACK);
    
    std::cout << "\nAll systems responded - Sound: " << soundEffectCount 
              << ", Animation: " << animationCount << ", Score: " << scoreCount << std::endl;
    
    bool passed = (soundEffectCount == 2 && animationCount == 2 && scoreCount == 2);
    printResult("Multiple Subscribers Per Event", passed);
}

void testGameSystemIntegration() {
    std::cout << "\n--- Test 3: Game System Integration ---\n";
    
    Observer<GameEvent> gameEvents;
    
    // Game state
    int playerScore = 0;
    int enemiesKilled = 0;
    int itemsCollected = 0;
    bool levelCompleted = false;
    std::vector<std::string> logMessages;
    
    // Score system
    gameEvents.subscribe(ENEMY_SPAWN, [&playerScore, &logMessages]() {
        logMessages.push_back("Enemy spawned - difficulty increased");
    });
    
    gameEvents.subscribe(ITEM_COLLECTED, [&playerScore, &itemsCollected, &logMessages]() {
        itemsCollected++;
        playerScore += 10;
        logMessages.push_back("Item collected - Score: " + std::to_string(playerScore));
    });
    
    // Combat system
    gameEvents.subscribe(PLAYER_ATTACK, [&enemiesKilled, &playerScore, &logMessages]() {
        enemiesKilled++;
        playerScore += 5;
        logMessages.push_back("Enemy defeated - Score: " + std::to_string(playerScore));
    });
    
    // Level system
    gameEvents.subscribe(LEVEL_COMPLETE, [&levelCompleted, &playerScore, &logMessages]() {
        levelCompleted = true;
        playerScore += 100;
        logMessages.push_back("Level completed - Bonus score added");
    });
    
    std::cout << "Game systems initialized and subscribed to events" << std::endl;
    
    // Simulate gameplay
    std::cout << "\nSimulating gameplay:" << std::endl;
    
    gameEvents.notify(ENEMY_SPAWN);
    gameEvents.notify(ITEM_COLLECTED);
    gameEvents.notify(ITEM_COLLECTED);
    gameEvents.notify(PLAYER_ATTACK);
    gameEvents.notify(ENEMY_SPAWN);
    gameEvents.notify(PLAYER_ATTACK);
    gameEvents.notify(ITEM_COLLECTED);
    gameEvents.notify(LEVEL_COMPLETE);
    
    std::cout << "\nGame session results:" << std::endl;
    std::cout << "  Final score: " << playerScore << std::endl;
    std::cout << "  Enemies killed: " << enemiesKilled << std::endl;
    std::cout << "  Items collected: " << itemsCollected << std::endl;
    std::cout << "  Level completed: " << (levelCompleted ? "Yes" : "No") << std::endl;
    
    std::cout << "\nEvent log:" << std::endl;
    for (const auto& msg : logMessages) {
        std::cout << "  - " << msg << std::endl;
    }
    
    bool passed = (playerScore == 140 && enemiesKilled == 2 && 
                   itemsCollected == 3 && levelCompleted);
    printResult("Game System Integration", passed);
}

void testUIEventHandling() {
    std::cout << "\n--- Test 4: UI Event Handling ---\n";
    
    enum UIEvent { BUTTON_CLICK, MENU_OPEN, MENU_CLOSE, SETTING_CHANGED };
    
    Observer<UIEvent> uiEvents;
    
    // UI state
    bool menuOpen = false;
    int buttonClicks = 0;
    std::string currentSetting = "default";
    std::vector<std::string> uiLog;
    
    // Button handling
    uiEvents.subscribe(BUTTON_CLICK, [&buttonClicks, &uiLog]() {
        buttonClicks++;
        uiLog.push_back("Button clicked (total: " + std::to_string(buttonClicks) + ")");
    });
    
    // Menu handling
    uiEvents.subscribe(MENU_OPEN, [&menuOpen, &uiLog]() {
        menuOpen = true;
        uiLog.push_back("Menu opened");
    });
    
    uiEvents.subscribe(MENU_CLOSE, [&menuOpen, &uiLog]() {
        menuOpen = false;
        uiLog.push_back("Menu closed");
    });
    
    // Settings handling
    uiEvents.subscribe(SETTING_CHANGED, [&currentSetting, &uiLog]() {
        currentSetting = "modified";
        uiLog.push_back("Settings changed to: " + currentSetting);
    });
    
    std::cout << "UI event handlers set up" << std::endl;
    
    // Simulate UI interactions
    std::cout << "\nSimulating UI interactions:" << std::endl;
    
    uiEvents.notify(BUTTON_CLICK);
    uiEvents.notify(MENU_OPEN);
    uiEvents.notify(SETTING_CHANGED);
    uiEvents.notify(BUTTON_CLICK);
    uiEvents.notify(MENU_CLOSE);
    uiEvents.notify(BUTTON_CLICK);
    
    std::cout << "\nUI State:" << std::endl;
    std::cout << "  Menu open: " << (menuOpen ? "Yes" : "No") << std::endl;
    std::cout << "  Button clicks: " << buttonClicks << std::endl;
    std::cout << "  Current setting: " << currentSetting << std::endl;
    
    std::cout << "\nUI Event Log:" << std::endl;
    for (const auto& msg : uiLog) {
        std::cout << "  - " << msg << std::endl;
    }
    
    bool passed = (!menuOpen && buttonClicks == 3 && currentSetting == "modified" && uiLog.size() == 6);
    printResult("UI Event Handling", passed);
}

void testEventWithoutSubscribers() {
    std::cout << "\n--- Test 5: Events Without Subscribers ---\n";
    
    Observer<GameEvent> gameEvents;
    
    int moveCount = 0;
    
    // Only subscribe to one event
    gameEvents.subscribe(PLAYER_MOVE, [&moveCount]() {
        moveCount++;
        std::cout << "  Player moved" << std::endl;
    });
    
    std::cout << "Only subscribed to PLAYER_MOVE event" << std::endl;
    
    // Notify subscribed event
    std::cout << "\nNotifying subscribed event (PLAYER_MOVE):" << std::endl;
    gameEvents.notify(PLAYER_MOVE);
    
    // Notify unsubscribed events (should do nothing)
    std::cout << "\nNotifying unsubscribed events (should be silent):" << std::endl;
    gameEvents.notify(PLAYER_JUMP);
    gameEvents.notify(ENEMY_SPAWN);
    gameEvents.notify(LEVEL_COMPLETE);
    
    std::cout << "Notified unsubscribed events - no output should appear above" << std::endl;
    std::cout << "Move count: " << moveCount << std::endl;
    
    bool passed = (moveCount == 1);
    printResult("Events Without Subscribers", passed);
}

void testEventSequenceOrder() {
    std::cout << "\n--- Test 6: Event Sequence Order ---\n";
    
    Observer<GameEvent> gameEvents;
    
    std::vector<std::string> executionOrder;
    
    // Subscribe multiple handlers for the same event
    gameEvents.subscribe(LEVEL_COMPLETE, [&executionOrder]() {
        executionOrder.push_back("Handler1");
        std::cout << "  Handler 1: Save game data" << std::endl;
    });
    
    gameEvents.subscribe(LEVEL_COMPLETE, [&executionOrder]() {
        executionOrder.push_back("Handler2");
        std::cout << "  Handler 2: Update statistics" << std::endl;
    });
    
    gameEvents.subscribe(LEVEL_COMPLETE, [&executionOrder]() {
        executionOrder.push_back("Handler3");
        std::cout << "  Handler 3: Show completion screen" << std::endl;
    });
    
    std::cout << "Added 3 handlers for LEVEL_COMPLETE event" << std::endl;
    
    // Trigger event
    std::cout << "\nTriggering LEVEL_COMPLETE event:" << std::endl;
    gameEvents.notify(LEVEL_COMPLETE);
    
    std::cout << "\nExecution order: ";
    for (size_t i = 0; i < executionOrder.size(); i++) {
        std::cout << executionOrder[i];
        if (i < executionOrder.size() - 1) std::cout << " -> ";
    }
    std::cout << std::endl;
    
    // Verify all handlers executed
    bool all_executed = (executionOrder.size() == 3 && 
                        executionOrder[0] == "Handler1" && 
                        executionOrder[1] == "Handler2" && 
                        executionOrder[2] == "Handler3");
    
    printResult("Event Sequence Order", all_executed);
}

int main() {
    std::cout << CYAN << "Observer Extended Tests" << RESET << std::endl;
    std::cout << "=======================" << std::endl;
    
    testBasicSubscriptionAndNotification();
    testMultipleSubscribersPerEvent();
    testGameSystemIntegration();
    testUIEventHandling();
    testEventWithoutSubscribers();
    testEventSequenceOrder();
    
    std::cout << "\n" << YELLOW << "Observer tests completed!" << RESET << std::endl;
    
    return 0;
}