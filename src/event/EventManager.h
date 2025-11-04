//
// Created by user on 2025-10-06.
//

#ifndef COMP8552_FINAL_PROJECT_EVENTMANAGER_H
#define COMP8552_FINAL_PROJECT_EVENTMANAGER_H

#include <functional>

#include "BaseEvent.h"

//Observer Pattern
class EventManager {
public:
    // For easy writing - but example of handler anyway
    using Handler = std::function<void(const BaseEvent&)>;

    // template<typename EventType>
    // void emit(const EventType& event) {
    //     // retreive the list of subscribers
    //     auto& listeners = getListeners<EventType>();
    //     // loop all subscribers
    //     for (auto& listener : listeners) {
    //         listener(event);
    //     }
    // }

    void emit(const BaseEvent& event) const {
        for (const auto& listener: listeners) {
            listener(event);
        }
    }
    // template<typename EventType>
    // void subscribe(std::function<void(const EventType&)> callback) {
    //     // pass in the callback/callable wrapper/subscription to the list of subscriptions
    //     getListeners<EventType>().push_back(callback);
    // }

    void subscribe(const Handler& callback) {
        listeners.emplace_back(callback);
    }


private:
    // Each event has it's own vector of listeners without you having to manage it explicitly
    // This is done using static local variable.
    // std::function<void(const EventType&) - this is the callable wrapper that can hold anything that can be called like a function
    // -- (lambda, free function, outdoor, etc..)
    // template<typename EventType>
    //  std::vector<std::function<void(const EventType&)>>& getListeners() {
    //      static std::vector<std::function<void(const EventType&)>> listeners;
    //     return listeners;
    // }
    std::vector<Handler> listeners;



};


#endif //COMP8552_FINAL_PROJECT_EVENTMANAGER_H