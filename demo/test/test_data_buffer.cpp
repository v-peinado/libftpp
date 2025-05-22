/**
 * @file test_data_buffer.cpp
 * @brief Extended tests for DataBuffer
 */

#include "../libftpp.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cassert>

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

void testBasicTypes() {
    std::cout << "\n--- Test 1: Basic Types ---\n";
    
    DataBuffer buffer;
    
    // Test data
    bool flag = true;
    char letter = 'A';
    int number = 42;
    float price = 19.99f;
    double pi = 3.14159265359;
    
    std::cout << "Storing: bool=" << flag << ", char=" << letter 
              << ", int=" << number << ", float=" << price 
              << ", double=" << pi << std::endl;
    
    // Store
    buffer << flag << letter << number << price << pi;
    std::cout << "Buffer size: " << buffer.size() << " bytes" << std::endl;
    
    // Retrieve
    bool stored_flag;
    char stored_letter;
    int stored_number;
    float stored_price;
    double stored_pi;
    
    buffer >> stored_flag >> stored_letter >> stored_number >> stored_price >> stored_pi;
    
    std::cout << "Retrieved: bool=" << stored_flag << ", char=" << stored_letter 
              << ", int=" << stored_number << ", float=" << stored_price 
              << ", double=" << stored_pi << std::endl;
    
    bool passed = (flag == stored_flag && letter == stored_letter && 
                   number == stored_number && price == stored_price && 
                   pi == stored_pi);
    
    printResult("Basic Types", passed);
}

void testStrings() {
    std::cout << "\n--- Test 2: Strings ---\n";
    
    DataBuffer buffer;
    
    std::string short_str = "Hi";
    std::string long_str = "This is a much longer string to test string handling";
    std::string empty_str = "";
    std::string special_str = "String with spaces and symbols: @#$%^&*()";
    
    std::cout << "Storing 4 different strings..." << std::endl;
    std::cout << "Short: '" << short_str << "'" << std::endl;
    std::cout << "Long: '" << long_str << "'" << std::endl;
    std::cout << "Empty: '" << empty_str << "'" << std::endl;
    std::cout << "Special: '" << special_str << "'" << std::endl;
    
    buffer << short_str << long_str << empty_str << special_str;
    std::cout << "Buffer size: " << buffer.size() << " bytes" << std::endl;
    
    std::string retrieved_short, retrieved_long, retrieved_empty, retrieved_special;
    buffer >> retrieved_short >> retrieved_long >> retrieved_empty >> retrieved_special;
    
    std::cout << "\nRetrieved strings:" << std::endl;
    std::cout << "Short: '" << retrieved_short << "'" << std::endl;
    std::cout << "Long: '" << retrieved_long << "'" << std::endl;
    std::cout << "Empty: '" << retrieved_empty << "'" << std::endl;
    std::cout << "Special: '" << retrieved_special << "'" << std::endl;
    
    bool passed = (short_str == retrieved_short && 
                   long_str == retrieved_long && 
                   empty_str == retrieved_empty && 
                   special_str == retrieved_special);
    
    printResult("Strings", passed);
}

void testMixedData() {
    std::cout << "\n--- Test 3: Mixed Data ---\n";
    
    DataBuffer buffer;
    
    // Person data
    std::string name = "Alice Johnson";
    int age = 28;
    float height = 1.68f;
    bool married = false;
    std::string city = "New York";
    double salary = 75000.50;
    
    std::cout << "Storing person data:" << std::endl;
    std::cout << "Name: " << name << std::endl;
    std::cout << "Age: " << age << std::endl;
    std::cout << "Height: " << height << "m" << std::endl;
    std::cout << "Married: " << (married ? "Yes" : "No") << std::endl;
    std::cout << "City: " << city << std::endl;
    std::cout << "Salary: $" << salary << std::endl;
    
    buffer << name << age << height << married << city << salary;
    std::cout << "\nSerialized to " << buffer.size() << " bytes" << std::endl;
    
    // Retrieve in same order
    std::string r_name;
    int r_age;
    float r_height;
    bool r_married;
    std::string r_city;
    double r_salary;
    
    buffer >> r_name >> r_age >> r_height >> r_married >> r_city >> r_salary;
    
    std::cout << "\nRetrieved person data:" << std::endl;
    std::cout << "Name: " << r_name << std::endl;
    std::cout << "Age: " << r_age << std::endl;
    std::cout << "Height: " << r_height << "m" << std::endl;
    std::cout << "Married: " << (r_married ? "Yes" : "No") << std::endl;
    std::cout << "City: " << r_city << std::endl;
    std::cout << "Salary: $" << r_salary << std::endl;
    
    bool passed = (name == r_name && age == r_age && height == r_height && 
                   married == r_married && city == r_city && salary == r_salary);
    
    printResult("Mixed Data", passed);
}

