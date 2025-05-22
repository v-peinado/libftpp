/**
 * @file test_memento.cpp
 * @brief Extended tests for Memento
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

// Test class: Simple calculator that can save/restore state
class Calculator : public Memento {
private:
    double m_result;
    std::string m_lastOperation;
    
protected:
    void saveToSnapshot(Snapshot& snapshot) const override {
        snapshot.set("result", m_result);
        snapshot.set("lastOperation", m_lastOperation);
    }
    
    void loadFromSnapshot(const Snapshot& snapshot) override {
        m_result = snapshot.get<double>("result");
        m_lastOperation = snapshot.get<std::string>("lastOperation");
    }
    
public:
    using Memento::save;
    using Memento::load;
    
    Calculator() : m_result(0.0), m_lastOperation("init") {
        std::cout << "Calculator initialized with result = " << m_result << std::endl;
    }
    
    void add(double value) {
        m_result += value;
        m_lastOperation = "add " + std::to_string(value);
        std::cout << "Added " << value << ", result = " << m_result << std::endl;
    }
    
    void subtract(double value) {
        m_result -= value;
        m_lastOperation = "subtract " + std::to_string(value);
        std::cout << "Subtracted " << value << ", result = " << m_result << std::endl;
    }
    
    void multiply(double value) {
        m_result *= value;
        m_lastOperation = "multiply " + std::to_string(value);
        std::cout << "Multiplied by " << value << ", result = " << m_result << std::endl;
    }
    
    void clear() {
        m_result = 0.0;
        m_lastOperation = "clear";
        std::cout << "Calculator cleared, result = " << m_result << std::endl;
    }
    
    double getResult() const { return m_result; }
    const std::string& getLastOperation() const { return m_lastOperation; }
    
    void showStatus() const {
        std::cout << "  Current result: " << m_result 
                  << ", Last operation: " << m_lastOperation << std::endl;
    }
};

// Test class: Player with multiple attributes
class Player : public Memento {
private:
    std::string m_name;
    int m_level;
    int m_health;
    int m_score;
    bool m_hasKey;
    
protected:
    void saveToSnapshot(Snapshot& snapshot) const override {
        snapshot.set("name", m_name);
        snapshot.set("level", m_level);
        snapshot.set("health", m_health);
        snapshot.set("score", m_score);
        snapshot.set("hasKey", m_hasKey);
    }
    
    void loadFromSnapshot(const Snapshot& snapshot) override {
        m_name = snapshot.get<std::string>("name");
        m_level = snapshot.get<int>("level");
        m_health = snapshot.get<int>("health");
        m_score = snapshot.get<int>("score");
        m_hasKey = snapshot.get<bool>("hasKey");
    }
    
public:
    using Memento::save;
    using Memento::load;
    
    Player(const std::string& name) 
        : m_name(name), m_level(1), m_health(100), m_score(0), m_hasKey(false) {
        std::cout << "Player " << m_name << " created" << std::endl;
    }
    
    void gainExperience(int exp) {
        m_score += exp;
        if (m_score >= m_level * 100) {
            m_level++;
            m_health = 100;  // Restore health on level up
            std::cout << m_name << " leveled up to level " << m_level << "!" << std::endl;
        } else {
            std::cout << m_name << " gained " << exp << " experience" << std::endl;
        }
    }
    
    void takeDamage(int damage) {
        m_health -= damage;
        if (m_health < 0) m_health = 0;
        std::cout << m_name << " took " << damage << " damage, health: " << m_health << std::endl;
    }
    
    void findKey() {
        m_hasKey = true;
        std::cout << m_name << " found a key!" << std::endl;
    }
    
    void showStatus() const {
        std::cout << "  " << m_name << " - Level: " << m_level 
                  << ", Health: " << m_health << ", Score: " << m_score 
                  << ", Has key: " << (m_hasKey ? "Yes" : "No") << std::endl;
    }
    
    // Getters for testing
    const std::string& getName() const { return m_name; }
    int getLevel() const { return m_level; }
    int getHealth() const { return m_health; }
    int getScore() const { return m_score; }
    bool hasKey() const { return m_hasKey; }
};

void testBasicSaveRestore() {
    std::cout << "\n--- Test 1: Basic Save/Restore ---\n";
    
    Calculator calc;
    calc.showStatus();
    
    // Perform some operations
    calc.add(10);
    calc.multiply(2);
    calc.subtract(5);
    calc.showStatus();
    
    // Save state
    std::cout << "\nSaving state..." << std::endl;
    auto checkpoint = calc.save();
    double saved_result = calc.getResult();
    std::string saved_operation = calc.getLastOperation();
    
    // Make more changes
    calc.add(100);
    calc.multiply(3);
    calc.showStatus();
    
    // Restore
    std::cout << "\nRestoring state..." << std::endl;
    calc.load(checkpoint);
    calc.showStatus();
    
    bool passed = (calc.getResult() == saved_result && 
                   calc.getLastOperation() == saved_operation);
    
    printResult("Basic Save/Restore", passed);
}

void testMultipleSnapshots() {
    std::cout << "\n--- Test 2: Multiple Snapshots ---\n";
    
    Player player("Hero");
    player.showStatus();
    
    // Save initial state
    auto initial_state = player.save();
    std::cout << "Initial state saved" << std::endl;
    
    // Progress 1
    player.gainExperience(50);
    player.takeDamage(20);
    player.showStatus();
    
    // Save checkpoint 1
    auto checkpoint1 = player.save();
    std::cout << "Checkpoint 1 saved" << std::endl;
    
    // Progress 2
    player.gainExperience(60);  // Should level up
    player.findKey();
    player.showStatus();
    
    // Save checkpoint 2
    auto checkpoint2 = player.save();
    std::cout << "Checkpoint 2 saved" << std::endl;
    
    // More progress
    player.takeDamage(50);
    player.gainExperience(25);
    player.showStatus();
    
    // Test restoring different checkpoints
    std::cout << "\nRestoring to checkpoint 1:" << std::endl;
    player.load(checkpoint1);
    player.showStatus();
    bool test1 = (player.getLevel() == 1 && player.getHealth() == 80 && !player.hasKey());
    
    std::cout << "\nRestoring to checkpoint 2:" << std::endl;
    player.load(checkpoint2);
    player.showStatus();
    bool test2 = (player.getLevel() == 2 && player.getHealth() == 100 && player.hasKey());
    
    std::cout << "\nRestoring to initial state:" << std::endl;
    player.load(initial_state);
    player.showStatus();
    bool test3 = (player.getLevel() == 1 && player.getHealth() == 100 && 
                  player.getScore() == 0 && !player.hasKey());
    
    bool passed = test1 && test2 && test3;
    printResult("Multiple Snapshots", passed);
}

void testSequentialOperations() {
    std::cout << "\n--- Test 3: Sequential Operations ---\n";
    
    Calculator calc;
    std::vector<Memento::Snapshot> history;
    std::vector<double> expected_results = {0, 5, 15, 10, 50, 25};
    
    // Save initial state
    history.push_back(calc.save());
    
    // Sequence of operations with saves
    calc.add(5);        // 0 + 5 = 5
    history.push_back(calc.save());
    
    calc.multiply(3);   // 5 * 3 = 15
    history.push_back(calc.save());
    
    calc.subtract(5);   // 15 - 5 = 10
    history.push_back(calc.save());
    
    calc.multiply(5);   // 10 * 5 = 50
    history.push_back(calc.save());
    
    calc.subtract(25);  // 50 - 25 = 25
    history.push_back(calc.save());
    
    std::cout << "Performed sequence of operations with saves" << std::endl;
    
    // Test restoring to each point in history
    bool all_correct = true;
    for (size_t i = 0; i < history.size(); i++) {
        calc.load(history[i]);
        double result = calc.getResult();
        std::cout << "Step " << i << ": result = " << result 
                  << " (expected: " << expected_results[i] << ")" << std::endl;
        
        if (result != expected_results[i]) {
            all_correct = false;
        }
    }
    
    printResult("Sequential Operations", all_correct);
}

void testUndoRedoScenario() {
    std::cout << "\n--- Test 4: Undo/Redo Scenario ---\n";
    
    Calculator calc;
    std::vector<Memento::Snapshot> undo_stack;
    
    // Function to save state for undo
    auto saveForUndo = [&]() {
        undo_stack.push_back(calc.save());
        if (undo_stack.size() > 5) {  // Limit undo history
            undo_stack.erase(undo_stack.begin());
        }
    };
    
    // Initial state
    saveForUndo();
    std::cout << "Starting calculator session" << std::endl;
    calc.showStatus();
    
    // Operation 1
    calc.add(10);
    saveForUndo();
    
    // Operation 2
    calc.multiply(2);
    saveForUndo();
    
    // Operation 3
    calc.subtract(3);
    saveForUndo();
    
    std::cout << "\nAfter operations:" << std::endl;
    calc.showStatus();
    
    // Undo last operation
    if (undo_stack.size() > 1) {
        undo_stack.pop_back();  // Remove current state
        calc.load(undo_stack.back());
        std::cout << "\nAfter undo:" << std::endl;
        calc.showStatus();
    }
    
    // Undo again
    if (undo_stack.size() > 1) {
        undo_stack.pop_back();
        calc.load(undo_stack.back());
        std::cout << "\nAfter second undo:" << std::endl;
        calc.showStatus();
    }
    
    // New operation (breaks redo chain)
    calc.add(5);
    saveForUndo();
    std::cout << "\nAfter new operation:" << std::endl;
    calc.showStatus();
    
    bool passed = (calc.getResult() == 15);  // 10 + 5 = 15
    printResult("Undo/Redo Scenario", passed);
}

void testComplexDataTypes() {
    std::cout << "\n--- Test 5: Complex Data Types ---\n";
    
    // Test with multiple data types
    class ComplexObject : public Memento {
    private:
        int m_intValue;
        double m_doubleValue;
        std::string m_stringValue;
        bool m_boolValue;
        
    protected:
        void saveToSnapshot(Snapshot& snapshot) const override {
            snapshot.set("int", m_intValue);
            snapshot.set("double", m_doubleValue);
            snapshot.set("string", m_stringValue);
            snapshot.set("bool", m_boolValue);
        }
        
        void loadFromSnapshot(const Snapshot& snapshot) override {
            m_intValue = snapshot.get<int>("int");
            m_doubleValue = snapshot.get<double>("double");
            m_stringValue = snapshot.get<std::string>("string");
            m_boolValue = snapshot.get<bool>("bool");
        }
        
    public:
        using Memento::save;
        using Memento::load;
        
        ComplexObject() : m_intValue(0), m_doubleValue(0.0), m_stringValue(""), m_boolValue(false) {}
        
        void setValues(int i, double d, const std::string& s, bool b) {
            m_intValue = i;
            m_doubleValue = d;
            m_stringValue = s;
            m_boolValue = b;
        }
        
        void showValues() const {
            std::cout << "  Values: int=" << m_intValue << ", double=" << m_doubleValue 
                      << ", string='" << m_stringValue << "', bool=" << (m_boolValue ? "true" : "false") << std::endl;
        }
        
        bool matches(int i, double d, const std::string& s, bool b) const {
            return (m_intValue == i && m_doubleValue == d && 
                    m_stringValue == s && m_boolValue == b);
        }
    };
    
    ComplexObject obj;
    
    // Set initial values
    obj.setValues(42, 3.14159, "Hello World", true);
    std::cout << "Initial values:" << std::endl;
    obj.showValues();
    
    // Save state
    auto snapshot = obj.save();
    
    // Change values
    obj.setValues(99, 2.71828, "Changed", false);
    std::cout << "Changed values:" << std::endl;
    obj.showValues();
    
    // Restore
    obj.load(snapshot);
    std::cout << "Restored values:" << std::endl;
    obj.showValues();
    
    bool passed = obj.matches(42, 3.14159, "Hello World", true);
    printResult("Complex Data Types", passed);
}

void testErrorHandling() {
    std::cout << "\n--- Test 6: Error Handling ---\n";
    
    Calculator calc1, calc2;
    
    calc1.add(10);
    calc1.multiply(2);
    
    calc2.subtract(5);
    calc2.add(15);
    
    std::cout << "Calculator 1:" << std::endl;
    calc1.showStatus();
    std::cout << "Calculator 2:" << std::endl;
    calc2.showStatus();
    
    // Save state from calc1
    auto snapshot_from_calc1 = calc1.save();
    
    // Load into calc2
    std::cout << "\nLoading calc1's state into calc2:" << std::endl;
    calc2.load(snapshot_from_calc1);
    calc2.showStatus();
    
    // Verify calc2 now has calc1's state
    bool state_transferred = (calc2.getResult() == calc1.getResult());
    
    // Original calc1 should be unchanged
    std::cout << "\nOriginal calc1 (should be unchanged):" << std::endl;
    calc1.showStatus();
    
    bool calc1_unchanged = (calc1.getResult() == 20);  // 10 * 2 = 20
    
    bool passed = state_transferred && calc1_unchanged;
    printResult("Error Handling", passed);
}

int main() {
    std::cout << CYAN << "Memento Extended Tests" << RESET << std::endl;
    std::cout << "======================" << std::endl;
    
    testBasicSaveRestore();
    testMultipleSnapshots();
    testSequentialOperations();
    testUndoRedoScenario();
    testComplexDataTypes();
    testErrorHandling();
    
    std::cout << "\n" << YELLOW << "Memento tests completed!" << RESET << std::endl;
    
    return 0;
}