/**
 * @file state_machine.hpp
 * @brief Implementation of the State design pattern using a state machine.
 */

#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include <functional>
#include <map>
#include <stdexcept>
#include <string>

/**
 * @brief Implements a generic state machine.
 *
 * This class manages states and transitions between them, allowing
 * custom actions to be executed when entering states or during transitions.
 *
 * @note Exception handling: All methods throw std::runtime_error for invalid
 * operations (e.g., transitions to non-existent states). If user-provided
 * callbacks throw exceptions, they are propagated to the caller.
 *
 * @tparam TState Type used to identify different states
 */
template<typename TState>
class StateMachine
{
private:
    /** Current state of the machine */
    TState m_currentState;
    
    /** Flag indicating if the machine has been initialized with a state */
    bool m_hasState;
    
    /** Map of valid states */
    std::map<TState, bool> m_states;
    
    /** Map of actions to execute for each state */
    std::map<TState, std::function<void()>> m_actions;
    
    /** Map of transitions between states and their associated actions */
    std::map<std::pair<TState, TState>, std::function<void()>> m_transitions;

public:
    /**
     * @brief Default constructor.
     *
     * Initializes an empty state machine with no current state.
     */
    StateMachine() : m_hasState(false) {}
    
    /**
     * @brief Add a valid state to the state machine.
     *
     * @param state The state to add
     */
    void addState(const TState& state)
    {
        m_states[state] = true;
        
        // If this is the first state, make it the current state
        if (!m_hasState)
        {
            m_currentState = state;
            m_hasState = true;
        }
    }
    
    /**
     * @brief Add a transition between two states with an associated action.
     *
     * @param startState The starting state of the transition
     * @param finalState The ending state of the transition
     * @param lambda The action to execute during the transition
     * @throws std::runtime_error if either state is not valid
     */
    void addTransition(const TState& startState, const TState& finalState, 
                       const std::function<void()>& lambda)
    {
        // Verify both states exist
        if (m_states.find(startState) == m_states.end() || 
            m_states.find(finalState) == m_states.end())
        {
            throw std::runtime_error("Cannot add transition: one or both states do not exist");
        }
        
        // Add the transition
        m_transitions[std::make_pair(startState, finalState)] = lambda;
    }
    
    /**
     * @brief Add an action to execute when the machine is in a specific state.
     *
     * @param state The state to associate the action with
     * @param lambda The action to execute
     * @throws std::runtime_error if the state is not valid
     */
    void addAction(const TState& state, const std::function<void()>& lambda)
    {
        // Verify the state exists
        if (m_states.find(state) == m_states.end())
        {
            throw std::runtime_error("Cannot add action: state does not exist");
        }
        
        // Add the action
        m_actions[state] = lambda;
    }
    
    /**
     * @brief Transition to a new state.
     *
     * Executes the transition action if one is defined.
     *
     * @param state The state to transition to
     * @throws std::runtime_error if the target state is not valid or if no
     *         transition is defined from the current state to the target state
     * @throws Any exception thrown by the transition callback
     */
    void transitionTo(const TState& state)
    {
        // Verify the state exists
        if (m_states.find(state) == m_states.end())
        {
            throw std::runtime_error("Cannot transition: target state does not exist");
        }
        
        // Check if the transition exists
        auto transitionKey = std::make_pair(m_currentState, state);
        if (m_transitions.find(transitionKey) == m_transitions.end())
        {
            throw std::runtime_error("No transition defined between current and target state");
        }
        
        TState oldState = m_currentState;
        
        try
        {
            // Execute the transition action
            m_transitions[transitionKey]();
            
            // Update current state if transition succeeded
            m_currentState = state;
        }
        catch (...)
        {
            // Keep current state unchanged if transition fails
            m_currentState = oldState;
            throw;
        }
    }
    
    /**
     * @brief Execute the action associated with the current state.
     *
     * @throws std::runtime_error if no action is defined for the current state
     * @throws Any exception thrown by the action callback
     */
    void update()
    {
        // Check if an action exists for the current state
        if (m_actions.find(m_currentState) == m_actions.end())
        {
            throw std::runtime_error("No action defined for current state");
        }
        
        // Execute the action
        m_actions[m_currentState]();
    }
};

#endif