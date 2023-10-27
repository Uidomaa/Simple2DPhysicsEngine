#ifndef APPLICATION_H
#define APPLICATION_H

#include "./Graphics.h"
#include "Physics/Particle.h"
#include <vector>

class Application {
    private:
        bool running = false;
    std::vector<Particle*> particles;
    Vec2 pushForce{0,0};
    Uint32 timeOfPreviousFrame{};
    float deltaTime{};
    SDL_Rect liquid;

    void CreateParticle(float x, float y);

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