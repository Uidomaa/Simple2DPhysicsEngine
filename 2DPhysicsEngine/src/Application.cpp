#include "Application.h"

#include "Physics/Constants.h"
#include "Physics/Force.h"

bool Application::IsRunning() {
    return running;
}

void Application::CreateParticle(float x, float y)
{
    auto newParticle = new Particle(x, y, 4.0);
    newParticle->radius = 12;
    particles.push_back(newParticle);
}

///////////////////////////////////////////////////////////////////////////////
// Setup function (executed once in the beginning of the simulation)
///////////////////////////////////////////////////////////////////////////////
void Application::Setup() {
    running = Graphics::OpenWindow();

    auto poolBall = new Particle(Graphics::Width()/2, Graphics::Height()/2, 10000.0);
    poolBall->radius = 100;
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
            if (event.button.button == SDL_BUTTON_RIGHT)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
                CreateParticle(x,y);
            }
            if (!leftmouseButtonDown && event.button.button == SDL_BUTTON_LEFT)
            {
                leftmouseButtonDown = true;
                int x, y;
                SDL_GetMouseState(&x, &y);
                mouseCursor.x = x;
                mouseCursor.y = y;
                //Get closest particle to mouse
                targetParticle = particles[0];
                float closestDist = (targetParticle->position - mouseCursor).MagnitudeSquared();
                for (const auto particle : particles)
                {
                    float dist = (particle->position - mouseCursor).MagnitudeSquared();
                    if (dist < closestDist)
                    {
                        targetParticle = particle;
                        closestDist = dist;
                    }
                }
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (leftmouseButtonDown && event.button.button == SDL_BUTTON_LEFT)
            {
                leftmouseButtonDown = false;
                const Vec2 impulseDirection = (targetParticle->position - mouseCursor).UnitVector();
                const float impulseMagnitude = (targetParticle->position - mouseCursor).Magnitude() * 0.01f;
                targetParticle->velocity = impulseDirection * impulseMagnitude;
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
    for (int i = 0; i < particles.size(); ++i)
    {
        for (int j = i + 1; j < particles.size(); ++j)
        {
            //Gravitational attraction
            Vec2 attraction = Force::GenerateGravitationalForce(*particles[i], *particles[j], 10.f * PIXELS_PER_METRE);
            particles[i]->AddForce(attraction);
            particles[j]->AddForce(-attraction);
        }
    }
    for (auto particle : particles)
    {
        //Integrate acceleration and velocity to get new position
        particle->Integrate(deltaTime);
    }    
}

///////////////////////////////////////////////////////////////////////////////
// Render function (called several times per second to draw objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Render() {
    Graphics::ClearScreen(0xFF000152);
    //Draw force line
    if (leftmouseButtonDown)
    {
        Graphics::DrawLine(targetParticle->position.x, targetParticle->position.y, mouseCursor.x, mouseCursor.y, 0xFF0000FF);
    }
    //Draw particles
    for (auto particle : particles)
    {
        Graphics::DrawFillCircle(particle->position.x, particle->position.y, particle->radius, 0xFF8a850b);
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