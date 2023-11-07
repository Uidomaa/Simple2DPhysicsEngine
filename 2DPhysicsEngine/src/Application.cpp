#include "Application.h"

#include "Physics/CollisionDetection.h"
#include "Physics/Constants.h"
#include "Physics/Force.h"

bool Application::IsRunning() {
    return running;
}

void Application::CreateBody(float x, float y, char shapeType)
{
    Body* newBody;
    if (shapeType == 'c') { newBody = new Body(CircleShape(50), x, y, 1.0); }
    else if (shapeType == 'b') { newBody = new Body(BoxShape(100,100), x, y, 1.0); }
    else { return; }
    
    newBody->restitution = 0.7f;
    bodies.push_back(newBody);
}

///////////////////////////////////////////////////////////////////////////////
// Setup function (executed once in the beginning of the simulation)
///////////////////////////////////////////////////////////////////////////////
void Application::Setup() {
    running = Graphics::OpenWindow();

    //Static floor
    Body* floor = new Body(BoxShape(Graphics::Width() - 100, 100), Graphics::Width()/2.0f, Graphics::Height() - 100, 0.f);
    floor->restitution = 0.2f;
    bodies.push_back(floor);
    //Static ceiling
    Body* ceiling = new Body(BoxShape(Graphics::Width() - 100, 100), Graphics::Width()/2.0f, 100, 0.f);
    ceiling->restitution = 0.2f;
    bodies.push_back(ceiling);
    //Static left wall
    Body* lWall = new Body(BoxShape(100, Graphics::Height() - 100), 100, Graphics::Height()/2.0f, 0.f);
    lWall->restitution = 0.2f;
    bodies.push_back(lWall);    
    //Static right wall
    Body* rWall = new Body(BoxShape(100, Graphics::Height() - 100), Graphics::Width() - 100, Graphics::Height()/2.0f, 0.f);
    rWall->restitution = 0.2f;
    bodies.push_back(rWall);
    
    // //Static box/Circle
    Body* newBody = new Body(BoxShape(400, 400), Graphics::Width()/2.0f, Graphics::Height()/2.f, 0.f);
    // Body* newBody = new Body(CircleShape(200), Graphics::Width()/2.0f, Graphics::Height()/2.f, 0.f);
    newBody->restitution = 0.5f;
    newBody->rotation = 1.4f;
    bodies.push_back(newBody);
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

            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_RIGHT)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
                CreateBody(x,y,'c');
            }
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
                CreateBody(x,y,'b');
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
    
    //Update physics
    for (auto body : bodies)
    {
        //Gravity
        Vec2 weight = {0.f, body->mass * 9.81f * PIXELS_PER_METRE};
        body->AddForce(weight);
        //Update polygon vertices
        body->Update(deltaTime);
    }
    //Reset collision debug state
    for (const auto body : bodies)
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
                //Draw debug info
                // Graphics::DrawFillCircle(contact.start.x, contact.start.y, 5, 0xFFBF5496);
                // Graphics::DrawFillCircle(contact.end.x, contact.end.y, 5, 0xFFBF5496);
                // Graphics::DrawLine(contact.start.x, contact.start.y, contact.start.x + contact.normal.x * 15, contact.start.y + contact.normal.y * 15,0xFFBF5496);
                // bodies[i]->shape->isColliding = true;
                // bodies[j]->shape->isColliding = true;
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
    //Draw bodies
    for (auto body : bodies)
    {
        if (body->shape->GetType() == CIRCLE)
        {
            auto circleShape = dynamic_cast<CircleShape*> (body->shape);
            Uint32 colour = circleShape->isColliding ? 0xFF999999 : 0xFFBF5496;
            Graphics::DrawCircle(body->position.x, body->position.y, circleShape->radius, body->rotation, colour);
            // Graphics::DrawFillCircle(body->position.x, body->position.y, circleShape->radius, colour);
        }
        else if (body->shape->GetType() == BOX)
        {
            auto boxShape = dynamic_cast<BoxShape*> (body->shape);
            Uint32 colour = boxShape->isColliding ? 0xFF999999 : 0xFFBF5496;
            Graphics::DrawPolygon(body->position.x, body->position.y, boxShape->worldVertices, colour);
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