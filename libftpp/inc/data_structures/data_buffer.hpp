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
    /** Internal byte buffer */
    std::vector<uint8_t> m_buffer;
    
    /** Current read position for extraction operations */
    size_t m_readPosition;

public:
    /**
     * @brief Default constructor.
     *
     * Creates an empty buffer with read position at the beginning.
     */
    DataBuffer();
    
    /**
     * @brief Move constructor for efficient buffer transfers.
     * @param other Buffer to move from
     */
    DataBuffer(DataBuffer&& other) noexcept;
    
    /**
     * @brief Move assignment operator.
     * @param other Buffer to move from
     * @return Reference to this buffer
     */
    DataBuffer& operator=(DataBuffer&& other) noexcept;
    
    /**
     * @brief Copy constructor.
     */
    DataBuffer(const DataBuffer&) = default;
    
    /**
     * @brief Copy assignment operator.
     */
    DataBuffer& operator=(const DataBuffer&) = default;
    
    /**
     * @brief Destructor.
     */
    ~DataBuffer();
    
    /**
     * @brief Clears the buffer and resets read position.
     */
    void clear();
    
    /**
     * @brief Returns the current size of the buffer.
     * @return Size of the buffer in bytes
     */
    size_t size() const;
    
    /**
     * @brief Returns a pointer to the raw buffer data.
     * @return Const pointer to the internal buffer
     */
    const uint8_t* data() const;
    
    /**
     * @brief Reserve capacity to avoid reallocations.
     * @param capacity Number of bytes to reserve
     */
    void reserve(size_t capacity);
    
    /**
     * @brief Get remaining bytes available for reading.
     * @return Number of bytes that can still be read
     */
    size_t getBytesRemaining() const;
    
    /**
     * @brief Serialization operator for storing objects in the buffer.
     *
     * Converts an object to its byte representation and appends it to the buffer.
     * Uses direct memory representation for trivial types.
     *
     * @tparam T Type of object to serialize
     * @param buffer Buffer to store the object in
     * @param data Object to serialize
     * @return Reference to the buffer for operator chaining
     * @throws std::bad_alloc if memory allocation fails
     */
    template<typename T>
    friend DataBuffer& operator<<(DataBuffer& buffer, const T& data);
    
    /**
     * @brief Deserialization operator for retrieving objects from the buffer.
     *
     * Extracts an object from the buffer at the current read position.
     * The read position is advanced by the size of the extracted object.
     *
     * @tparam T Type of object to deserialize
     * @param buffer Buffer to extract the object from
     * @param data Object where the deserialized data will be stored
     * @return Reference to the buffer for operator chaining
     * @throws std::runtime_error if there is not enough data in the buffer
     */
    template<typename T>
    friend DataBuffer& operator>>(DataBuffer& buffer, T& data);
};

// Template implementations must be in the header file

template<typename T>
DataBuffer& operator<<(DataBuffer& buffer, const T& data)
{
    // Get byte representation of the data
    const uint8_t* bytePtr = reinterpret_cast<const uint8_t*>(&data);
    
    // Reserve space if needed to avoid multiple reallocations
    if (buffer.m_buffer.capacity() < buffer.m_buffer.size() + sizeof(T)) {
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
    if (buffer.m_readPosition + sizeof(T) > buffer.m_buffer.size()) {
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

// Declare specializations for std::string
template<>
DataBuffer& operator<<(DataBuffer& buffer, const std::string& data);

template<>
DataBuffer& operator>>(DataBuffer& buffer, std::string& data);

#endif