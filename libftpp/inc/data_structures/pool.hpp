/**
 * @file pool.hpp
 * @brief Object pool implementation for efficient reuse
 */

#ifndef POOL_HPP
#define POOL_HPP

#include <vector>
#include <memory>
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
 * @tparam TType Type of objects managed by the pool
 */
template<typename TType>
class Pool
{
private:
    /* Vector of pre-allocated objects - RAII with unique_ptr */
    std::vector<std::unique_ptr<TType>> m_objects;
    
    /* Index of available objects */
    std::vector<size_t> m_available;
    
    /* Helper methods */
    void returnToPool(size_t index);
    
    /* Friends class */
    friend class Object;  // Zero-overhead access to internals
    
public:
    /* Constructors and destructors */
    
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
     * Automatic cleanup via unique_ptr RAII - no manual delete needed!
     */
    ~Pool() = default;  // unique_ptr handles cleanup automatically
    
    /* Rule of Five: delete copy/move to prevent issues */
    Pool(const Pool&) = delete;
    Pool& operator=(const Pool&) = delete;
    Pool(Pool&&) = delete;
    Pool& operator=(Pool&&) = delete;
    
    /* Inner classes */
    class Object;
    
    /* Core methods */
    
    /**
     * @brief Sets the pool capacity.
     *
     * Allocates memory for the specified number of objects and initializes them.
     * If the pool was previously initialized, all existing objects are destroyed.
     *
     * @param numberOfObjectStored Number of objects to pre-allocate
     * @throws std::bad_alloc if memory allocation fails
     */
    void resize(const size_t& numberOfObjectStored);
    
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
    Object acquire(TArgs&&... p_args);
    
    /* Utility methods */
    
    /**
     * @brief Get the total number of objects in the pool.
     *
     * @return Total number of objects (both available and in use)
     */
    size_t size() const;
    
    /**
     * @brief Get the number of available objects in the pool.
     *
     * @return Number of objects currently available for acquisition
     */
    size_t available() const;
    
    /**
     * @brief Get the number of objects currently in use.
     *
     * @return Number of objects that have been acquired but not yet returned
     */
    size_t inUse() const;
    
    /**
     * @brief Check if the pool is empty (no objects available).
     *
     * @return true if no objects are available for acquisition
     */
    bool isEmpty() const;
    
    /**
     * @brief Check if all objects are available (none in use).
     *
     * @return true if all objects are available for acquisition
     */
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
    TType* m_ptr;         // Managed object pointer
    Pool<TType>* m_pool;  // Parent pool for automatic return
    size_t m_index;       // Pool slot index
    
public:
    /* Constructors and destructors */
    /* RAII Management */
    
    /**
     * @brief Constructs a new Object wrapper
     * 
     * @param ptr Pointer to the acquired object
     * @param pool Pointer to the pool that owns the object
     * @param index Index of the object in the pool
     */
    Object(TType* ptr, Pool<TType>* pool, size_t index);
    
    /**
     * @brief Destructor - returns object to pool automatically
     */
    ~Object();
    
    /* Rule of Five - no copy, yes move */
    
    Object(const Object&) = delete;             // Prevent double-free
    Object& operator=(const Object&) = delete;  // Prevent double-free
    
    /**
     * @brief Move constructor - Transfer ownership
     * 
     * @param other object to move from
     */
    Object(Object&& other) noexcept;
    
    /**
     * @brief Move assignment operator - Transfer ownership
     * 
     * @param other object to move from
     * @return *this object
     */
    Object& operator=(Object&& other) noexcept;
    
    /* Smart pointer interface */
    
    /**
     * @brief Arrow operator for accessing the managed object.
     * 
     * @return Pointer to the managed object
     */
    TType* operator->() const;
    
    /**
     * @brief Dereference operator for accessing the managed object.
     * 
     * @return Reference to the managed object
     */
    TType& operator*() const;
    
    /**
     * @brief Get raw pointer (like std::unique_ptr::get)
     * 
     * @return Pointer to the managed object
     */
    TType* get() const;
    
    /* State checking */
    
    /**
     * @brief Check if the object wrapper contains a valid object.
     * 
     * @return true if the wrapper contains a valid object, false otherwise
     */
    bool isValid() const;
};

