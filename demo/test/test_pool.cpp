/**
 * @file test_pool.cpp
 * @brief Extended tests for Pool
 */

#include "../libftpp.hpp"
#include <iostream>
#include <string>
#include <vector>
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

// Test class for pool
class TestObject {
private:
    int m_id;
    std::string m_name;
    
public:
    TestObject() : m_id(0), m_name("default") {
        std::cout << "  Default constructor: id=" << m_id << std::endl;
    }
    
    TestObject(int id) : m_id(id), m_name("object_" + std::to_string(id)) {
        std::cout << "  Constructor: id=" << m_id << ", name=" << m_name << std::endl;
    }
    
    TestObject(int id, const std::string& name) : m_id(id), m_name(name) {
        std::cout << "  Constructor: id=" << m_id << ", name=" << m_name << std::endl;
    }
    
    ~TestObject() {
        std::cout << "  Destructor: id=" << m_id << ", name=" << m_name << std::endl;
    }
    
    int getId() const { return m_id; }
    const std::string& getName() const { return m_name; }
    
    void doWork() {
        std::cout << "  Object " << m_id << " (" << m_name << ") is working" << std::endl;
    }
};

void testBasicPoolOperations() {
    std::cout << "\n--- Test 1: Basic Pool Operations ---\n";
    
    Pool<TestObject> pool;
    std::cout << "Pool created" << std::endl;
    
    // Initialize pool
    pool.resize(3);
    std::cout << "Pool resized to 3 objects" << std::endl;
    std::cout << "Available objects: " << pool.available() << std::endl;
    std::cout << "Objects in use: " << pool.inUse() << std::endl;
    
    bool test1_passed = (pool.size() == 3 && pool.available() == 3 && pool.inUse() == 0);
    
    // Acquire one object
    std::cout << "\nAcquiring first object:" << std::endl;
    {
        auto obj = pool.acquire(1, "first_object");
        std::cout << "Object acquired - ID: " << obj->getId() << ", Name: " << obj->getName() << std::endl;
        std::cout << "Available objects: " << pool.available() << std::endl;
        std::cout << "Objects in use: " << pool.inUse() << std::endl;
        
        bool test2_passed = (pool.available() == 2 && pool.inUse() == 1);
        test1_passed = test1_passed && test2_passed;
        
        obj->doWork();
    }
    
    std::cout << "Object returned to pool" << std::endl;
    std::cout << "Available objects: " << pool.available() << std::endl;
    std::cout << "Objects in use: " << pool.inUse() << std::endl;
    
    bool test3_passed = (pool.available() == 3 && pool.inUse() == 0);
    bool passed = test1_passed && test3_passed;
    
    printResult("Basic Pool Operations", passed);
}

void testMultipleAcquisitions() {
    std::cout << "\n--- Test 2: Multiple Acquisitions ---\n";
    
    Pool<TestObject> pool;
    pool.resize(2);
    std::cout << "Pool with 2 objects created" << std::endl;
    
    std::vector<int> acquired_ids;
    
    // Acquire multiple objects
    std::cout << "\nAcquiring multiple objects:" << std::endl;
    {
        auto obj1 = pool.acquire(10, "worker_1");
        std::cout << "Acquired object 1: " << obj1->getName() << std::endl;
        acquired_ids.push_back(obj1->getId());
        
        auto obj2 = pool.acquire(20, "worker_2");
        std::cout << "Acquired object 2: " << obj2->getName() << std::endl;
        acquired_ids.push_back(obj2->getId());
        
        std::cout << "Pool status - Available: " << pool.available() 
                  << ", In use: " << pool.inUse() << std::endl;
        
        bool pool_empty = pool.isEmpty();
        bool pool_not_full = !pool.isFull();
        
        obj1->doWork();
        obj2->doWork();
        
        // Test passed if pool is empty and both objects work
        bool test_passed = pool_empty && pool_not_full && 
                          acquired_ids.size() == 2 && 
                          pool.inUse() == 2;
        
        printResult("Multiple Acquisitions", test_passed);
    }
    
    std::cout << "All objects returned to pool" << std::endl;
    std::cout << "Pool status - Available: " << pool.available() 
              << ", In use: " << pool.inUse() << std::endl;
}

