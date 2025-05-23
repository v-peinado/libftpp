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
#include "include/data_structures.hpp"

/**
 * @defgroup design_patterns Design Patterns
 * @brief Common design pattern implementations
 */
#include "include/design_patterns.hpp"

/**
 * @defgroup iostream IOStream
 * @brief Thread-safe I/O stream utilities
 */
#include "include/iostream.hpp"

#endif