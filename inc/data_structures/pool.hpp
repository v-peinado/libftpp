/**
 * @file pool.hpp
 * @brief Object pool implementation for efficient reuse
 */

#ifndef POOL_HPP
# define POOL_HPP

#include <vector>
#include <stdexcept>

/**
 * @brief Generic object pool that manages reusable resources
 * 
 * Provides an efficient way to reuse objects without overhead and
 * frequent allocation. Objects preallocated and return to the pool after use.
 * 
 * @note Exception handling: This class follows strict exception propagation.
 * All exceptions are propagated to the caller after ensuring the pool remains
 * in a consistent state.
 * 
 * @tparam TType: Type of objects managed by the pool
 */

template<typename TType>
class Pool
{
    private:

        std::vector<TType*> m_objects;                  // Vector of pre-allocated objects
        std::vector<size_t> m_available;                // Index of available objects

        /* Helper methods */

        void returnToPool(size_t index);

        /* Friends class */

        friend class Object;       // Zero-overhead access to internals vs public getters that break encapsulation
        
    public:

        /* Constructors and destructors */

        Pool() = default;
        ~Pool();

        /* Inner classes */

        class Object;

        /* Core methods */

        void resize(const size_t &numberOfObjectStored);
        template<typename... TArgs>
        Object acquire(Targs&&... p_args);

       /* Utility methods */ 

        size_t size() const;
        size_t available() const;
        size_t inUse() const;
        bool isEmpty() const;
        bool isFull() const;
};

/**
 * @brief RAII wrapper that automatically returns objects to the pool.
 *
 * Acts as a smart pointer that:
 * - Provides access to the pooled object via ->
 * - Automatically returns the object when destroyed
 * - Ensures safe object recycling even if exceptions occur
 * - Prevents use-after-return and double-return errors
 */

class Object
{
    private:

        TType* m_ptr;                     // Pointer to the acquired object
        Pool<TType>* m_pool;              // Pointer to the pool that owns this object
        size_t m_index;                   // Index of this object in the pool

    public:

        /* Constructors and destructors */

        Object(TType* ptr, Pool<TType>* pool, size_t index);
        ~Object();
        Object(const Object& other) = delete;                   // Delete copy operations - prevents compiler-generated defaults,
        Object& operator=(const Object& other) = delete;        // modern alternative to C++98 private declarations
        Object(Object&& other) noexcept;                        // No const ref
        Object& operator=(Object&& other) noexcept;

        /* Operators for members pointers */

        TType* operator->();
        TType* operator*();

        /* Getters */

        TType* getPointer();

        /* Methods */

        bool isValid() const;
};

// ============================================
//           POOL IMPLEMENTATIONS
// ============================================

Pool::~Pool()
{
    for(TType* ptr : m_objects)
        delete ptr;
}

void Pool::resize(const size_t &numberOfObjectStored)
{
    std::vector<TType*> oldObjects = std::move(m_objects);          // Save current state to restore in case of exception
    std::vector<size_t> oldAvailable = std::move(m_available);      // Move leaves vectors empty, transferring ownership to oldObjects/oldAvailable

    try
    {
        m_objects.reserve(numberOfObjectStored);                    // Pre-allocate capacity to avoid multiple reallocations during push_back operations
        m_available.reserve(numberOfObjectStored);

        for (int i = 0; i < numberOfObjectStored; i++)              // Create new objects 
        {
            m_objects.push_back(new TType());
            m_available.push_back(i);
        }
        for(auto ptr : oldObjects)                                  // Success - clean up old objects
            delete ptr;

    }
    catch(...)
    {
        m_objects = std::move(oldObjects);
        m_available = std::move(oldAvailable);
        throw;
    }
    
}

// ============================================
//        POOL::OBJECT IMPLEMENTATIONS
// ============================================

#endif