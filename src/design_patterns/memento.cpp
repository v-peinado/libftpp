/**
 * @file memento.cpp
 * @brief Implementation of the Memento design pattern.
 */

#include "design_patterns/memento.hpp"

// Implementation of Snapshot::hasKey
bool Memento::Snapshot::hasKey(const std::string& key) const
{
    return m_data.find(key) != m_data.end();
}

/**
 * @brief Save the current state of the object.
 *
 * Creates a new Snapshot and fills it with the object's current state
 * by calling the derived class's saveToSnapshot method.
 *
 * @return A Snapshot containing the object's current state
 * @throws Any exception thrown by derived class's saveToSnapshot()
 */
Memento::Snapshot Memento::save()
{
    Snapshot snapshot;
    
    // Call derived class's implementation
    // Any exceptions will propagate to the caller
    saveToSnapshot(snapshot);
    
    return snapshot;
}

/**
 * @brief Load a previously saved state.
 *
 * Restores the object's state from the provided Snapshot
 * by calling the derived class's loadFromSnapshot method.
 *
 * @param snapshot The Snapshot containing the state to restore
 * @throws Any exception thrown by derived class's loadFromSnapshot()
 */
void Memento::load(const Snapshot& snapshot)
{
    // Call derived class's implementation
    // Any exceptions will propagate to the caller
    loadFromSnapshot(snapshot);
}