//
// Created by user on 2025-09-09.
//

#ifndef COMP8552_FINAL_PROJECT_GAMEOBJECT_H
#define COMP8552_FINAL_PROJECT_GAMEOBJECT_H

#include "Game.h"

class GameObject {
public:
    float speed = 10.0; // pixels / second

    GameObject(const char* path, int x, int y);
    ~GameObject();

    void update(float deltaTime);
    void draw();
private:
    float xPos{}, yPos{};

    SDL_Texture* texture = nullptr;
    SDL_FRect srcRect{}, dstRect{};

};

#endif //COMP8552_FINAL_PROJECT_GAMEOBJECT_H