// ============================================================================
//                          POOL IMPLEMENTATIONS
// ============================================================================

// Note: Destructor is now = default in class declaration
// unique_ptr handles all cleanup automatically - no manual delete needed!

template<typename TType>
void Pool<TType>::resize(const size_t& numberOfObjectStored)
{
    // Save current state to restore in case of exception
    std::vector<std::unique_ptr<TType>> oldObjects = std::move(m_objects);
    std::vector<size_t> oldAvailable = std::move(m_available);
    
    try
    {
        // Pre-allocate capacity to avoid multiple reallocations
        m_objects.reserve(numberOfObjectStored);
        m_available.reserve(numberOfObjectStored);
        
        // Create new objects using RAII
        for (size_t i = 0; i < numberOfObjectStored; ++i)
        {
            // make_unique provides exception safety and RAII
            m_objects.push_back(std::make_unique<TType>());
            m_available.push_back(i);
        }
        
        // Success - old objects will be automatically destroyed
        // by unique_ptr when oldObjects goes out of scope
        // No manual delete needed!
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

template<typename TType>
template<typename... TArgs>
typename Pool<TType>::Object Pool<TType>::acquire(TArgs&&... p_args)
{
    if (m_available.empty())
    {
        throw std::runtime_error("Pool is empty. Resize the pool or release objects.");
    }
    
    // Get an available index
    size_t index = m_available.back();
    m_available.pop_back();
    
    // Get the raw pointer from unique_ptr (unique_ptr still owns it)
    TType* ptr = m_objects[index].get();
    
    try
    {
        // Reinitialize the object in-place
        ptr->~TType();  // Explicit destructor call
        
        // Placement new with perfect forwarding
        new (ptr) TType(std::forward<TArgs>(p_args)...);
    }
    catch (...)
    {
        // Return the index to the pool if construction fails
        m_available.push_back(index);
        
        // Re-throw the exception
        throw;
    }
    
    // Return Object wrapper - RAII will handle return to pool
    return Object(ptr, this, index);
}

template<typename TType>
void Pool<TType>::returnToPool(size_t index)
{
    // Add the index back to available objects
    m_available.push_back(index);
}

template<typename TType>
size_t Pool<TType>::size() const
{
    return m_objects.size();
}

template<typename TType>
size_t Pool<TType>::available() const
{
    return m_available.size();
}

template<typename TType>
size_t Pool<TType>::inUse() const
{
    return m_objects.size() - m_available.size();
}

template<typename TType>
bool Pool<TType>::isEmpty() const
{
    return m_available.empty();
}

template<typename TType>
bool Pool<TType>::isFull() const
{
    return m_available.size() == m_objects.size();
}

// ============================================================================
//                      POOL::OBJECT IMPLEMENTATIONS
// ============================================================================

template<typename TType>
Pool<TType>::Object::Object(TType* ptr, Pool<TType>* pool, size_t index)
    : m_ptr(ptr), m_pool(pool), m_index(index)
{
}

template<typename TType>
Pool<TType>::Object::~Object()
{
    if (m_ptr && m_pool)
    {
        m_pool->returnToPool(m_index);
    }
}

template<typename TType>
Pool<TType>::Object::Object(Object&& other) noexcept
    : m_ptr(other.m_ptr), m_pool(other.m_pool), m_index(other.m_index)
{
    // Clear the source object
    other.m_ptr = nullptr;
    other.m_pool = nullptr;
    // Note: no need to clear m_index as ptr/pool being null is enough
}

template<typename TType>
typename Pool<TType>::Object& Pool<TType>::Object::operator=(Object&& other) noexcept
{
    if (this != &other)
    {
        // Return current object to pool if we have one
        if (m_ptr && m_pool)
        {
            m_pool->returnToPool(m_index);
        }
        
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

template<typename TType>
TType* Pool<TType>::Object::operator->() const
{
    return m_ptr;
}

template<typename TType>
TType& Pool<TType>::Object::operator*() const
{
    return *m_ptr;
}

template<typename TType>
TType* Pool<TType>::Object::get() const
{
    return m_ptr;
}

template<typename TType>
bool Pool<TType>::Object::isValid() const
{
    return m_ptr != nullptr && m_pool != nullptr;
}

#endif