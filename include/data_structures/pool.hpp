/**
 * @file pool.hpp
 * @brief Object pool implementation for efficient reuse
 */

#ifndef POOL_HPP
# define POLL_HPP

#include <vector>
#include <functional>
#include <stdexcept>


/**
 * @brief Generic object pool that manages reusable resources
 * 
 * Provides an efficient way to reuse objects whithout overhead and
 * frequent allocation. Objects preallocated an return to the pool after use.
 * 
 * @note Exception handling: This class follows strict exception propagation.
 * All exceptions are propagated to the caller after ensuring the pool remains
 * in a consistent state.
 * 
 * @tparam TType: Type of objects managed by the pool
 */

template<typename TType>
class Pool {
    private:
        /* Vector of pre-allocated objects */
        std::vector<TType*> m_objects;

        /* Index of available objects */
        std::vector<size_t> m_available;

    public:
        /**
         * @brief RAII wrapper that automatically returns objects to the pool.
         *
         * Acts as a smart pointer that:
         * - Provides access to the pooled object via ->
         * - Automatically returns the object when destroyed
         * - Ensures safe object recycling even if exceptions occur
         * - Prevents use-after-return and double-return errors
         */
        class Object {
            private:

                /** Pointer to the acquired object */
                TType* m_ptr;
                
                /** Function that returns the object to the pool */
                std::function<void(TType*)> m_releaseFunc;

            public:

                /** 
                 * @brief Constructs a new Object wrapper
                 * 
                 * @param prt Pointer to the adquiired object
                 * @param releaseFunc Function to call when releasing the object
                 */
                Object(TType* ptr, std::function<void(TType*)> releaseFunc) : m_ptr(ptr), m_releaseFunc(releaseFunc) {}
               
                /**
                 * @brief Destroy the wrapper and return the object to the pool
                 */
                ~Object() {
                    if(m_ptr)
                        m_releaseFunc(m_ptr);
                }

                // Non-copyable
                Object(const Object& other) = delete;
                Object& operator=(const Object& other) = delete;
                
                /**
                 * @brief Move constructor
                 * 
                 * @param other object to move from
                 */
                Object(Object&& other) noexcept : m_ptr(other.m_ptr), m_releaseFunc(std::move(other.m_releaseFunc)) {
                    other.m_ptr = nullptr;
                }

                /**
                 * @brief Move assignment operator
                 * 
                 * @param other object to move from
                 * @return *this object
                 */
                Object& operator=(Object&& other) noexcept {
                    if (this != &other) {
                        if (m_ptr) {
                            m_releaseFunc(m_ptr);
                        }
                        m_ptr = other.m_ptr;
                        m_releaseFunc = std::move(other.m_releaseFunc);
                        other.m_ptr = nullptr;
                    }
                    return *this;
                }

                /**
                 * @brief Arrow operator for accessing the managed object.
                 * 
                 * obj.getPtr()->method(); Equivalent but more verbose alternative
                 *
                 * @return Pointer to the managed object
                 */
                TType* operator->() {
                    return m_ptr;
                }
        };

        /**
         * @brief Default constructor.
         *
         * Creates an empty pool with no pre-allocated objects.
         * 
         * @warning You must call resize() before using acquire(),
         * otherwise acquire() will throw std::runtime_error.
         */
        Pool() = default;

        /**
         * @brief Destructor.
         *
         * Releases all allocated memory.
         */
        ~Pool() {
            for (TType* ptr : m_objects) {
                delete ptr;
            }
        }

        /**
         * @brief Sets the pool capacity.
         *
         * Allocates memory for the specified number of objects and initializes them.
         * If the pool was previously initialized, all existing objects are destroyed.
         *
         * @param numberOfObjectStored Number of objects to pre-allocate
         * @throws std::bad_alloc if memory allocation fails
         */
        void resize(const size_t& numberOfObjectStored)
        {
            // Save current state to restore in case of exception
            std::vector<TType*> oldObjects = std::move(m_objects);
            std::vector<size_t> oldAvailable = std::move(m_available);
            
            try
            {
                // Pre-allocate space
                m_objects.reserve(numberOfObjectStored);
                m_available.reserve(numberOfObjectStored);
                
                // Create new objects
                for (size_t i = 0; i < numberOfObjectStored; ++i)
                {
                    m_objects.push_back(new TType());
                    m_available.push_back(i);
                }
                
                // Success - clean up old objects
                for (auto ptr : oldObjects)
                {
                    delete ptr;
                }
            }
            catch (...)
            {
                // Restore previous state if any operation fails
                m_objects = std::move(oldObjects);
                m_available = std::move(oldAvailable);
                
                // Re-throw the exception
                throw;
            }
        }

        /**
         * @brief Acquires an object from the pool.
         *
         * Gets an available object, reinitializes it with the provided parameters,
         * and returns it wrapped in an Object that will automatically return it to
         * the pool when destroyed.
         *
         * @tparam TArgs Types of constructor arguments
         * @param p_args Arguments to forward to the object's constructor
         * @return Object wrapper containing the acquired object
         * @throws std::runtime_error if the pool is empty
         * @throws Any exception thrown by TType's constructor
         */
        template<typename... TArgs>
        Object acquire(TArgs&&... p_args)
        {
            if (m_available.empty())
            {
                throw std::runtime_error("Pool is empty. Resize the pool or release objects.");
            }

            // Get an available object
            size_t index = m_available.back();
            m_available.pop_back();
            TType* ptr = m_objects[index];
            
            try
            {
                // Reinitialize the object in-place
                ptr->~TType();
                new (ptr) TType(std::forward<TArgs>(p_args)...);
            }
            catch (...)
            {
                // Return the index to the pool if construction fails
                m_available.push_back(index);
                
                // Re-throw the exception
                throw;
            }

            // Create release function
            auto releaseFunc = [this, index](TType* p)
            {
                p->~TType();
                this->m_available.push_back(index);
            };

            return Object(ptr, releaseFunc);
        }
};

#endif