/**
 * @file data_buffer.cpp
 * @brief Implementation of the DataBuffer class.
 */

#include "data_structures/data_buffer.hpp"
#include <stdexcept>
#include <limits>

// Constructor
DataBuffer::DataBuffer() : m_readPosition(0) 
{
    // Reserve some initial capacity to avoid early reallocations
    m_buffer.reserve(64);
}

// Move constructor
DataBuffer::DataBuffer(DataBuffer&& other) noexcept
    : m_buffer(std::move(other.m_buffer))
    , m_readPosition(other.m_readPosition)
{
    other.m_readPosition = 0;
}

// Move assignment operator
DataBuffer& DataBuffer::operator=(DataBuffer&& other) noexcept
{
    if (this != &other) {
        m_buffer = std::move(other.m_buffer);
        m_readPosition = other.m_readPosition;
        other.m_readPosition = 0;
    }
    return *this;
}

// Destructor
DataBuffer::~DataBuffer() {}

// Buffer management methods
void DataBuffer::clear()
{
    m_buffer.clear();
    m_readPosition = 0;
}

size_t DataBuffer::size() const
{
    return m_buffer.size();
}

const uint8_t* DataBuffer::data() const
{
    return m_buffer.data();
}

void DataBuffer::reserve(size_t capacity)
{
    m_buffer.reserve(capacity);
}

size_t DataBuffer::getBytesRemaining() const
{
    return (m_readPosition < m_buffer.size()) 
           ? (m_buffer.size() - m_readPosition)
           : 0;
}

/**
 * @brief Specialization for std::string serialization.
 */
template<>
DataBuffer& operator<<(DataBuffer& buffer, const std::string& data)
{
    // Validate string size fits in uint32_t
    if (data.size() > std::numeric_limits<uint32_t>::max()) {
        throw std::length_error("String too large for serialization");
    }
    
    // Store string length followed by character data
    uint32_t length = static_cast<uint32_t>(data.size());
    buffer << length;
    
    // Reserve space for efficiency
    if (!data.empty()) {
        buffer.reserve(buffer.size() + data.size());
        
        // Store string data efficiently
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data.data());
        buffer.m_buffer.insert(buffer.m_buffer.end(), bytes, bytes + data.size());
    }
    
    return buffer;
}

/**
 * @brief Specialization for std::string deserialization.
 * @throws std::runtime_error if buffer contains insufficient data
 */
template<>
DataBuffer& operator>>(DataBuffer& buffer, std::string& data)
{
    // Read string length
    uint32_t length;
    buffer >> length;
    
    // Validate buffer has enough data
    if (buffer.getBytesRemaining() < length) {
        throw std::runtime_error(
            "DataBuffer underflow: string requires " + std::to_string(length) + 
            " bytes, only " + std::to_string(buffer.getBytesRemaining()) + " bytes available"
        );
    }
    
    // Extract string data
    data.clear();
    
    if (length > 0) {
        data.reserve(length);
        
        // Direct assignment for efficiency
        const char* charData = reinterpret_cast<const char*>(
            buffer.m_buffer.data() + buffer.m_readPosition
        );
        data.assign(charData, length);
        
        buffer.m_readPosition += length;
    }
    
    return buffer;
}