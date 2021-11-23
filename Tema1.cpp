#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/Tema1/transform2D.h"
#include "lab_m1/Tema1/object2D.h"

using namespace std;
using namespace m1;

Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


void Tema1::Init()
{
    auto camera = GetSceneCamera();
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    logicSpace.x = 0;
    logicSpace.y = 0;
    logicSpace.width = 90;   
    logicSpace.height = 60; 

    glm::vec3 corner = glm::vec3(0.001, 0.001, 0);

    length = 6;
    speed = 35;
    angularStep = 0;

    cx = (corner.x + length) / 2;
    cy = (corner.y + length) / 2;

    positionX = logicSpace.width / 2 - cx;
    positionY = logicSpace.height / 2 - cy / 2;

    PushWaterPos();

    Mesh* character = object2D::CreateCharacter("character", corner, length, glm::vec3(0, 0.3, 0), glm::vec3(0, 1, 0));
    AddMeshToList(character);

    Mesh* enemyMesh = object2D::CreateEnemy("enemyMesh", corner, length, glm::vec3(1, 0, 0), glm::vec3(0.8, 0, 0));
    AddMeshToList(enemyMesh);

    Mesh* water = object2D::CreateSquare("water", corner, length, glm::vec3(0.25f, 0.76f, 0.89f), true);
    AddMeshToList(water);

    Mesh* map = object2D::CreateSquare("map", glm::vec3(0.001, 0.001, -1), 120, glm::vec3(0.6f, 0.35f, 0.05f), true);
    AddMeshToList(map);

    Mesh* bullet = object2D::CreateSquare("bullet", corner, 1.5, glm::vec3(0, 0, 0), true);
    AddMeshToList(bullet);

    Mesh* healthBar = object2D::CreateSquare("healthBar", corner, 5, glm::vec3(1, 0, 0), true);
    AddMeshToList(healthBar);

    Mesh* healthBarBorder = object2D::CreateSquare("healthBarBorder", corner, 5, glm::vec3(0, 0, 0), false);
    AddMeshToList(healthBarBorder);
}

void Tema1::PushWaterPos()
{
    waterPos.push_back(make_pair(10, 50));
    waterPos.push_back(make_pair(16, 50));
    waterPos.push_back(make_pair(22, 50));

    waterPos.push_back(make_pair(60, 80));
    waterPos.push_back(make_pair(60, 86));
    waterPos.push_back(make_pair(60, 92));
    waterPos.push_back(make_pair(60, 98));
    waterPos.push_back(make_pair(66, 80));
    waterPos.push_back(make_pair(72, 80));
    waterPos.push_back(make_pair(78, 80));

    waterPos.push_back(make_pair(90, 30));
    waterPos.push_back(make_pair(90, 36));
    waterPos.push_back(make_pair(90, 42));
    waterPos.push_back(make_pair(90, 10));

    waterPos.push_back(make_pair(20, 10));
    waterPos.push_back(make_pair(26, 10));
    waterPos.push_back(make_pair(20, 16));
    waterPos.push_back(make_pair(26, 16));

    waterPos.push_back(make_pair(20, 100));
    waterPos.push_back(make_pair(26, 100));
    waterPos.push_back(make_pair(26, 94));
}

// 2D visualization matrix
glm::mat3 Tema1::VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
    float sx, sy, tx, ty;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    tx = viewSpace.x - sx * logicSpace.x;
    ty = viewSpace.y - sy * logicSpace.y;

    return glm::transpose(glm::mat3(
        sx, 0.0f, tx,
        0.0f, sy, ty,
        0.0f, 0.0f, 1.0f));
}


// Uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}


void Tema1::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}


