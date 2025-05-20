/**
 * @file memento.hpp
 * @brief Implementation of the Memento design pattern.
 */

#ifndef MEMENTO_HPP
# define MEMENTO_HPP

#include <map>
#include <string>
#include <any>
#include <typeindex>

/**
 * @brief Implements the Memento design pattern for object state saving/restoration.
 *
 * This class provides a mechanism to capture and externalize an object's internal state
 * without violating encapsulation, making it possible to restore the object to this
 * state later.
 * 
 * @note The save() and load() methods are declared as protected rather than private
 * to allow derived classes to access them. Derived classes can selectively expose
 * these methods to their clients using the 'using' directive (e.g. 'using Memento::save;').
 * This design gives derived classes control over the visibility of the state
 * management functionality.
 */

class Memento
{
    public:
        /**
         * @brief Container for storing object state data.
         *
         * Snapshot is a type-safe polymorphic container that can store different types
         * of data representing an object's state.
         */
        class Snapshot
        {
            private:
                /** Container for storing different types of data */
                std::map<std::string, std::any> m_data;
                
            public:
                /**
                 * @brief Stores a value of any type with the given key.
                 *
                 * @tparam T Type of the value to store
                 * @param key Identifier for the stored value
                 * @param value The value to store
                 */
                template<typename T>
                void set(const std::string& key, const T& value) {
                    m_data[key] = value;
                }
                
                /**
                 * @brief Retrieves a value by its key.
                 *
                 * @tparam T Expected type of the value
                 * @param key Identifier of the value to retrieve
                 * @return The stored value cast to type T
                 * @throws std::bad_any_cast if the stored type doesn't match T
                 */
                template<typename T>
                T get(const std::string& key) const {
                    return std::any_cast<T>(m_data.at(key));
                }
                
                /**
                 * @brief Checks if a key exists in the snapshot.
                 *
                 * @param key Key to check
                 * @return true if the key exists, false otherwise
                 */
                bool hasKey(const std::string& key) const;
        };

    protected:
        /**
         * @brief Save the current state of the object.
         *
         * @return A Snapshot containing the object's current state
         */
        Snapshot save();
        
        /**
         * @brief Load a previously saved state.
         *
         * @param snapshot The Snapshot containing the state to restore
         */
        void load(const Snapshot& snapshot);
        
        /**
         * @brief Virtual method that must be implemented by derived classes.
         *
         * Saves the object's state into the provided Snapshot.
         *
         * @param snapshot The Snapshot to save state into
         */
        virtual void saveToSnapshot(Snapshot& snapshot) const = 0;
        
        /**
         * @brief Virtual method that must be implemented by derived classes.
         *
         * Loads the object's state from the provided Snapshot.
         *
         * @param snapshot The Snapshot to load state from
         */
        virtual void loadFromSnapshot(const Snapshot& snapshot) = 0;
};

#endif