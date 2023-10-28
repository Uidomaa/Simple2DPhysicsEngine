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

    anchor = {Graphics::Width()/2.f, 0};
    for (int i = 0; i < 20; ++i)
    {
        auto particle = new Particle(Graphics::Width()/2 + i * 100, i * 50 + 50, 10.0);
        particle->radius = 20;
        particles.push_back(particle);
    }
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
            // if (event.button.button == SDL_BUTTON_RIGHT)
            // {
            //     int x, y;
            //     SDL_GetMouseState(&x, &y);
            //     CreateParticle(x,y);
            // }
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
                const float impulseMagnitude = (targetParticle->position - mouseCursor).Magnitude() * 0.1f;
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
        // for (int j = i + 1; j < particles.size(); ++j)
        // {
        //      //Gravitational attraction
        //      Vec2 attraction = Force::GenerateGravitationalForce(*particles[i], *particles[j], 10.f * PIXELS_PER_METRE);
        //      particles[i]->AddForce(attraction);
        //      particles[j]->AddForce(-attraction);
        // }
        //Spring
        if (i == 0)
        {
            Vec2 springForce = Force::GenerateSpringForce(*particles[i], anchor, 50, 100);
            particles[i]->AddForce(springForce);
        }
        else
        {
            Vec2 springForce = Force::GenerateSpringForce(*particles[i], *particles[i-1], 50, 100);
            particles[i]->AddForce(springForce);
            particles[i-1]->AddForce(-springForce);
        }
    }
    for (auto particle : particles)
    {
        //Gravity
        Vec2 weight = {0.f, particle->mass * 9.81f * PIXELS_PER_METRE};
        particle->AddForce(weight);
        //Input
        particle->AddForce(pushForce);
        // //Underwater
        // if (particle->position.y > liquid.y)
        // {
        //Drag
        particle->AddForce(Force::GenerateDragForce(*particle, 0.1f * PIXELS_PER_METRE));
        // }
        // else
        // //Above water
        // {
        //     //Wind
        //     particle->AddForce({2.f * PIXELS_PER_METRE, 0});
        //Friction
        particle->AddForce(Force::GenerateFrictionForce(*particle, 2.f * PIXELS_PER_METRE));
        // }

        //Integrate acceleration and velocity to get new position
        particle->Integrate(deltaTime);
    }    
}

///////////////////////////////////////////////////////////////////////////////
// Render function (called several times per second to draw objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Render() {
    Graphics::ClearScreen(0xFF003466);
    //Draw force line
    if (leftmouseButtonDown)
    {
        Graphics::DrawLine(targetParticle->position.x, targetParticle->position.y, mouseCursor.x, mouseCursor.y, 0xFF0000FF);
    }
    //Draw spring
    for (int i = 0; i < particles.size(); ++i)
    {
        if (i == 0)
        {
            Graphics::DrawLine(particles[i]->position.x, particles[i]->position.y, anchor.x, anchor.y, 0xFFAAAAAA);
        }
        else
        {
            Graphics::DrawLine(particles[i]->position.x, particles[i]->position.y, particles[i-1]->position.x, particles[i-1]->position.y, 0xFFAAAAAA);
        }
    }
    //Draw particles
    for (auto particle : particles)
    {
        Graphics::DrawFillCircle(particle->position.x, particle->position.y, particle->radius, 0xFFFF9496);
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