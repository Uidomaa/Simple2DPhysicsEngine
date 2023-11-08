#include "Application.h"

#include "Physics/CollisionDetection.h"
#include "Physics/Constants.h"
#include "Physics/Force.h"

bool Application::IsRunning() {
    return running;
}

void Application::CreateBody(float x, float y)
{
    auto newBody = new Body(CircleShape(50), x, y, 1.0);
    newBody->restitution = 0.7f;
    newBody->SetTexture("./assets/basketball.png");
    // auto newBody = new Body(BoxShape(50,50), x, y, 1);
    bodies.push_back(newBody);
}

void Application::CreateNewPolygon()
{
    if (newPolygonVertices.size() < 3) { return; }

    float xSum{}, ySum{};
    //Get average aka pivot point
    for (auto v : newPolygonVertices)
    {
        xSum += v.x;
        ySum += v.y;
    }
    float xAverage = xSum / newPolygonVertices.size();
    float yAverage = ySum / newPolygonVertices.size();
    Vec2 polygonCentre{xAverage, yAverage};
    //Rewrite vertices in relation to the pivot point
    for (int i = 0; i < newPolygonVertices.size(); i++)
    {
        newPolygonVertices[i] = newPolygonVertices[i] - polygonCentre;
    }
    //Create new polygon
    auto newBody = new Body(PolygonShape(newPolygonVertices), xAverage, yAverage, 5.0);
    newBody->restitution = 0.5f;
    bodies.push_back(newBody);
    //Clear polygon vertex list
    newPolygonVertices.clear();
}
///////////////////////////////////////////////////////////////////////////////
// Setup function (executed once in the beginning of the simulation)
///////////////////////////////////////////////////////////////////////////////
void Application::Setup() {
    running = Graphics::OpenWindow();

    //Static floor
    Body* floor = new Body(BoxShape(Graphics::Width() - 100, 100), Graphics::Width()/2.0f, Graphics::Height() - 100, 0.f);
    floor->restitution = 0.2f;
    floor->SetTexture("./assets/metal.png");
    bodies.push_back(floor);
    //Static ceiling
    Body* ceiling = new Body(BoxShape(Graphics::Width() - 100, 100), Graphics::Width()/2.0f, 100, 0.f);
    ceiling->restitution = 0.2f;
    ceiling->SetTexture("./assets/metal.png");
    bodies.push_back(ceiling);
    //Static left wall
    Body* lWall = new Body(BoxShape(100, Graphics::Height() - 100), 100, Graphics::Height()/2.0f, 0.f);
    lWall->restitution = 0.2f;
    lWall->SetTexture("./assets/metal.png");
    bodies.push_back(lWall);    
    //Static right wall
    Body* rWall = new Body(BoxShape(100, Graphics::Height() - 100), Graphics::Width() - 100, Graphics::Height()/2.0f, 0.f);
    rWall->restitution = 0.2f;
    rWall->SetTexture("./assets/metal.png");
    bodies.push_back(rWall);
    
    // //Static box/Circle
    Body* newBody = new Body(BoxShape(400, 400), Graphics::Width()/2.0f, Graphics::Height()/2.f, 0.f);
    // Body* newBody = new Body(CircleShape(200), Graphics::Width()/2.0f, Graphics::Height()/2.f, 0.f);
    newBody->restitution = 0.5f;
    newBody->rotation = 1.4f;
    newBody->SetTexture("./assets/crate.png");
    bodies.push_back(newBody);
    
    // sideLength = 250;
    // numColumns = 2;
    // numRows = 1;
    // for (int i = 0; i < numRows; ++i)
    // {
    //     for (int j = 0; j < numColumns; ++j)
    //     {
    //         auto body = new Body(BoxShape(200, 200), sideLength + j * sideLength, sideLength + i * sideLength, 50.0);
    //         body->rotation = 0.5f * j;
    //         bodies.push_back(body);
    //     }
    // }
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
            if (event.key.keysym.sym == SDLK_d)
                debug = !debug;
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
            // if (bodies.size() > 1)
            // {
            //     bodies[1]->position.x = event.motion.x;
            //     bodies[1]->position.y = event.motion.y;
            // }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_RIGHT)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
                CreateBody(x,y);
            }
            if (event.button.button == SDL_BUTTON_MIDDLE)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
                newPolygonVertices.emplace_back(x,y);
            }
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                CreateNewPolygon();
            }
            // if (!leftmouseButtonDown && event.button.button == SDL_BUTTON_LEFT)
            // {
            //     leftmouseButtonDown = true;
            //     int x, y;
            //     SDL_GetMouseState(&x, &y);
            //     mouseCursor.x = x;
            //     mouseCursor.y = y;
            //     //Get closest body to mouse
            //     targetBody = bodies[0];
            //     float closestDist = (targetBody->position - mouseCursor).MagnitudeSquared();
            //     for (const auto body : bodies)
            //     {
            //         float dist = (body->position - mouseCursor).MagnitudeSquared();
            //         if (dist < closestDist)
            //         {
            //             targetBody = body;
            //             closestDist = dist;
            //         }
            //     }
            // }
            break;
        case SDL_MOUSEBUTTONUP:
            // if (leftmouseButtonDown && event.button.button == SDL_BUTTON_LEFT)
            // {
            //     leftmouseButtonDown = false;
            //     const Vec2 impulseDirection = (targetBody->position - mouseCursor).UnitVector();
            //     const float impulseMagnitude = (targetBody->position - mouseCursor).Magnitude() * 0.1f;
            //     targetBody->velocity = impulseDirection * impulseMagnitude;
            // }
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

    // //Keep (circle) bodies on-screen
    // for (const auto body : bodies)
    // {
    //     if (body->shape->GetType() == CIRCLE)
    //     {
    //         CircleShape* circleShape = dynamic_cast<CircleShape*>(body->shape);
    //         if (body->position.x - circleShape->radius <= 0)
    //         {
    //             body->position.x = circleShape->radius;
    //             body->velocity.x *= -0.9f;
    //         }
    //         else if (body->position.x + circleShape->radius >= Graphics::Width())
    //         {
    //             body->position.x = Graphics::Width() - circleShape->radius;
    //             body->velocity.x *= -0.9f;
    //         }
    //         if (body->position.y - circleShape->radius <= 0)
    //         {
    //             body->position.y = circleShape->radius;
    //             body->velocity.y *= -0.9f;
    //         }
    //         else if (body->position.y + circleShape->radius >= Graphics::Height())
    //         {
    //             body->position.y = Graphics::Height() - circleShape->radius;
    //             body->velocity.y *= -0.9f;
    //         }
    //     }
    // }
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
        // body->AddTorque(30.f);
        // //Input
        // body->AddForce(pushForce);
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
                if (debug)
                {
                    Graphics::DrawFillCircle(contact.start.x, contact.start.y, 5, 0xFFBF5496);
                    Graphics::DrawFillCircle(contact.end.x, contact.end.y, 5, 0xFFBF5496);
                    Graphics::DrawLine(contact.start.x, contact.start.y, contact.start.x + contact.normal.x * 15, contact.start.y + contact.normal.y * 15,0xFFBF5496);
                    bodies[i]->shape->isColliding = true;
                    bodies[j]->shape->isColliding = true;
                }
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
            if (!debug && body->texture) { Graphics::DrawTexture(body->position.x, body->position.y, circleShape->radius * 2, circleShape->radius * 2, body->rotation, body->texture); }
            else { Graphics::DrawCircle(body->position.x, body->position.y, circleShape->radius, body->rotation, 0xFFBF5496); }
            // Graphics::DrawFillCircle(body->position.x, body->position.y, circleShape->radius, colour);
        }
        else if (body->shape->GetType() == BOX)
        {
            auto boxShape = dynamic_cast<BoxShape*> (body->shape);
            if (!debug && body->texture) { Graphics::DrawTexture(body->position.x, body->position.y, boxShape->width, boxShape->height, body->rotation, body->texture); }
            else { Graphics::DrawPolygon(body->position.x, body->position.y, boxShape->worldVertices, 0xFFBF5496); }
           
        }
        else if (body->shape->GetType() == POLYGON)
        {
            auto polygonShape = dynamic_cast<PolygonShape*> (body->shape);
            if (!debug) {Graphics::DrawFillPolygon(body->position.x, body->position.y, polygonShape->worldVertices, 0xFFBF5496); }
            else { Graphics::DrawPolygon(body->position.x, body->position.y, polygonShape->worldVertices, 0xFFBF5496); }
        }
    }
    for (auto vertex : newPolygonVertices)
    {
        Graphics::DrawFillCircle(vertex.x, vertex.y, 5, 0xFF9F5496);
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