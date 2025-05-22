/**
 * @file test_singleton.cpp
 * @brief Extended tests for Singleton
 */

#include "../libftpp.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

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

// Game configuration singleton
class GameConfig : public Singleton<GameConfig> {
private:
    friend class Singleton<GameConfig>;
    
    int m_volume;
    std::string m_difficulty;
    bool m_fullscreen;
    std::string m_language;
    int m_maxPlayers;
    
    GameConfig(int volume = 50, const std::string& difficulty = "Normal")
        : m_volume(volume), m_difficulty(difficulty), m_fullscreen(false), 
          m_language("English"), m_maxPlayers(4) {
        std::cout << "GameConfig singleton created with volume=" << volume 
                  << ", difficulty=" << difficulty << std::endl;
    }
    
public:
    void setVolume(int volume) { 
        m_volume = volume; 
        std::cout << "Volume set to: " << volume << std::endl;
    }
    
    void setDifficulty(const std::string& difficulty) { 
        m_difficulty = difficulty;
        std::cout << "Difficulty set to: " << difficulty << std::endl;
    }
    
    void setFullscreen(bool fullscreen) { 
        m_fullscreen = fullscreen;
        std::cout << "Fullscreen " << (fullscreen ? "enabled" : "disabled") << std::endl;
    }
    
    void setLanguage(const std::string& language) { 
        m_language = language;
        std::cout << "Language set to: " << language << std::endl;
    }
    
    void setMaxPlayers(int players) { 
        m_maxPlayers = players;
        std::cout << "Max players set to: " << players << std::endl;
    }
    
    // Getters
    int getVolume() const { return m_volume; }
    const std::string& getDifficulty() const { return m_difficulty; }
    bool isFullscreen() const { return m_fullscreen; }
    const std::string& getLanguage() const { return m_language; }
    int getMaxPlayers() const { return m_maxPlayers; }
    
    void showSettings() const {
        std::cout << "  Current settings:" << std::endl;
        std::cout << "    Volume: " << m_volume << std::endl;
        std::cout << "    Difficulty: " << m_difficulty << std::endl;
        std::cout << "    Fullscreen: " << (m_fullscreen ? "Yes" : "No") << std::endl;
        std::cout << "    Language: " << m_language << std::endl;
        std::cout << "    Max Players: " << m_maxPlayers << std::endl;
    }
};

// Database connection singleton
class DatabaseManager : public Singleton<DatabaseManager> {
private:
    friend class Singleton<DatabaseManager>;
    
    std::string m_connectionString;
    bool m_connected;
    int m_queryCount;
    std::vector<std::string> m_queryLog;
    
    DatabaseManager(const std::string& connectionString)
        : m_connectionString(connectionString), m_connected(false), m_queryCount(0) {
        std::cout << "DatabaseManager created with connection: " << connectionString << std::endl;
    }
    
public:
    void connect() {
        if (!m_connected) {
            m_connected = true;
            std::cout << "Connected to database: " << m_connectionString << std::endl;
        } else {
            std::cout << "Already connected to database" << std::endl;
        }
    }
    
    void disconnect() {
        if (m_connected) {
            m_connected = false;
            std::cout << "Disconnected from database" << std::endl;
        }
    }
    
    void executeQuery(const std::string& query) {
        if (!m_connected) {
            std::cout << "Error: Not connected to database!" << std::endl;
            return;
        }
        
        m_queryCount++;
        m_queryLog.push_back(query);
        std::cout << "Executing query " << m_queryCount << ": " << query << std::endl;
    }
    
    bool isConnected() const { return m_connected; }
    int getQueryCount() const { return m_queryCount; }
    
    void showStats() const {
        std::cout << "  Database stats:" << std::endl;
        std::cout << "    Connection: " << m_connectionString << std::endl;
        std::cout << "    Status: " << (m_connected ? "Connected" : "Disconnected") << std::endl;
        std::cout << "    Queries executed: " << m_queryCount << std::endl;
        std::cout << "    Recent queries:" << std::endl;
        
        int start = std::max(0, (int)m_queryLog.size() - 3);
        for (int i = start; i < (int)m_queryLog.size(); i++) {
            std::cout << "      " << (i + 1) << ". " << m_queryLog[i] << std::endl;
        }
    }
};

