#include "lab_m1/Tema1/object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"

Mesh* object2D::CreateSquare(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}


Mesh* object2D::CreateCharacter(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color1,
    glm::vec3 color2)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color1),
        VertexFormat(corner + glm::vec3(length, 0, 0), color1),
        VertexFormat(corner + glm::vec3(length, length, 0), color1),
        VertexFormat(corner + glm::vec3(0, length, 0), color1),

        VertexFormat(corner + glm::vec3(length / 6 * 5, length / 6, 0), color2),
        VertexFormat(corner + glm::vec3(length / 6 * 5, length / 6 * 3, 0), color2), // punct comun
        VertexFormat(corner + glm::vec3(length / 6 * 7, length / 6 * 2, 0), color2),
        VertexFormat(corner + glm::vec3(length / 6 * 5, length / 6 * 5, 0), color2),
        VertexFormat(corner + glm::vec3(length / 6 * 7, length / 6 * 4, 0), color2)
    };

    Mesh* character = new Mesh(name);
    std::vector<unsigned int> indices =
    {
        4, 6, 5, 
        5, 8, 7,
        0, 1, 2,
        3, 0, 2};

    character->InitFromData(vertices, indices);
    return character;
}

Mesh* object2D::CreateEnemy(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color1,
    glm::vec3 color2)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color1),
        VertexFormat(corner + glm::vec3(length, 0, 0), color1),
        VertexFormat(corner + glm::vec3(length / 2, length, 0), color1),

        VertexFormat(corner + glm::vec3(length / 3, length / 3 * 2, 0), color2),
        VertexFormat(corner + glm::vec3(0, length / 3 * 2, 0), color2),
        VertexFormat(corner + glm::vec3(length / 3, length, 0), color2),

        VertexFormat(corner + glm::vec3(length / 3 * 2, length / 3 * 2, 0), color2),
        VertexFormat(corner + glm::vec3(length, length / 3 * 2, 0), color2),
        VertexFormat(corner + glm::vec3(length / 3 * 2, length, 0), color2),
    };

    Mesh* enemy = new Mesh(name);
    std::vector<unsigned int> indices =
    {0, 1, 2,
    3, 4, 5,
    6, 7, 8};

    enemy->InitFromData(vertices, indices);
    return enemy;
}