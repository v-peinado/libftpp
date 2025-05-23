/**
 * @file libftpp.hpp
 * @brief Main public header file for the libftpp library.
 * 
 * This header acts as the public API for the library, providing access to all 
 * components implemented in libftpp. Users only need to include this single
 * header file to access the entire functionality of the library.
 */

#ifndef LIBFTPP_HPP
# define LIBFTPP_HPP

/**
 * @defgroup data_structures Data Structures
 * @brief Reusable data structure templates
 */

#include "include/data_structures/data_buffer.hpp"
#include "include/data_structures/pool.hpp"

/**
 * @defgroup design_patterns Design Patterns
 * @brief Common design pattern implementations
 */

#include "include/design_patterns/memento.hpp"
#include "include/design_patterns/observer.hpp"
#include "include/design_patterns/singleton.hpp"
#include "include/design_patterns/state_machine.hpp"

/**
 * @defgroup iostream IOStream
 * @brief Thread-safe I/O stream utilities
 */

#include "include/iostream/thread_safe_iostream.hpp"

#endif