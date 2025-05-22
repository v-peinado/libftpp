/**
 * @file singleton.hpp
 * @brief Implementation of the Singleton design pattern.
 */

#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <stdexcept>
#include <memory>

/**
 * @brief Implements the Singleton design pattern.
 *
 * Ensures that a class has only one instance and provides a global point
 * of access to it.
 *
 * @note Exception handling: instance() throws if called before instantiate().
 * instantiate() throws if called when an instance already exists.
 *
 * @tparam TType The type of the singleton instance
 */
template<typename TType>
class Singleton
{
private:
    /** Pointer to the singleton instance */
    static std::unique_ptr<TType> s_instance;

public:
    /**
     * @brief Get the singleton instance.
     *
     * @return Pointer to the singleton instance
     * @throws std::runtime_error if the instance hasn't been created yet
     */
    static TType* instance()
    {
        if (!s_instance)
        {
            throw std::runtime_error("Singleton instance not created. Call instantiate() first.");
        }
        return s_instance.get();
    }
    
    /**
     * @brief Create the singleton instance with the given arguments.
     *
     * @tparam TArgs Types of the constructor arguments
     * @param p_args Arguments to pass to the constructor
     * @throws std::runtime_error if the instance has already been created
     */
    template<typename... TArgs>
    static void instantiate(TArgs&&... p_args)
    {
        if (s_instance)
        {
            throw std::runtime_error("Singleton instance already created.");
        }
        // Use new instead of make_unique to access private constructors
        s_instance.reset(new TType(std::forward<TArgs>(p_args)...));
    }
};

// Initialize the static member
template<typename TType>
std::unique_ptr<TType> Singleton<TType>::s_instance = nullptr;

#endif