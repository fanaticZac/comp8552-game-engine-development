cmake_minimum_required(VERSION 4.0)
project(comp8552_Final_Project)

# MY ADDED CONFIG
# Linux specific using instructions from:
# https://wiki.libsdl.org/SDL3/README-cmake
# Find SDL3 using CMake's package system
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED CONFIG)

# DEFAULT - BUILT IN
set(CMAKE_CXX_STANDARD 20)

include_directories(
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/src/utils
        ${PROJECT_SOURCE_DIR}/src/ecs
        ${PROJECT_SOURCE_DIR}/src/ecs/system
        ${PROJECT_SOURCE_DIR}/src/vendor
        ${PROJECT_SOURCE_DIR}/src/scene
        ${PROJECT_SOURCE_DIR}/src/manager
        ${PROJECT_SOURCE_DIR}/src/event
)

# DEFAULT - BUILT IN
add_executable(comp8552_Final_Project src/main.cpp
        src/Game.h
        src/Game.cpp
        src/scene/SceneType.h
        src/scene/Scene.h
        src/scene/Scene.cpp
        src/ecs/system/AnimationClip.h
        src/ecs/system/AnimationSystem.h
        src/ecs/system/CameraSystem.h
        src/ecs/system/CollisionSystem.h
        src/ecs/system/CollisionSystem.cpp
        src/ecs/system/DestructionSystem.h
        src/ecs/system/EventResponseSystem.h
        src/ecs/system/EventResponseSystem.cpp
        src/ecs/system/KeyboardInputSystem.h
        src/ecs/system/MainMenuSystem.h
        src/ecs/system/MainMenuSystem.cpp
        src/ecs/system/MovementSystem.h
        src/ecs/system/RenderSystem.h
        src/ecs/system/SpawnTimerSystem.h
        src/event/BaseEvent.h
        src/event/EventManager.h
        src/manager/AssetManager.h
        src/manager/AssetManager.cpp
        src/manager/SceneManager.h
        src/utils/Collision.h
        src/utils/Collision.cpp
        src/utils/Vector2D.h
        src/utils/Vector2D.cpp
        src/vendor/tinyxml2.h
        src/vendor/tinyxml2.cpp
        src/GameObject.h
        src/GameObject.cpp
        src/TextureManager.h
        src/TextureManager.cpp
        src/ecs/Component.h
        src/ecs/ComponentType.h
        src/ecs/Entity.h
        src/ecs/World.h
        src/Map.h
        src/Map.cpp)

# MY ADDED CONFIG
target_link_libraries(comp8552_Final_Project
        PRIVATE SDL3::SDL3
        SDL3_image::SDL3_image
)