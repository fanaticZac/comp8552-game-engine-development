//
// Created by user on 2025-09-16.
//

#ifndef COMP8552_FINAL_PROJECT_ENTITY_H
#define COMP8552_FINAL_PROJECT_ENTITY_H

#include <array>
#include <bitset>
#include <cstddef>

#include "ComponentType.h"

// create a const that is evaluated at compile time (constexpr)
//32 components fits perfectly in a 32-bit integer
// all about efficiency
constexpr std::size_t MAX_COMPONENTS = 32;

//bit operations are fast, and work well for checking each frame
using ComponentBitSet = std::bitset<MAX_COMPONENTS>;

//fixed size array, where each index is a component.
//the index corresponds to the ComponentTypeID
// We use a fixed number of components , so there is no dynamic allocation of the array
// (efficiency)
using ComponentArray = std::array<void*, MAX_COMPONENTS>;

class Entity {
    bool active = true; // remove enity if false on cleanup
    inline static int nextID = 0;
    int id;
    ComponentArray componentArray {};
    ComponentBitSet componentBitSet{};
public:
    Entity() { id = nextID++; }
    int getID() const { return id; }

    bool isActive() {
        return active;
    }

    void destroy() {
        active = false;
    }

    //templates need to be implemented in header
    //e.g. usage: entity.hasComponent<Position>() -> returns true or false
    template <typename T>
    bool hasComponent() {
        return componentBitSet[getComponentTypeID<T>()];
    }

    //template that can receive multiple arguments
    //entity.addComponent<Position>(3,4)
    //entity.addComponent<Health>(100)
    template <typename T, typename... mArgs>
    T& addComponent(mArgs&&... args) {

        //impliments 'perfet forwarding'
        // means everything is not treated as an lvalue
        //this is more efficient with moves and temporaries
        T* component(new T(std::forward<mArgs>(args)...));

        //add the new component to the array
        componentArray[getComponentTypeID<T>()] = component;
        //update the bitset to true
        componentBitSet[getComponentTypeID<T>()] = true;
        return *component;
    }

    //e.g. entity.getComponent<Health>()
    template <typename T>
    T& getComponent() {
        auto ptr(componentArray[getComponentTypeID<T>()]);
        // returned cast ref so we know the type rather than void pointer
        return *static_cast<T*>(ptr);
    }


    template <typename T>
    void deactivateComponent() {
        componentBitSet[getComponentTypeID<T>()] = false;
    }

};

#endif //COMP8552_FINAL_PROJECT_ENTITY_H