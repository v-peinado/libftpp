#include "libftpp.hpp"  // Incluye todo desde libftpp/
#include <iostream>

class TestObject {
private:
    int m_value;
    
public:
    TestObject() : m_value(0) { 
        std::cout << "TestObject default constructor" << std::endl; 
    }
    
    TestObject(int value) : m_value(value) { 
        std::cout << "TestObject constructor with value [" << value << "]" << std::endl;
    }
    
    ~TestObject() { 
        std::cout << "TestObject destructor" << std::endl; 
    }

    void sayHello() const { 
        std::cout << "Hello from TestObject with value: " << m_value << std::endl; 
    }
};

int main() {
    std::cout << "=== Pool Test ===" << std::endl;
    
    // Create a Pool for TestObject
    Pool<TestObject> myPool;

    // Resize the pool to pre-allocate 5 objects
    std::cout << "\n--- Resizing pool to 5 objects ---" << std::endl;
    myPool.resize(5);

    // Acquire an object with parameter
    std::cout << "\n--- Acquiring obj1 with value 15 ---" << std::endl;
    Pool<TestObject>::Object obj1 = myPool.acquire(15);
    obj1->sayHello();

    {
        std::cout << "\n--- Acquiring obj2 (default) in scope ---" << std::endl;
        Pool<TestObject>::Object obj2 = myPool.acquire();
        obj2->sayHello();
        std::cout << "--- obj2 going out of scope ---" << std::endl;
    }

    // Acquire another object
    std::cout << "\n--- Acquiring obj3 (should reuse obj2's slot) ---" << std::endl;
    Pool<TestObject>::Object obj3 = myPool.acquire();
    obj3->sayHello();

    std::cout << "\n--- End of program ---" << std::endl;
    return 0;
}