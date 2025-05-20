/**
 * @file data_buffer.cpp
 * @brief Implementation of the DataBuffer class.
 */

#include "data_structures/data_buffer.hpp"
#include <stdexcept>

// Constructor and destructor
DataBuffer::DataBuffer() : m_readPosition(0) {}
DataBuffer::~DataBuffer() {}

// Buffer management methods
void DataBuffer::clear() {
    m_buffer.clear();
    m_readPosition = 0;
}

size_t DataBuffer::size() const {
    return m_buffer.size();
}

const uint8_t* DataBuffer::data() const {
    return m_buffer.data();
}

/**
 * @brief Specialization for std::string serialization.
 */
template<>
DataBuffer& operator<<(DataBuffer& buffer, const std::string& data) {
    // Store string length followed by character data
    uint32_t length = static_cast<uint32_t>(data.size());
    buffer << length;
    
    for (char c : data) {
        buffer.m_buffer.push_back(static_cast<uint8_t>(c));
    }
    
    return buffer;
}

/**
 * @brief Specialization for std::string deserialization.
 * @throws std::runtime_error if buffer contains insufficient data
 */
template<>
DataBuffer& operator>>(DataBuffer& buffer, std::string& data) {
    // Read string length
    uint32_t length;
    buffer >> length;
    
    // Validate buffer has enough data
    if (buffer.m_readPosition + length > buffer.m_buffer.size()) {
        throw std::runtime_error("Buffer overflow: not enough data to read string");
    }
    
    // Extract string data
    data.clear();
    data.reserve(length);
    
    for (uint32_t i = 0; i < length; ++i) {
        data.push_back(static_cast<char>(buffer.m_buffer[buffer.m_readPosition++]));
    }
    
    return buffer;
}