void testBasicSingletonBehavior() {
    std::cout << "\n--- Test 1: Basic Singleton Behavior ---\n";
    
    // Try to access before creation
    bool exception_caught = false;
    try {
        GameConfig::instance();
    } catch (const std::exception& e) {
        std::cout << "Correctly caught exception before instantiation: " << e.what() << std::endl;
        exception_caught = true;
    }
    
    // Create singleton
    std::cout << "\nCreating singleton..." << std::endl;
    GameConfig::instantiate();
    
    // Access singleton
    GameConfig* config = GameConfig::instance();
    std::cout << "Successfully accessed singleton instance" << std::endl;
    config->showSettings();
    
    // Try to create another instance
    bool second_exception_caught = false;
    try {
        GameConfig::instantiate(75, "Hard");
    } catch (const std::exception& e) {
        std::cout << "Correctly prevented second instantiation: " << e.what() << std::endl;
        second_exception_caught = true;
    }
    
    bool passed = exception_caught && second_exception_caught && (config != nullptr);
    printResult("Basic Singleton Behavior", passed);
}

void testGlobalAccess() {
    std::cout << "\n--- Test 2: Global Access ---\n";
    
    // Function to simulate different modules accessing the singleton
    auto mainMenu = []() {
        std::cout << "\n[Main Menu Module]" << std::endl;
        GameConfig::instance()->setVolume(80);
        GameConfig::instance()->showSettings();
    };
    
    auto gameplayModule = []() {
        std::cout << "\n[Gameplay Module]" << std::endl;
        GameConfig::instance()->setDifficulty("Expert");
        GameConfig::instance()->setMaxPlayers(8);
        GameConfig::instance()->showSettings();
    };
    
    auto settingsMenu = []() {
        std::cout << "\n[Settings Menu Module]" << std::endl;
        GameConfig::instance()->setFullscreen(true);
        GameConfig::instance()->setLanguage("Spanish");
        GameConfig::instance()->showSettings();
    };
    
    std::cout << "Simulating access from different modules..." << std::endl;
    
    mainMenu();
    gameplayModule();
    settingsMenu();
    
    // Verify final state
    GameConfig* config = GameConfig::instance();
    bool passed = (config->getVolume() == 80 && 
                   config->getDifficulty() == "Expert" && 
                   config->isFullscreen() == true && 
                   config->getLanguage() == "Spanish" && 
                   config->getMaxPlayers() == 8);
    
    printResult("Global Access", passed);
}

void testMultipleSingletons() {
    std::cout << "\n--- Test 3: Multiple Singleton Types ---\n";
    
    // Create database singleton
    std::cout << "Creating DatabaseManager singleton..." << std::endl;
    DatabaseManager::instantiate("postgresql://localhost:5432/gamedb");
    
    // Use both singletons
    std::cout << "\nUsing GameConfig singleton:" << std::endl;
    GameConfig::instance()->setVolume(60);
    
    std::cout << "\nUsing DatabaseManager singleton:" << std::endl;
    DatabaseManager::instance()->connect();
    DatabaseManager::instance()->executeQuery("SELECT * FROM players");
    DatabaseManager::instance()->executeQuery("UPDATE scores SET high_score = 1000");
    
    std::cout << "\nBoth singletons working independently:" << std::endl;
    GameConfig::instance()->showSettings();
    DatabaseManager::instance()->showStats();
    
    bool passed = (GameConfig::instance()->getVolume() == 60 && 
                   DatabaseManager::instance()->isConnected() && 
                   DatabaseManager::instance()->getQueryCount() == 2);
    
    printResult("Multiple Singleton Types", passed);
}

void testSingletonPersistence() {
    std::cout << "\n--- Test 4: Singleton State Persistence ---\n";
    
    // Function that modifies singleton state
    auto modifyConfig = [](int volume, const std::string& difficulty) {
        std::cout << "Modifying config - Volume: " << volume << ", Difficulty: " << difficulty << std::endl;
        GameConfig::instance()->setVolume(volume);
        GameConfig::instance()->setDifficulty(difficulty);
    };
    
    // Function that reads singleton state
    auto readConfig = []() -> std::pair<int, std::string> {
        int volume = GameConfig::instance()->getVolume();
        std::string difficulty = GameConfig::instance()->getDifficulty();
        std::cout << "Reading config - Volume: " << volume << ", Difficulty: " << difficulty << std::endl;
        return {volume, difficulty};
    };
    
    // Modify state
    modifyConfig(90, "Nightmare");
    
    // Read state from different context
    auto [volume, difficulty] = readConfig();
    
    // Modify again
    modifyConfig(25, "Easy");
    
    // Read again
    auto [volume2, difficulty2] = readConfig();
    
    bool passed = (volume == 90 && difficulty == "Nightmare" && 
                   volume2 == 25 && difficulty2 == "Easy");
    
    printResult("Singleton State Persistence", passed);
}

