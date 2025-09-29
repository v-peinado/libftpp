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
        
        Pool(const Pool&) = delete;                 // Rule of Five: delete copy to prevent double-free, delete move for simplicity
        Pool& operator=(const Pool&) = delete;
        Pool(Pool&&) = delete;
        Pool& operator=(Pool&&) = delete;

        /* Inner classes */

        class Object;

        /* Core methods */

        void resize(const size_t &numberOfObjectStored);
        template<typename... TArgs>
        Object acquire(TArgs&&... p_args);

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

template<typename TType>
class Pool<TType>::Object
{
    private:

        TType* m_ptr;        // Managed object pointer
        Pool<TType>* m_pool; // Parent pool for automatic return
        size_t m_index;      // Pool slot index

    public:

        /* Constructors and destructors */
            /* RAII Management */

        Object(TType* ptr, Pool<TType>* pool, size_t index);
        ~Object();  // Returns to pool automatically
        
            /* Rule of Five - no copy, yes move */

        Object(const Object&) = delete;             // Prevent double-free
        Object& operator=(const Object&) = delete;  // Prevent double-free
        Object(Object&&) noexcept;                  // Transfer ownership
        Object& operator=(Object&&) noexcept;       // Transfer ownership

        /* Smart pointer interface */

        TType* operator->() const;  // Access object members
        TType& operator*() const;   // Dereference object
        TType* get() const;         // Get raw pointer (like std::unique_ptr::get)

        /* State checking */
        bool isValid() const;  // Check if holds valid object
};

// ============================================
//           POOL IMPLEMENTATIONS
// ============================================

template<typename TType>
Pool<TType>::~Pool()
{
    for(TType* ptr : m_objects)
        delete ptr;
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
template<typename TType>
void Pool<TType>::resize(const size_t &numberOfObjectStored)
{
    std::vector<TType*> oldObjects = std::move(m_objects);          // Save current state to restore in case of exception
    std::vector<size_t> oldAvailable = std::move(m_available);      // Move leaves vectors empty, transferring ownership to oldObjects/oldAvailable

    try
    {
        m_objects.reserve(numberOfObjectStored);                    // Pre-allocate capacity to avoid multiple reallocations during push_back operations
        m_available.reserve(numberOfObjectStored);

        for (size_t i = 0; i < numberOfObjectStored; i++)              // Create new objects 
        {
            m_objects.push_back(new TType());                       // () ensures proper initialization (zero for PODs [Plain Old Data: primitives/simple structs], constructor for classes)
            m_available.push_back(i);
        }
        for(auto ptr : oldObjects)                                  // Success - clean up old objects
            delete ptr;

    }
    catch(...)
    {
        m_objects = std::move(oldObjects);
        m_available = std::move(oldAvailable);
        throw;                                                      // Propagate exception to caller after cleanup - resize failed but pool state is restored
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
template<typename TType>
template<typename... TArgs>
typename Pool<TType>::Object Pool<TType>::acquire(TArgs&&... p_args)
{
    if (m_available.empty())
        throw std::runtime_error("Pool is empty. Resize the pool or release objects.");
    
    size_t index = m_available.back();
    m_available.pop_back();
    TType* ptr = m_objects[index];

    try
    {
        ptr->~TType();
        new (ptr) TType(std::forward<TArgs>(p_args)...);
    }
    catch(...)
    {
        // Return the index to the pool if construction fails
        m_available.push_back(index);
                
        // Re-throw the exception
        throw;        
    }
    return Object(ptr, this, index);
}

/**
 * @brief Returns an object to the pool by its index.
 *
 * This method is called automatically by Object's destructor.
 * Should not be called directly by user code.
 *
 * @param index Index of the object to return to the pool
 */
template<typename TType>
void Pool<TType>::returnToPool(size_t index)
{
    // Add the index back to available objects
    m_available.push_back(index);
}

/**
 * @brief Get the total number of objects in the pool.
 *
 * @return Total number of objects (both available and in use)
 */
template<typename TType>
size_t Pool<TType>::size() const
{
    return m_objects.size();
}

/**
 * @brief Get the number of available objects in the pool.
 *
 * @return Number of objects currently available for acquisition
 */
template<typename TType>
size_t Pool<TType>::available() const
{
    return m_available.size();
}

/**
 * @brief Get the number of objects currently in use.
 *
 * @return Number of objects that have been acquired but not yet returned
 */
template<typename TType>
size_t Pool<TType>::inUse() const
{
    return m_objects.size() - m_available.size();
}

/**
 * @brief Check if the pool is empty (no objects available).
 *
 * @return true if no objects are available for acquisition
 */
template<typename TType>
bool Pool<TType>::isEmpty() const
{
    return m_available.empty();
}

/**
 * @brief Check if all objects are available (none in use).
 *
 * @return true if all objects are available for acquisition
 */
template<typename TType>
bool Pool<TType>::isFull() const
{
    return m_available.size() == m_objects.size();
}

// ============================================
//        POOL::OBJECT IMPLEMENTATIONS
// ============================================

/**
 * @brief Constructor of Object wrapper
 */
template<typename TType>
Pool<TType>::Object::Object(TType* ptr, Pool<TType>* pool, size_t index) : m_ptr(ptr), m_pool(pool), m_index(index)
{
}

/**
 * @brief Destructor - returns object to pool
 */
template<typename TType>
Pool<TType>::Object::~Object()
{
    if (m_ptr && m_pool) {
        m_pool->returnToPool(m_index);
    }
}
/**
 * @brief Move constructor
 * 
 * @param other object to move from
 */
template<typename TType>
Pool<TType>::Object::Object(Object&& other) noexcept : m_ptr(other.m_ptr), m_pool(other.m_pool), m_index(other.m_index)
{
    other.m_ptr = nullptr;
    other.m_pool = nullptr;
}

/**
 * @brief Move assignment operator
 * 
 * @param other object to move from
 * @return *this object
 */
template<typename TType>
typename Pool<TType>::Object& Pool<TType>::Object::operator=(Object&& other) noexcept
{
    if (this != &other)
    {
        // Return current object to pool if we have one, devolvemos el antiguo al
        if (m_ptr && m_pool)
            m_pool->returnToPool(m_index);
        
        // Take ownership of the other object
        m_ptr = other.m_ptr;
        m_pool = other.m_pool;
        m_index = other.m_index;
        
        // Clear the other object
        other.m_ptr = nullptr;
        other.m_pool = nullptr;
    }
    return *this;
}

/**
 * @brief Arrow operator for accessing the managed object.
 * 
 * @return Pointer to the managed object
 */
template<typename TType>
TType* Pool<TType>::Object::operator->() const
{
    return m_ptr;
}

/**
 * @brief Dereference operator for accessing the managed object.
 * 
 * @return Reference to the managed object
 */
template<typename TType>
TType& Pool<TType>::Object::operator*() const
{
    return *m_ptr;
}

/**
 * @brief Get the raw pointer to the managed object.
 * 
 * @return Pointer to the managed object
 */
template<typename TType>
TType* Pool<TType>::Object::get() const
{
    return m_ptr;
}

/**
 * @brief Check if the object wrapper contains a valid object.
 * 
 * @return true if the wrapper contains a valid object, false otherwise
 */
template<typename TType>
bool Pool<TType>::Object::isValid() const
{
    return m_ptr != nullptr && m_pool != nullptr;
}
#endif