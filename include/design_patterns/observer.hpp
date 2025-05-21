/**
 * @file observer.hpp
 * @brief Implementation of the Observer design pattern.
 */

#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <functional>
#include <map>
#include <vector>
#include <stdexcept>

/**
 * @brief Implements the Observer design pattern for event notification.
 *
 * This class allows objects to subscribe to specific events and be notified
 * when those events are triggered.
 *
 * @note Exception handling: If a callback throws an exception during notify(),
 * the exception will be propagated to the caller and remaining callbacks for
 * that event will not be executed.
 *
 * @tparam TEvent Type used to identify different events
 */
template<typename TEvent>
class Observer
{
private:
    /** Map of event types to their registered callback functions */
    std::map<TEvent, std::vector<std::function<void()>>> m_subscribers;

public:
    /**
     * @brief Subscribe a callback function to a specific event.
     *
     * When the event is triggered via notify(), all subscribed callbacks
     * will be executed.
     *
     * @param event The event to subscribe to
     * @param lambda The callback function to execute when the event occurs
     */
    void subscribe(const TEvent& event, const std::function<void()>& lambda)
    {
        m_subscribers[event].push_back(lambda);
    }
    
    /**
     * @brief Notify all subscribers of a specific event.
     *
     * Executes all callback functions that have been subscribed to the
     * specified event.
     *
     * @param event The event to notify subscribers about
     * @throws Any exception thrown by callback functions
     */
    void notify(const TEvent& event)
    {
        if (m_subscribers.find(event) != m_subscribers.end())
        {
            // Make a copy of callbacks to prevent issues if callbacks modify the subscriber list
            auto callbacksCopy = m_subscribers[event];
            
            for (const auto& callback : callbacksCopy)
            {
                // Execute callback - any exceptions will propagate to caller
                callback();
            }
        }
    }
};

#endif