void Tema1::FrameStart()
{
    glClearColor(0.25f, 0.76f, 0.89f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void Tema1::Update(float deltaTimeSeconds)
{
    if (playerHealth)
    {
        glm::ivec2 resolution = window->GetResolution();
        viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
        SetViewportArea(viewSpace, glm::vec3(0), true);

        visMatrix = glm::mat3(1);
        visMatrix *= VisualizationTransf2D(logicSpace, viewSpace);
        modelMatrix = glm::mat3(1);

        DrawPlayer();

        DrawHealthBar();

        for (int i = 0; i < bullets.size(); i++) {
            for (int j = 0; j < enemies.size(); j++) {
                if (CheckEnemyCollision(make_pair(enemies[j].x, enemies[j].y), make_pair(bullets[i].x, bullets[i].y), 4.5f)) {
                    enemies.erase(enemies.begin() + j);
                    bullets.erase(bullets.begin() + i);
                    cout << "Your score is : " << ++score << endl;
                    break;
                }
            }
        }

        for (int i = 0; i < bullets.size(); i++) {
            for (int j = 0; j < waterPos.size(); j++)
            {
                if (CheckWaterCollision(waterPos[j], make_pair(bullets[i].x, bullets[i].y), 4.5f))
                {
                    bullets.erase(bullets.begin() + i);
                    break;
                }
            }
        }

        for (int i = 0; i < bullets.size(); i++) {
            bullets[i].x += cos(bullets[i].angle) * deltaTimeSeconds * bullets[i].speed;
            bullets[i].y += sin(bullets[i].angle) * deltaTimeSeconds * bullets[i].speed;
            DrawBullet(bullets[i]);
            bullets[i].range -= deltaTimeSeconds;
        }

        if (fireRate <= 0) {
            fireRate = 0.2;
            canShoot = true;
        }
        else {
            fireRate -= deltaTimeSeconds;
        }

        if (counter <= 0) {
            counter = 3;
            AddEnemies(std::rand() % 4 + 1);
        }
        else {
            counter -= deltaTimeSeconds;
        }

        for (int i = 0; i < enemies.size(); i++) {
            if (CheckEnemyCollision(make_pair(enemies[i].x, enemies[i].y), make_pair(positionX, positionY), length)) {
                enemies.erase(enemies.begin() + i);
                playerHealth--;
            }
        }

        for (int i = 0; i < enemies.size(); i++)
        {
            enemies[i].angle = atan2(positionY - enemies[i].y, positionX - enemies[i].x);
            enemies[i].x += cos(enemies[i].angle) * deltaTimeSeconds * enemies[i].speed;
            enemies[i].y += sin(enemies[i].angle) * deltaTimeSeconds * enemies[i].speed;
            DrawEnemy(enemies[i]);
        }
        DrawScene(visMatrix);
    }
    else
    {
        if (gameOver == false) 
        {
            cout << "Game over !" << endl;
            gameOver = true;
        }
    }
}

void Tema1::Delay(float seconds)
{
    float end = clock() / CLOCKS_PER_SEC + seconds;
    while ((clock() / CLOCKS_PER_SEC) < end);
}

void Tema1::FrameEnd()
{
}

void Tema1::DrawScene(glm::mat3 visMatrix)
{
    for each (auto pos in waterPos)
    {
        modelMatrix = glm::mat3(1);
        modelMatrix = visMatrix * transform2D::Translate(pos.first, pos.second);
        RenderMesh2D(meshes["water"], shaders["VertexColor"], modelMatrix);
    }

    modelMatrix = glm::mat3(1);
    modelMatrix = visMatrix * transform2D::Translate(0, 0);
    RenderMesh2D(meshes["map"], shaders["VertexColor"], modelMatrix);
}

void Tema1::DrawEnemy(Enemy enemy)
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= visMatrix * transform2D::Translate(enemy.x + length/2, enemy.y + length/2)
        * transform2D::Rotate(enemy.angle - 3.14/2)
        * transform2D::Translate(-length / 2, -length / 2);
    RenderMesh2D(meshes["enemyMesh"], shaders["VertexColor"], modelMatrix);
}

void Tema1::DrawPlayer() 
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= visMatrix *
        transform2D::Translate(positionX + length / 2, positionY + length / 2)
        * transform2D::Rotate(angularStep)
        * transform2D::Translate(-length / 2, -length / 2);
    RenderMesh2D(meshes["character"], shaders["VertexColor"], modelMatrix);
}

void Tema1::DrawBullet(Bullet bullet)
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= visMatrix * transform2D::Translate(bullet.x + 3, bullet.y + 3)
        * transform2D::Rotate(bullet.angle);
    RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);
}

