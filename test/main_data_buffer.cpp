#include "libftpp.hpp"
#include <iostream>
#include <string>
#include <exception>
#include <cassert>

class TestObject {
public:
    int x;
    std::string y;

    friend DataBuffer& operator<<(DataBuffer& p_buffer, const TestObject& p_object) {
        p_buffer << p_object.x << p_object.y;
        return p_buffer;
    }

    friend DataBuffer& operator>>(DataBuffer& p_buffer, TestObject& p_object) {
        p_buffer >> p_object.x >> p_object.y;
        return p_buffer;
    }
};

int main() {
    std::cout << "=== DataBuffer Tests ===" << std::endl;
    
    // Test 1: Basic types
    {
        DataBuffer buf;
        int i = 42; float f = 3.14f; std::string s = "test";
        buf << i << f << s;
        int ri; float rf; std::string rs;
        buf >> ri >> rf >> rs;
        assert(i == ri && f == rf && s == rs);
        std::cout << "✓ Basic types" << std::endl;
    }
    
    // Test 2: Custom objects (tu test original)
    {
        DataBuffer myBuffer;
        TestObject obj1{42, "Hello"}, obj2{99, "World"};
        myBuffer << obj1 << obj2;
        TestObject r1, r2;
        myBuffer >> r1 >> r2;
        assert(r1.x == 42 && r1.y == "Hello");
        assert(r2.x == 99 && r2.y == "World");
        std::cout << "✓ Custom objects" << std::endl;
    }
    
    // Test 3: Exceptions
    {
        DataBuffer buf;
        int value;
        bool caught = false;
        try {
            buf >> value;
        } catch (...) {
            caught = true;
        }
        assert(caught);
        std::cout << "✓ Exception handling" << std::endl;
    }
    
    // Test 4: Buffer operations
    {
        DataBuffer buf;
        buf << 42;
        assert(buf.size() > 0);
        buf.clear();
        assert(buf.size() == 0);
        std::cout << "✓ Buffer operations" << std::endl;
    }
    
    std::cout << "\n✅ All tests passed!" << std::endl;
    return 0;
}