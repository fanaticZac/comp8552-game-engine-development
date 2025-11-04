//
// Created by user on 2025-09-16.
//

#ifndef COMP8552_FINAL_PROJECT_COMPONENTTYPE_H
#define COMP8552_FINAL_PROJECT_COMPONENTTYPE_H

#include <cstddef>

//creating a type alias for std:size+t
//size_t is designed to represent sizes and indices because it's unsigned, and it's very large (long, long).
using ComponentTypeID = std::size_t;

// inline function because when you have a free function defined in a header, each .cpp file that included this header (ComponentType.h)
// would get its own definition causing a linker error.
//global counter generator, returns a unique number everytime we call it
// making id static, means it will keep its value through multiple calls of this function.
inline ComponentTypeID getComponentTypeID() {
    static ComponentTypeID id = 0;
    return id++;
}

//can overload this function because they have different signatures
//template allows you to use generic types
//template is inline by default - we don't have to mark it so
//getComponentTypeID<Positions>() -> would always return 0
//getComponentTypeID<Health>() -> always return 1
// - Static local variable is created and initialized once.
template <typename T>
ComponentTypeID getComponentTypeID() {
    static ComponentTypeID typeID = getComponentTypeID();
    return typeID;
}

//// ^^ Combined - Good pattern to use


#endif //COMP8552_FINAL_PROJECT_COMPONENTTYPE_H