void Tema1::DrawHealthBar()
{
    modelMatrix = glm::mat3(1);
    modelMatrix *= visMatrix * transform2D::Translate(logicSpace.x + logicSpace.width - 31, logicSpace.y + logicSpace.height - 6)
        * transform2D::Scale(6, 1);
    RenderMesh2D(meshes["healthBarBorder"], shaders["VertexColor"], modelMatrix);
    modelMatrix = glm::mat3(1);
    modelMatrix *= visMatrix * transform2D::Translate(logicSpace.x + logicSpace.width - 31, logicSpace.y + logicSpace.height - 6)
        * transform2D::Scale(playerHealth, 1);
    RenderMesh2D(meshes["healthBar"], shaders["VertexColor"], modelMatrix);
}

void Tema1::AddEnemies(int number)
{
    while (number)
    {
        Enemy enemy;
        bool ok = false;
        enemy.x = std::rand() % 120;
        enemy.y = std::rand() % 120;
        while (ok == false) {
            if (sqrt((positionX - enemy.x) * (positionX - enemy.x) + (positionY - enemy.y) * (positionY - enemy.y)) > 20) {
                ok = true;
            }
            else {
                enemy.x = std::rand() % 120;
                enemy.y = std::rand() % 120;
            }
        }
        enemies.push_back(enemy);
        number --;
    }
}

bool Tema1::CheckWaterCollision(std::pair<float, float> water, std::pair<float, float> position, float length) 
{
    bool collisionX = water.first + length + 1.2f >= position.first &&
        position.first + length + 1.2f >= water.first;
    bool collisionY = water.second + length + 1.2f >= position.second &&
        position.second + length + 1.2f >= water.second;
    bool edgeCollX = position.first <= 1.2f || position.first + length + 1.2f >= 120;
    bool edgeCollY = position.second <= 1.2f || position.second + length + 1.2f >= 120;

    return (collisionX && collisionY) || edgeCollX || edgeCollY;
}

bool Tema1::CheckEnemyCollision(std::pair<float, float> enemy, std::pair<float, float> position, float length)
{
    bool collisionX = enemy.first + length + 1.2f >= position.first &&
        position.first + length + 1.2f >= enemy.first;
    bool collisionY = enemy.second + length + 1.2f >= position.second &&
        position.second + length + 1.2f >= enemy.second;
    return collisionX && collisionY;
}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    bool collision;
    if (window->KeyHold(GLFW_KEY_W)) {
        collision = false;
        for each (auto water in waterPos)
        {
            if (CheckWaterCollision(water, make_pair(positionX, positionY + speed * deltaTime), length))
            {
                collision = true;
                break;
            }
        }
        if (collision == false) 
        {
            positionY += speed * deltaTime;
            logicSpace.y += speed * deltaTime;
        }
    }
    else if (window->KeyHold(GLFW_KEY_S)) {
        collision = false;
        for each (auto water in waterPos)
        {
            if (CheckWaterCollision(water, make_pair(positionX, positionY - speed * deltaTime), length))
            {
                collision = true;
                break;
            }
        }
        if (collision == false)
        {
            positionY -= speed * deltaTime;
            logicSpace.y -= speed * deltaTime;
        }

    }
    else if (window->KeyHold(GLFW_KEY_A)) {
        collision = false;
        for each (auto water in waterPos)
        {
            if (CheckWaterCollision(water, make_pair(positionX - speed * deltaTime, positionY), length))
            {
                collision = true;
                break;
            }
        }
        if (collision == false) 
        {
            positionX -= speed * deltaTime;
            logicSpace.x -= speed * deltaTime;
        }
    }
    else if (window->KeyHold(GLFW_KEY_D)) {
        collision = false;
        for each (auto water in waterPos)
        {
            if (CheckWaterCollision(water, make_pair(positionX + speed * deltaTime, positionY), length))
            {
                collision = true;
                break;
            }
        }
        if (collision == false) 
        {
            positionX += speed * deltaTime;
            logicSpace.x += speed * deltaTime;
        }
    }
}


void Tema1::OnKeyPress(int key, int mods)
{
}


void Tema1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    glm::ivec2 resolution = window->GetResolution();
    float newY = (float)(resolution.y - mouseY) / resolution.y * logicSpace.height - logicSpace.height / 2 - cy / 2;
    float newX = (float)mouseX / resolution.x * logicSpace.width - logicSpace.width / 2 - cx;
    angularStep = atan2(newY, newX);
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT) && canShoot) {
        Bullet bullet;
        bullet.x = positionX;
        bullet.y = positionY;
        bullet.angle = angularStep;
        bullets.push_back(bullet);
        canShoot = false;
        fireRate = 0.2;
    }
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}
