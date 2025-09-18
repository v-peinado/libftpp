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

        /* Vector of pre-allocated objects */

        std::vector<TType*> m_objects;

        /* Index of available objects */

        std::vector<size_t> m_available;

        /* Helper methods */

        void returnToPool(size_t index);

        /* Friends class */

        friend class PoolObject;       // Zero-overhead access to internals vs public getters that break encapsulation
        
    public:

        /* Constructors and destructors */

        Pool() = default;
        ~Pool();

        /* Inner classes */

        class PoolObject;

        /* Core methods */

        void resize(const size_t &numberOfObjectStored);
        template<typename... TArgs>
        PoolObject acquire(Targs&&... p_args);

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

class PoolObject
{
    private:

        /* Pointer to the acquired object */

        TType* m_ptr;

        /* Pointer to the pool that owns this object */

        Pool<TType>* m_pool;

        /* Index of this object in the pool */

        size_t m_index;

    public:

        /* Constructors and destructors */

        PoolObject(TType* ptr, Pool<TType>* pool, size_t index);
        ~PoolObject();
        PoolObject(const PoolObject& other) = delete;                   // Delete copy operations - prevents compiler-generated defaults,
        PoolObject& operator=(const PoolObject& other) = delete;        // modern alternative to C++98 private declarations
        PoolObject(PoolObject&& other) noexcept;
        PoolObject& operator=(PoolObject&& other) noexcept;
}


#endif