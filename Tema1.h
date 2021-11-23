#pragma once

#include "components/simple_scene.h"


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
    public:
        struct ViewportSpace
        {
            ViewportSpace() : x(0), y(0), width(1), height(1) {}
            ViewportSpace(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        struct LogicSpace
        {
            LogicSpace() : x(0), y(0), width(1), height(1) {}
            LogicSpace(float x, float y, float width, float height)
                : x(x), y(y), width(width), height(height) {}
            float x;
            float y;
            float width;
            float height;
        };

        class Bullet
        {
            public:
                float range = 0.8;
                int speed = 30;
                float x, y;
                float angle;

                Bullet() {}
        };

        class Enemy
        {
            public:
                float x, y;
                float angle;
                float speed = 25;

                Enemy() {}
        };

    public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void DrawScene(glm::mat3 visMatrix);
        void DrawPlayer();
        void DrawEnemy(Enemy enemy);
        void DrawBullet(Bullet bullet);
        void DrawHealthBar();
        void PushWaterPos();
        bool CheckWaterCollision(std::pair<float, float> water, std::pair<float, float> position, float length);
        bool CheckEnemyCollision(std::pair<float, float> enemy, std::pair<float, float> position, float length);
        void Delay(float seconds);
        void AddEnemies(int number);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

        // Sets the logic space and view space
        // logicSpace: { x, y, width, height }
        // viewSpace: { x, y, width, height }
        glm::mat3 VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
        glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);

        void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);

    protected:
        float length;
        float speed;
        float angularStep;
        float positionX, positionY;
        float fireRate = 0.2;
        float cx, cy;
        float counter = 3;
        bool canShoot = true;
        int playerHealth = 6;
        int score = 0;
        bool gameOver = false;
        ViewportSpace viewSpace;
        LogicSpace logicSpace;
        glm::mat3 modelMatrix, visMatrix;
        std::vector<std::pair<float, float>> waterPos;
        std::vector<Bullet> bullets;
        std::vector<Enemy> enemies;
    };
}   // namespace m1
