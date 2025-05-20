/**
 * @file data_buffer.hpp
 * @brief Polymorphic container for storing objects in byte format.
 */

#ifndef DATA_BUFFER_HPP
# define DATA_BUFFER_HPP

#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept>

/**
 * @brief A polymorphic container for binary serialization and deserialization.
 *
 * DataBuffer allows storing and retrieving objects in byte format using
 * stream-like operators. It provides a type-safe way to serialize objects
 * into a binary buffer and later deserialize them.
 */

class DataBuffer {
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
         * @brief Destructor.
         */
        ~DataBuffer();
        
        /**
         * @brief Clears the buffer and resets read position.
         */
        void clear();
        
        /**
         * @brief Returns the current size of the buffer.
         *
         * @return Size of the buffer in bytes
         */
        size_t size() const;
        
        /**
         * @brief Returns a pointer to the raw buffer data.
         *
         * @return Const pointer to the internal buffer
         */
        const uint8_t* data() const;
        
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
DataBuffer& operator<<(DataBuffer& buffer, const T& data) {
    // Get byte representation of the data
    const uint8_t* bytePtr = reinterpret_cast<const uint8_t*>(&data);
    
    // Append bytes to the buffer
    for (size_t i = 0; i < sizeof(T); ++i) {
        buffer.m_buffer.push_back(bytePtr[i]);
    }
    
    return buffer;
}

template<typename T>
DataBuffer& operator>>(DataBuffer& buffer, T& data) {
    // Check if there's enough data in the buffer
    if (buffer.m_readPosition + sizeof(T) > buffer.m_buffer.size()) {
        throw std::runtime_error("Buffer overflow: not enough data to read");
    }
    
    // Copy bytes from buffer to the data object
    for (size_t i = 0; i < sizeof(T); ++i) {
        reinterpret_cast<uint8_t*>(&data)[i] = buffer.m_buffer[buffer.m_readPosition + i];
    }
    
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