void testPoolExhaustion() {
    std::cout << "\n--- Test 3: Pool Exhaustion ---\n";
    
    Pool<TestObject> pool;
    pool.resize(1);  // Only one object
    std::cout << "Pool with only 1 object created" << std::endl;
    
    bool exception_caught = false;
    
    {
        auto obj1 = pool.acquire(100, "only_object");
        std::cout << "Acquired the only available object: " << obj1->getName() << std::endl;
        std::cout << "Pool is empty: " << (pool.isEmpty() ? "Yes" : "No") << std::endl;
        
        // Try to acquire another object (should fail)
        try {
            auto obj2 = pool.acquire(200, "should_fail");
            std::cout << RED << "ERROR: Should have failed!" << RESET << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Correctly caught exception: " << e.what() << std::endl;
            exception_caught = true;
        }
        
        obj1->doWork();
    }
    
    std::cout << "Object returned, pool is full again: " << (pool.isFull() ? "Yes" : "No") << std::endl;
    
    printResult("Pool Exhaustion", exception_caught && pool.isFull());
}

void testObjectReuse() {
    std::cout << "\n--- Test 4: Object Reuse ---\n";
    
    Pool<TestObject> pool;
    pool.resize(1);
    std::cout << "Pool with 1 object for reuse test" << std::endl;
    
    std::vector<std::string> names_used;
    
    // Use object multiple times
    for (int i = 1; i <= 3; i++) {
        std::cout << "\nRound " << i << ":" << std::endl;
        {
            std::string name = "round_" + std::to_string(i) + "_object";
            auto obj = pool.acquire(i * 10, name);
            names_used.push_back(obj->getName());
            std::cout << "Using object: " << obj->getName() << " (ID: " << obj->getId() << ")" << std::endl;
            obj->doWork();
        }
        std::cout << "Object returned to pool" << std::endl;
    }
    
    // Verify all names were different (object was reinitialized each time)
    bool all_different = (names_used[0] != names_used[1] && 
                         names_used[1] != names_used[2] && 
                         names_used[0] != names_used[2]);
    
    std::cout << "\nNames used: ";
    for (const auto& name : names_used) {
        std::cout << "'" << name << "' ";
    }
    std::cout << std::endl;
    
    printResult("Object Reuse", all_different && names_used.size() == 3);
}

void testPerformanceComparison() {
    std::cout << "\n--- Test 5: Performance Comparison ---\n";
    
    const int iterations = 1000;
    std::cout << "Comparing performance over " << iterations << " iterations" << std::endl;
    
    // Test without pool (traditional approach)
    auto start1 = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; i++) {
        TestObject* obj = new TestObject(i, "traditional");
        delete obj;
    }
    
    auto end1 = std::chrono::high_resolution_clock::now();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1);
    
    std::cout << "Traditional approach: " << duration1.count() << " microseconds" << std::endl;
    
    // Test with pool
    Pool<TestObject> pool;
    pool.resize(10);  // Pool of 10 objects
    
    auto start2 = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; i++) {
        auto obj = pool.acquire(i, "pooled");
        // Object automatically returns to pool at end of scope
    }
    
    auto end2 = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);
    
    std::cout << "Pool approach: " << duration2.count() << " microseconds" << std::endl;
    
    double improvement = (double)duration1.count() / duration2.count();
    std::cout << "Pool is " << improvement << "x faster" << std::endl;
    
    bool performance_better = (duration2 < duration1);
    printResult("Performance Comparison", performance_better);
}

void testPoolResize() {
    std::cout << "\n--- Test 6: Pool Resize ---\n";
    
    Pool<TestObject> pool;
    
    // Initial size
    pool.resize(2);
    std::cout << "Initial pool size: " << pool.size() << std::endl;
    
    // Use one object
    {
        auto obj = pool.acquire(1, "before_resize");
        std::cout << "Object acquired before resize" << std::endl;
        std::cout << "Available: " << pool.available() << ", In use: " << pool.inUse() << std::endl;
        
        // Resize while object is in use
        std::cout << "\nResizing pool to 4 objects..." << std::endl;
        pool.resize(4);
        std::cout << "New pool size: " << pool.size() << std::endl;
        std::cout << "Available: " << pool.available() << ", In use: " << pool.inUse() << std::endl;
        
        obj->doWork();
    }
    
    std::cout << "Object returned after resize" << std::endl;
    std::cout << "Final status - Size: " << pool.size() 
              << ", Available: " << pool.available() 
              << ", In use: " << pool.inUse() << std::endl;
    
    bool passed = (pool.size() == 4 && pool.available() == 4 && pool.inUse() == 0);
    printResult("Pool Resize", passed);
}

int main() {
    std::cout << CYAN << "Pool Extended Tests" << RESET << std::endl;
    std::cout << "===================" << std::endl;
    
    testBasicPoolOperations();
    testMultipleAcquisitions();
    testPoolExhaustion();
    testObjectReuse();
    testPerformanceComparison();
    testPoolResize();
    
    std::cout << "\n" << YELLOW << "Pool tests completed!" << RESET << std::endl;
    
    return 0;
}