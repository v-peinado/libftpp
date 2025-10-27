/**
 * @file data_buffer.hpp
 * @brief Polymorphic container for storing objects in byte format.
 */

#ifndef DATA_BUFFER_HPP
#define DATA_BUFFER_HPP

#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <cstring>
#include <limits>

/**
 * @brief A polymorphic container for binary serialization and deserialization.
 *
 * DataBuffer allows storing and retrieving objects in byte format using
 * stream-like operators. It provides a type-safe way to serialize objects
 * into a binary buffer and later deserialize them.
 * 
 * @note Exception handling: All exceptions are propagated to the caller.
 * Reading operations will throw if there is insufficient data in the buffer.
 */
class DataBuffer
{
    private:
        
        std::vector<uint8_t> m_buffer;      // Internal byte buffer
        size_t m_readPosition;              // Current read position for extraction operations

    public:

        /* Constructors and destructors */

        DataBuffer();
        ~DataBuffer();

        DataBuffer(DataBuffer&& other) noexcept;
        DataBuffer& operator=(DataBuffer&& other) noexcept;
        DataBuffer(const DataBuffer&) = default;
        DataBuffer& operator=(const DataBuffer&) = default;
        
        /* Utility methods */ 

        void clear();
        size_t size() const;
        const uint8_t* data() const;
        void reserve(size_t capacity);
        size_t getBytesRemaining() const;
        
        /* Core methods */

        template<typename T>
        friend DataBuffer& operator<<(DataBuffer& buffer, const T& data);
        
        template<typename T>
        friend DataBuffer& operator>>(DataBuffer& buffer, T& data);
};

// ============================================
//           DATABUFFER IMPLEMENTATIONS
// ============================================

template<typename T>
DataBuffer& operator<<(DataBuffer& buffer, const T& data)
{
    // Get byte representation of the data
    const uint8_t* bytePtr = reinterpret_cast<const uint8_t*>(&data);
    
    // Reserve space if needed to avoid multiple reallocations
    if (buffer.m_buffer.capacity() < buffer.m_buffer.size() + sizeof(T))
    {
        buffer.m_buffer.reserve(buffer.m_buffer.size() + sizeof(T));
    }
    
    // Append bytes to the buffer
    buffer.m_buffer.insert(buffer.m_buffer.end(), bytePtr, bytePtr + sizeof(T));
    
    return buffer;
}

template<typename T>
DataBuffer& operator>>(DataBuffer& buffer, T& data)
{
    // Check if there's enough data in the buffer
    if (buffer.m_readPosition + sizeof(T) > buffer.m_buffer.size())
    {
        throw std::runtime_error(
            "DataBuffer underflow: attempted to read " + std::to_string(sizeof(T)) + 
            " bytes, only " + std::to_string(buffer.getBytesRemaining()) + " bytes available"
        );
    }
    
    // Copy bytes from buffer to the data object using memcpy for efficiency
    std::memcpy(&data, buffer.m_buffer.data() + buffer.m_readPosition, sizeof(T));
    
    // Advance read position
    buffer.m_readPosition += sizeof(T);
    
    return buffer;
}

// Declare specializations for std::string, al ser una implementacion completa, se implementa en .cpp
// Es necesario esta distincion porque std::string tiene puntero interno, si se serializa se serializaria la direccion de memoria no el contenido
template<>
DataBuffer& operator<<(DataBuffer& buffer, const std::string& data);

template<>
DataBuffer& operator>>(DataBuffer& buffer, std::string& data);

#endif