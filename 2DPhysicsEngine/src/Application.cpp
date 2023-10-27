#include "Application.h"

#include "Physics/Constants.h"
#include "Physics/Force.h"

bool Application::IsRunning() {
    return running;
}

void Application::CreateParticle(float x, float y)
{
    auto newParticle = new Particle(x, y, 1.0);
    newParticle->radius = 8;
    particles.push_back(newParticle);
}

///////////////////////////////////////////////////////////////////////////////
// Setup function (executed once in the beginning of the simulation)
///////////////////////////////////////////////////////////////////////////////
void Application::Setup() {
    running = Graphics::OpenWindow();

    auto poolBall = new Particle(Graphics::Width()/4, Graphics::Height()/2, 10.0);
    poolBall->radius = 32;
    particles.push_back(poolBall);
}

///////////////////////////////////////////////////////////////////////////////
// Input processing
///////////////////////////////////////////////////////////////////////////////
void Application::Input() {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                running = false;
            if (event.key.keysym.sym == SDLK_UP)
                pushForce.y = -50 * PIXELS_PER_METRE;
            if (event.key.keysym.sym == SDLK_RIGHT)
                pushForce.x = 50 * PIXELS_PER_METRE;
            if (event.key.keysym.sym == SDLK_DOWN)
                pushForce.y = 50 * PIXELS_PER_METRE;
            if (event.key.keysym.sym == SDLK_LEFT)
                pushForce.x = -50 * PIXELS_PER_METRE;
            break;
        case SDL_KEYUP:
            if (event.key.keysym.sym == SDLK_UP)
                pushForce.y = 0;
            if (event.key.keysym.sym == SDLK_RIGHT)
                pushForce.x = 0;
            if (event.key.keysym.sym == SDLK_DOWN)
                pushForce.y = 0;
            if (event.key.keysym.sym == SDLK_LEFT)
                pushForce.x = 0;
            break;
        case SDL_MOUSEMOTION:
            mouseCursor.x = event.motion.x;
            mouseCursor.y = event.motion.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (!leftmouseButtonDown && event.button.button == SDL_BUTTON_LEFT)
            {
                leftmouseButtonDown = true;
                int x, y;
                SDL_GetMouseState(&x, &y);
                mouseCursor.x = x;
                mouseCursor.y = y;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (leftmouseButtonDown && event.button.button == SDL_BUTTON_LEFT)
            {
                leftmouseButtonDown = false;
                const Vec2 impulseDirection = (particles[0]->position - mouseCursor).UnitVector();
                const float impulseMagnitude = (particles[0]->position - mouseCursor).Magnitude() * 0.1f;
                particles[0]->velocity = impulseDirection * impulseMagnitude;
            }
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Update function (called several times per second to update objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Update() {
    //Ensure constant frame rate
    int timeToWait = MILLISECONDS_PER_FRAME - (SDL_GetTicks() - timeOfPreviousFrame);
    if (timeToWait > 0) { SDL_Delay(timeToWait); }
    //Calculate Delta Time
    deltaTime = (SDL_GetTicks() - timeOfPreviousFrame) / 1000.f;
    //Limit deltaTime (to avoid massive values while debugging)
    if (deltaTime > 0.016) { deltaTime = 0.016f; }
    timeOfPreviousFrame = SDL_GetTicks();

    //Keep particles on-screen
    for (const auto particle : particles)
    {
        if (particle->position.x - particle->radius <= 0)
        {
            particle->position.x = particle->radius;
            particle->velocity.x *= -0.9f;
        }
        else if (particle->position.x + particle->radius >= Graphics::Width())
        {
            particle->position.x = Graphics::Width() - particle->radius;
            particle->velocity.x *= -0.9f;
        }
        if (particle->position.y - particle->radius <= 0)
        {
            particle->position.y = particle->radius;
            particle->velocity.y *= -0.9f;
        }
        else if (particle->position.y + particle->radius >= Graphics::Height())
        {
            particle->position.y = Graphics::Height() - particle->radius;
            particle->velocity.y *= -0.9f;
        }
    }
    //Update physics
    for (auto particle : particles)
    {
        //Friction
        particle->AddForce(Force::GenerateFrictionForce(*particle, 30 * PIXELS_PER_METRE));
        
        //Integrate acceleration and velocity to get new position
        particle->Integrate(deltaTime);
    }    
}

///////////////////////////////////////////////////////////////////////////////
// Render function (called several times per second to draw objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Render() {
    Graphics::ClearScreen(0xFF056263);
    //Draw force line
    if (leftmouseButtonDown)
    {
        Graphics::DrawLine(particles[0]->position.x, particles[0]->position.y, mouseCursor.x, mouseCursor.y, 0xFF0000FF);
    }
    //Draw particles
    for (auto particle : particles)
    {
        Graphics::DrawFillCircle(particle->position.x, particle->position.y, particle->radius, 0xFFFFFFFF);
    }
    Graphics::RenderFrame();
}

///////////////////////////////////////////////////////////////////////////////
// Destroy function to delete objects and close the window
///////////////////////////////////////////////////////////////////////////////
void Application::Destroy() {
    for (auto particle : particles)
    {
        delete particle;
    }
    Graphics::CloseWindow();
}