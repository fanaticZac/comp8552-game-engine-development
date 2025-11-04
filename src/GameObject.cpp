//
// Created by user on 2025-09-09.
//

#include "../GameObject.h"
#include "TextureManager.h"

#include <iostream>

GameObject::GameObject(const char *path, int x, int y) :
xPos(static_cast<float>(x)), yPos(static_cast<float>(y)), texture(TextureManager::load(path))
{

}

GameObject::~GameObject() {
    if (texture) SDL_DestroyTexture(texture);
}

void GameObject::update(float deltaTime) {
    // xPos++;
    // yPos++;
    xPos += speed * deltaTime;
    yPos += speed * deltaTime;

    std::cout << "xPos: " << xPos << " yPos: " << yPos << std::endl;

    //where do we want to crop the texture from?
    srcRect.x = srcRect.y = 0;

    //How much to crop?
    srcRect.w = srcRect.h = 32;

    //where to draw?
    // dstRect.x = static_cast<float>(xPos);
    // dstRect.y = static_cast<float> (yPos);
    dstRect.x = xPos;
    dstRect.y = yPos;

    // what size to render?
    dstRect.h = srcRect.h;
    dstRect.w = srcRect.w;
}

void GameObject::draw() {
    TextureManager::draw(texture, srcRect, dstRect);
}