void testRealWorldScenario() {
    std::cout << "\n--- Test 5: Real World Scenario ---\n";
    
    // Simulate a game session with multiple systems using singletons
    
    // Game startup
    std::cout << "\n=== Game Startup ===" << std::endl;
    GameConfig::instance()->setVolume(70);
    GameConfig::instance()->setDifficulty("Normal");
    DatabaseManager::instance()->connect();
    
    // Player customization
    std::cout << "\n=== Player Customization ===" << std::endl;
    GameConfig::instance()->setLanguage("French");
    GameConfig::instance()->setMaxPlayers(2);
    DatabaseManager::instance()->executeQuery("SELECT * FROM player_profiles WHERE user_id = 123");
    
    // Gameplay
    std::cout << "\n=== Gameplay ===" << std::endl;
    DatabaseManager::instance()->executeQuery("INSERT INTO game_sessions (player_id, start_time) VALUES (123, NOW())");
    DatabaseManager::instance()->executeQuery("UPDATE player_stats SET games_played = games_played + 1");
    
    // Settings change during gameplay
    std::cout << "\n=== Settings Change ===" << std::endl;
    GameConfig::instance()->setVolume(50);  // Player lowered volume
    GameConfig::instance()->setFullscreen(true);  // Player went fullscreen
    
    // Game end
    std::cout << "\n=== Game End ===" << std::endl;
    DatabaseManager::instance()->executeQuery("UPDATE game_sessions SET end_time = NOW() WHERE player_id = 123");
    DatabaseManager::instance()->executeQuery("INSERT INTO scores (player_id, score) VALUES (123, 1500)");
    
    // Show final state
    std::cout << "\n=== Final State ===" << std::endl;
    GameConfig::instance()->showSettings();
    DatabaseManager::instance()->showStats();
    
    // Cleanup
    DatabaseManager::instance()->disconnect();
    
    bool passed = (GameConfig::instance()->getVolume() == 50 && 
                   GameConfig::instance()->getLanguage() == "French" && 
                   GameConfig::instance()->isFullscreen() && 
                   DatabaseManager::instance()->getQueryCount() == 6 && 
                   !DatabaseManager::instance()->isConnected());
    
    printResult("Real World Scenario", passed);
}

void testSingletonWithParameters() {
    std::cout << "\n--- Test 6: Singleton with Parameters ---\n";
    
    // Test singleton creation with different parameters
    class Logger : public Singleton<Logger> {
    private:
        friend class Singleton<Logger>;
        
        std::string m_level;
        std::string m_output;
        int m_maxSize;
        
        Logger(const std::string& level, const std::string& output, int maxSize)
            : m_level(level), m_output(output), m_maxSize(maxSize) {
            std::cout << "Logger created - Level: " << level 
                      << ", Output: " << output 
                      << ", MaxSize: " << maxSize << std::endl;
        }
        
    public:
        const std::string& getLevel() const { return m_level; }
        const std::string& getOutput() const { return m_output; }
        int getMaxSize() const { return m_maxSize; }
        
        void log(const std::string& message) {
            std::cout << "[" << m_level << "] " << message << " -> " << m_output << std::endl;
        }
        
        void showConfig() const {
            std::cout << "  Logger config - Level: " << m_level 
                      << ", Output: " << m_output 
                      << ", MaxSize: " << m_maxSize << " MB" << std::endl;
        }
    };
    
    // Create logger with specific parameters
    std::cout << "Creating Logger singleton with parameters..." << std::endl;
    Logger::instantiate("DEBUG", "application.log", 100);
    
    // Use the logger
    Logger::instance()->log("Application started");
    Logger::instance()->log("Database connected");
    Logger::instance()->log("User logged in");
    Logger::instance()->showConfig();
    
    bool passed = (Logger::instance()->getLevel() == "DEBUG" && 
                   Logger::instance()->getOutput() == "application.log" && 
                   Logger::instance()->getMaxSize() == 100);
    
    printResult("Singleton with Parameters", passed);
}

int main() {
    std::cout << CYAN << "Singleton Extended Tests" << RESET << std::endl;
    std::cout << "========================" << std::endl;
    
    testBasicSingletonBehavior();
    testGlobalAccess();
    testMultipleSingletons();
    testSingletonPersistence();
    testRealWorldScenario();
    testSingletonWithParameters();
    
    std::cout << "\n" << YELLOW << "Singleton tests completed!" << RESET << std::endl;
    
    return 0;
}