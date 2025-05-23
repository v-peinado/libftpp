/**
 * @file thread_safe_iostream.cpp
 * @brief Implementation of the ThreadSafeIOStream class.
 */

#include "iostream/thread_safe_iostream.hpp"
#include <iomanip>

// Static member definitions
std::mutex ThreadSafeIOStream::s_ioMutex;
thread_local std::string ThreadSafeIOStream::s_prefix;

// Global thread-safe iostream instance
thread_local ThreadSafeIOStream threadSafeCout;

void ThreadSafeIOStream::setPrefix(const std::string& prefix)
{
    s_prefix = prefix;
}

ThreadSafeIOStream& ThreadSafeIOStream::operator<<(std::ostream& (*manip)(std::ostream&))
{
    std::lock_guard<std::mutex> bufferLock(m_bufferMutex);
    
    // If it's std::endl, flush the buffer with prefix
    if (manip == static_cast<std::ostream& (*)(std::ostream&)>(std::endl))
    {
        flushBuffer();
        m_buffer.str("");  // Clear buffer
        m_buffer.clear();  // Clear state flags
    }
    else
    {
        // For other manipulators, just add them to buffer
        m_buffer << manip;
    }
    
    return *this;
}

void ThreadSafeIOStream::flushBuffer() const
{
    std::lock_guard<std::mutex> lock(s_ioMutex);
    
    std::string content = m_buffer.str();
    if (!content.empty())
    {
        // Add prefix and output
        std::cout << getCurrentPrefix() << content << std::endl;
        std::cout.flush();
    }
}

std::string ThreadSafeIOStream::getCurrentPrefix() const
{
    if (s_prefix.empty())
    {
        // Generate default prefix with thread ID
        std::ostringstream oss;
        oss << "[Thread-" << std::this_thread::get_id() << "] ";
        return oss.str();
    }
    else
    {
        return "[" + s_prefix + "] ";
    }
}