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
 * @tparam TType: Type of objects managed by the pool
 */

template<typename TType>
class Pool {
    private:
        /* Vector of pre-allocated objects */
        std::vector<TType *> m_objects;

        /* Index of available objects */
        std::vector<size_t> m_available;

    public:
           

}

#endif