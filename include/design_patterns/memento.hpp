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
 * Stores the current state of an object. Must be inherited by the "saveable" class.
 */
class Memento
{
    public:
        /**
         * @brief Container for storing object state data.
         *
         * Snapshot is a polymorphic container that can store different types
         * of data representing an object's state.
         */
        class Snapshot
        {
            private:
                /** Container for storing different types of data */
                std::map<std::string, std::any> m_data;
                /** Counter for automatic key generation during serialization */
                mutable size_t m_insertCounter;
                /** Counter for automatic key retrieval during deserialization */
                mutable size_t m_extractCounter;
                
            public:
                /**
                 * @brief Constructor
                 */
                Snapshot() : m_insertCounter(0), m_extractCounter(0) {}
                
                /**
                 * @brief Copy constructor 
                 */
                Snapshot(const Snapshot& other) 
                    : m_data(other.m_data), m_insertCounter(other.m_insertCounter), m_extractCounter(0) {}
                
                /**
                 * @brief Assignment operator
                 */
                Snapshot& operator=(const Snapshot& other) {
                    if (this != &other) {
                        m_data = other.m_data;
                        m_insertCounter = other.m_insertCounter;
                        m_extractCounter = 0;
                    }
                    return *this;
                }
                
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
                
                /**
                 * @brief Stream insertion operator for easy data storage.
                 *
                 * @tparam TType Type of the value to store
                 * @param snapshot The snapshot to store data in
                 * @param value The value to store
                 * @return Reference to the snapshot for chaining
                 */
                template<typename TType>
                friend Snapshot& operator<<(Snapshot& snapshot, const TType& value);
                
                /**
                 * @brief Stream extraction operator for easy data retrieval.
                 *
                 * @tparam TType Type of the value to retrieve
                 * @param snapshot The snapshot to extract data from
                 * @param value Reference where the retrieved value will be stored
                 * @return Reference to the snapshot for chaining
                 */
                template<typename TType>
                friend Snapshot& operator>>(Snapshot& snapshot, TType& value);
        };

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

    private:
        /**
         * @brief Virtual method that must be implemented by derived classes.
         *
         * Saves the object's state into the provided Snapshot.
         * To be saved, the inheriting class must implement this method as private.
         *
         * @param snapshot The Snapshot to save state into
         */
        virtual void _saveToSnapshot(Snapshot& snapshot) const = 0;
        
        /**
         * @brief Virtual method that must be implemented by derived classes.
         *
         * Loads the object's state from the provided Snapshot.
         * To be saved, the inheriting class must implement this method as private.
         *
         * @param snapshot The Snapshot to load state from
         */
        virtual void _loadFromSnapshot(Snapshot& snapshot) = 0;
};

// Template implementations for Snapshot operators
template<typename TType>
Memento::Snapshot& operator<<(Memento::Snapshot& snapshot, const TType& value)
{
    snapshot.set(std::to_string(snapshot.m_insertCounter++), value);
    return snapshot;
}

template<typename TType>
Memento::Snapshot& operator>>(Memento::Snapshot& snapshot, TType& value)
{
    value = snapshot.get<TType>(std::to_string(snapshot.m_extractCounter++));
    return snapshot;
}

#endif