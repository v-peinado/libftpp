/**
 * @file memento.hpp
 * @brief Implementation of the Memento design pattern.
 */

#ifndef MEMENTO_HPP
# define MEMENTO_HPP

#include "data_structures/data_buffer.hpp"

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
         * Snapshot is a wrapper around DataBuffer that provides the polymorphic
         * container functionality for easy state restoration.
         */
        class Snapshot
        {
            private:
                /** Internal data buffer for binary serialization */
                DataBuffer m_buffer;
                
            public:
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
    snapshot.m_buffer << value;
    return snapshot;
}

template<typename TType>
Memento::Snapshot& operator>>(Memento::Snapshot& snapshot, TType& value)
{
    snapshot.m_buffer >> value;
    return snapshot;
}

#endif