void testMultipleOperations() {
    std::cout << "\n--- Test 4: Multiple Operations ---\n";
    
    DataBuffer buffer1, buffer2;
    
    // First buffer
    int value1 = 100;
    std::string text1 = "Buffer1";
    buffer1 << value1 << text1;
    
    // Second buffer
    int value2 = 200;
    std::string text2 = "Buffer2";
    buffer2 << value2 << text2;
    
    std::cout << "Created two separate buffers" << std::endl;
    std::cout << "Buffer1 size: " << buffer1.size() << " bytes" << std::endl;
    std::cout << "Buffer2 size: " << buffer2.size() << " bytes" << std::endl;
    
    // Read from both
    int r_value1, r_value2;
    std::string r_text1, r_text2;
    
    buffer1 >> r_value1 >> r_text1;
    buffer2 >> r_value2 >> r_text2;
    
    std::cout << "Buffer1 data: " << r_value1 << ", '" << r_text1 << "'" << std::endl;
    std::cout << "Buffer2 data: " << r_value2 << ", '" << r_text2 << "'" << std::endl;
    
    bool passed = (value1 == r_value1 && text1 == r_text1 && 
                   value2 == r_value2 && text2 == r_text2);
    
    printResult("Multiple Operations", passed);
}

void testClearAndReuse() {
    std::cout << "\n--- Test 5: Clear and Reuse ---\n";
    
    DataBuffer buffer;
    
    // First use
    int first_value = 123;
    buffer << first_value;
    std::cout << "First use - stored: " << first_value << ", size: " << buffer.size() << std::endl;
    
    int retrieved_first;
    buffer >> retrieved_first;
    std::cout << "Retrieved: " << retrieved_first << std::endl;
    
    // Clear buffer
    buffer.clear();
    std::cout << "Buffer cleared, size now: " << buffer.size() << std::endl;
    
    // Second use
    std::string second_value = "Reused buffer";
    buffer << second_value;
    std::cout << "Second use - stored: '" << second_value << "', size: " << buffer.size() << std::endl;
    
    std::string retrieved_second;
    buffer >> retrieved_second;
    std::cout << "Retrieved: '" << retrieved_second << "'" << std::endl;
    
    bool passed = (first_value == retrieved_first && 
                   second_value == retrieved_second && 
                   buffer.size() > 0);
    
    printResult("Clear and Reuse", passed);
}

void testErrorHandling() {
    std::cout << "\n--- Test 6: Error Handling ---\n";
    
    DataBuffer buffer;
    
    // Store only one integer
    int stored_value = 42;
    buffer << stored_value;
    std::cout << "Stored one integer: " << stored_value << std::endl;
    
    // Try to read more than available
    int retrieved_value;
    std::string extra_string;
    
    buffer >> retrieved_value;
    std::cout << "Successfully retrieved integer: " << retrieved_value << std::endl;
    
    bool error_caught = false;
    try {
        buffer >> extra_string;  // This should fail
        std::cout << RED << "ERROR: Should have thrown exception!" << RESET << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Correctly caught exception: " << e.what() << std::endl;
        error_caught = true;
    }
    
    bool passed = (stored_value == retrieved_value && error_caught);
    printResult("Error Handling", passed);
}

int main() {
    std::cout << CYAN << "DataBuffer Extended Tests" << RESET << std::endl;
    std::cout << "=========================" << std::endl;
    
    testBasicTypes();
    testStrings();
    testMixedData();
    testMultipleOperations();
    testClearAndReuse();
    testErrorHandling();
    
    std::cout << "\n" << YELLOW << "DataBuffer tests completed!" << RESET << std::endl;
    
    return 0;
}