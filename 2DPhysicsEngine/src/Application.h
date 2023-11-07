#ifndef APPLICATION_H
#define APPLICATION_H

#include "./Graphics.h"
#include "Physics/Body.h"
#include <vector>

class Application {
    private:
    bool running = false;
    std::vector<Body*> bodies;
    Vec2 pushForce{0,0};
    Uint32 timeOfPreviousFrame{};
    float deltaTime{};
    // SDL_Rect liquid;
    // Vec2 anchor{};
    int numColumns{};
    int numRows{};
    float sideLength{};
    Vec2 mouseCursor{};
    bool leftmouseButtonDown{};
    Body* targetBody{nullptr};

    void CreateBody(float x, float y, char shapeType);

    public:
        Application() = default;
        ~Application() = default;
        bool IsRunning();
        void Setup();
        void Input();
        void Update();
        void Render();
        void Destroy();
};

#endif