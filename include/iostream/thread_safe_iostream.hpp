/**
 * @file thread_safe_iostream.hpp
 * @brief Thread-safe version of iostream with prefixed lines.
 */

#ifndef THREAD_SAFE_IOSTREAM_HPP
#define THREAD_SAFE_IOSTREAM_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <mutex>
#include <thread>

/**
 * @brief Thread-safe I/O stream with line prefixing capabilities.
 *
 * This class provides thread-safe input/output operations with automatic
 * line prefixing. Each thread can have its own prefix, and all I/O operations
 * are synchronized to prevent race conditions and mixed output.
 */
class ThreadSafeIOStream
{
private:
    /** Global mutex for synchronizing all I/O operations */
    static std::mutex s_ioMutex;
    
    /** Thread-local prefix string */
    thread_local static std::string s_prefix;
    
    /** Internal buffer for building complete lines */
    mutable std::ostringstream m_buffer;
    
    /** Mutex for protecting the internal buffer */
    mutable std::mutex m_bufferMutex;

public:
    /**
     * @brief Default constructor.
     */
    ThreadSafeIOStream() = default;
    
    /**
     * @brief Copy constructor (deleted for thread safety).
     */
    ThreadSafeIOStream(const ThreadSafeIOStream&) = delete;
    
    /**
     * @brief Assignment operator (deleted for thread safety).
     */
    ThreadSafeIOStream& operator=(const ThreadSafeIOStream&) = delete;
    
    /**
     * @brief Set the prefix for the current thread.
     *
     * @param prefix The prefix string to prepend to each line
     */
    void setPrefix(const std::string& prefix);
    
    /**
     * @brief Prompt the user with a question and read the response.
     *
     * This method is thread-safe and will display the question with the
     * current thread's prefix, then read the user's response.
     *
     * @tparam T Type of the destination variable
     * @param question The question to display to the user
     * @param dest Reference to store the user's response
     */
    template<typename T>
    void prompt(const std::string& question, T& dest);
    
    /**
     * @brief Thread-safe output operator.
     *
     * @tparam T Type of the value to output
     * @param value The value to output
     * @return Reference to this stream for chaining
     */
    template<typename T>
    ThreadSafeIOStream& operator<<(const T& value);
    
    /**
     * @brief Thread-safe input operator.
     *
     * @tparam T Type of the value to input
     * @param value Reference to store the input value
     * @return Reference to this stream for chaining
     */
    template<typename T>
    ThreadSafeIOStream& operator>>(T& value);
    
    /**
     * @brief Handle special stream manipulators like std::endl.
     *
     * @param manip Stream manipulator function
     * @return Reference to this stream for chaining
     */
    ThreadSafeIOStream& operator<<(std::ostream& (*manip)(std::ostream&));

private:
    /**
     * @brief Flush the internal buffer to stdout with prefix.
     */
    void flushBuffer() const;
    
    /**
     * @brief Get the current thread's prefix.
     *
     * @return The prefix string for the current thread
     */
    std::string getCurrentPrefix() const;
};

// Template method implementations

template<typename T>
void ThreadSafeIOStream::prompt(const std::string& question, T& dest)
{
    std::lock_guard<std::mutex> lock(s_ioMutex);
    
    // Display question with prefix
    std::cout << getCurrentPrefix() << question << " ";
    std::cout.flush();
    
    // Read response
    std::cin >> dest;
}

template<typename T>
ThreadSafeIOStream& ThreadSafeIOStream::operator<<(const T& value)
{
    std::lock_guard<std::mutex> lock(m_bufferMutex);
    m_buffer << value;
    return *this;
}

template<typename T>
ThreadSafeIOStream& ThreadSafeIOStream::operator>>(T& value)
{
    std::lock_guard<std::mutex> lock(s_ioMutex);
    
    // Display prefix before reading
    std::cout << getCurrentPrefix();
    std::cout.flush();
    
    // Read input
    std::cin >> value;
    
    return *this;
}

// Global thread-safe iostream instance
extern thread_local ThreadSafeIOStream threadSafeCout;

#endif