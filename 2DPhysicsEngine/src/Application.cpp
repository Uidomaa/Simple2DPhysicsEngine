#include "Application.h"

#include "Physics/CollisionDetection.h"
#include "Physics/Constants.h"
#include "Physics/Force.h"

bool Application::IsRunning() {
    return running;
}

void Application::CreateBody(float x, float y)
{
    auto newBody = new Body(CircleShape(50), x, y, 4.0);
    bodies.push_back(newBody);
}

///////////////////////////////////////////////////////////////////////////////
// Setup function (executed once in the beginning of the simulation)
///////////////////////////////////////////////////////////////////////////////
void Application::Setup() {
    running = Graphics::OpenWindow();

    sideLength = 250;
    numColumns = 1;
    numRows = 2;
    for (int i = 0; i < numRows; ++i)
    {
        for (int j = 0; j < numColumns; ++j)
        {
            auto body = new Body(CircleShape(80), sideLength + j * sideLength, sideLength + i * sideLength, 50.0);
            bodies.push_back(body);
        }
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
            if (event.button.button == SDL_BUTTON_RIGHT)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
                CreateBody(x,y);
            }
            if (!leftmouseButtonDown && event.button.button == SDL_BUTTON_LEFT)
            {
                leftmouseButtonDown = true;
                int x, y;
                SDL_GetMouseState(&x, &y);
                mouseCursor.x = x;
                mouseCursor.y = y;
                //Get closest body to mouse
                targetBody = bodies[0];
                float closestDist = (targetBody->position - mouseCursor).MagnitudeSquared();
                for (const auto body : bodies)
                {
                    float dist = (body->position - mouseCursor).MagnitudeSquared();
                    if (dist < closestDist)
                    {
                        targetBody = body;
                        closestDist = dist;
                    }
                }
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (leftmouseButtonDown && event.button.button == SDL_BUTTON_LEFT)
            {
                leftmouseButtonDown = false;
                const Vec2 impulseDirection = (targetBody->position - mouseCursor).UnitVector();
                const float impulseMagnitude = (targetBody->position - mouseCursor).Magnitude() * 0.1f;
                targetBody->velocity = impulseDirection * impulseMagnitude;
            }
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Update function (called several times per second to update objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Update() {
    //Clear screen in Update so we can draw debug info
    Graphics::ClearScreen(0xFF101426);
    //Ensure constant frame rate
    int timeToWait = MILLISECONDS_PER_FRAME - (SDL_GetTicks() - timeOfPreviousFrame);
    if (timeToWait > 0) { SDL_Delay(timeToWait); }
    //Calculate Delta Time
    deltaTime = (SDL_GetTicks() - timeOfPreviousFrame) / 1000.f;
    //Limit deltaTime (to avoid massive values while debugging)
    if (deltaTime > 0.016) { deltaTime = 0.016f; }
    timeOfPreviousFrame = SDL_GetTicks();

    //Keep bodies on-screen
    for (const auto body : bodies)
    {
        if (body->shape->GetType() == CIRCLE)
        {
            CircleShape* circleShape = dynamic_cast<CircleShape*>(body->shape);
            if (body->position.x - circleShape->radius <= 0)
            {
                body->position.x = circleShape->radius;
                body->velocity.x *= -0.9f;
            }
            else if (body->position.x + circleShape->radius >= Graphics::Width())
            {
                body->position.x = Graphics::Width() - circleShape->radius;
                body->velocity.x *= -0.9f;
            }
            if (body->position.y - circleShape->radius <= 0)
            {
                body->position.y = circleShape->radius;
                body->velocity.y *= -0.9f;
            }
            else if (body->position.y + circleShape->radius >= Graphics::Height())
            {
                body->position.y = Graphics::Height() - circleShape->radius;
                body->velocity.y *= -0.9f;
            }
        }
    }
    //Update physics
    for (int i = 0; i < bodies.size(); ++i)
    {
        // for (int j = i + 1; j < bodies.size(); ++j)
        // {
        //      //Gravitational attraction
        //      Vec2 attraction = Force::GenerateGravitationalForce(*bodies[i], *bodies[j], 10.f * PIXELS_PER_METRE);
        //      bodies[i]->AddForce(attraction);
        //      bodies[j]->AddForce(-attraction);
        // }
        // //Spring forces
        // for (int j = i + 1; j < bodies.size(); ++j)
        // {
        //     //Only connect neighbours
        //     const int deltaA = (j % numColumns) - (i % numColumns);
        //     const int deltaB = (j / numColumns) - (i / numColumns);
        //     if (deltaA + deltaB < 0 || deltaA + deltaB > 2 || (deltaA != 1 && deltaB != 1)) { continue; }
        //     
        //     float a = deltaA * sideLength;
        //     float b = deltaB * sideLength;
        //     float springLength = sqrt((a * a) + (b * b));
        //     
        //     Vec2 springForce = Force::GenerateSpringForce(*bodies[i], *bodies[j], springLength, 80);
        //     bodies[i]->AddForce(springForce);
        //     bodies[j]->AddForce(-springForce);
        // }
    }
    for (auto body : bodies)
    {
        //Gravity
        Vec2 weight = {0.f, body->mass * 9.81f * PIXELS_PER_METRE};
        body->AddForce(weight);
        // //Torque
        // body->AddTorque(20.f);
        //Input
        body->AddForce(pushForce);
        // //Drag
        // body->AddForce(Force::GenerateDragForce(*body, 0.1f * PIXELS_PER_METRE));
        // //Wind
        // body->AddForce({2.f * PIXELS_PER_METRE, 0});
        // //Friction
        // body->AddForce(Force::GenerateFrictionForce(*body, 2.f * PIXELS_PER_METRE));

        //Update polygon vertices
        body->Update(deltaTime);
    }
    //Reset collision debug state
    for (auto body : bodies)
    {
        body->shape->isColliding = false;
    }
    //Handle Collision
    for (int i = 0; i < bodies.size(); ++i)
    {
        for (int j = i + 1; j < bodies.size(); ++j)
        {
            Contact contact{};
            const bool isColliding = CollisionDetection::IsColliding(bodies[i], bodies[j], contact);
            if (isColliding)
            {
                contact.ResolveCollision();
                bodies[i]->shape->isColliding = true;
                bodies[j]->shape->isColliding = true;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Render function (called several times per second to draw objects)
///////////////////////////////////////////////////////////////////////////////
void Application::Render() {
    
    //Draw force line
    if (leftmouseButtonDown)
    {
        Graphics::DrawLine(targetBody->position.x, targetBody->position.y, mouseCursor.x, mouseCursor.y, 0xFF0000FF);
    }
    // //Draw spring
    // for (int i = 0; i < bodies.size(); ++i)
    // {
    //     for (int j = i + 1; j < bodies.size(); ++j)
    //     {
    //         const int deltaA = (j % numColumns) - (i % numColumns);
    //         const int deltaB = (j / numColumns) - (i / numColumns);
    //         if (deltaA + deltaB < 0 || deltaA + deltaB > 2 || (deltaA != 1 && deltaB != 1)) { continue; } //Only connect neighbours
    //         Graphics::DrawLine(bodies[i]->position.x, bodies[i]->position.y, bodies[j]->position.x, bodies[j]->position.y, 0xFFFF94F6);
    //     }
    // }
    //Draw bodies
    for (auto body : bodies)
    {
        if (body->shape->GetType() == CIRCLE)
        {
            auto circleShape = dynamic_cast<CircleShape*> (body->shape);
            Uint32 colour = circleShape->isColliding ? 0xFF999999 : 0xFFBF5496;
            // Graphics::DrawCircle(body->position.x, body->position.y, circleShape->radius, body->rotation, colour);
            Graphics::DrawFillCircle(body->position.x, body->position.y, circleShape->radius, colour);
        }
        else if (body->shape->GetType() == BOX)
        {
            auto boxShape = dynamic_cast<BoxShape*> (body->shape);
            Graphics::DrawPolygon(body->position.x, body->position.y, boxShape->worldVertices, 0xFFFF94F6);
        }
    }
    Graphics::RenderFrame();
}

///////////////////////////////////////////////////////////////////////////////
// Destroy function to delete objects and close the window
///////////////////////////////////////////////////////////////////////////////
void Application::Destroy() {
    for (auto body : bodies)
    {
        delete body;
    }
    Graphics::CloseWindow();
}