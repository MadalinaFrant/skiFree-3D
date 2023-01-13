#include "lab_m1/tema3/tema3.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


Tema3::Tema3()
{
}


Tema3::~Tema3()
{
}


void Tema3::Init()
{
    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "textures");

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "snow.jpeg").c_str(), GL_REPEAT);
        mapTextures["snow"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "skier.png").c_str(), GL_REPEAT);
        mapTextures["skier"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "gradient.jpeg").c_str(), GL_REPEAT);
        mapTextures["gradient"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "gift.png").c_str(), GL_REPEAT);
        mapTextures["gift"] = texture;
    }


    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("cone");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "cone.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Shader *shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    speed = 5;

    movement.x = 0;
    movement.y = 0;
    movement.z = 0;

    modifTex.x = 0;
    modifTex.y = 0;

    dir = 0;

    GetSceneCamera()->RotateOX(-200);
}


void Tema3::FrameStart()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema3::Update(float deltaTimeSeconds)
{
    movement.x += dir * 0.00005f;
    movement.z += speed * deltaTimeSeconds;

    modifTex.x = movement.x * 0.025f;
    modifTex.y = movement.z * 0.025f;

    glm::mat4 movementMatrix = glm::translate(glm::mat4(1), movement);

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1), RADIANS(30.0f), glm::vec3(1, 0, 0));

    glm::vec3 cPos = rotationMatrix * glm::vec4(movement, 1);
    cPos.y += 5;
    cPos.z += 7;

    GetSceneCamera()->SetPosition(cPos);

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= rotationMatrix;
        modelMatrix *= movementMatrix;
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
        RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix, mapTextures["snow"]);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= rotationMatrix;
        modelMatrix *= movementMatrix;
        modelMatrix = glm::rotate(modelMatrix, RADIANS(dirAngle), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.15f + 0.25f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, mapTextures["skier"]);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= rotationMatrix;
        modelMatrix *= movementMatrix;
        modelMatrix = glm::rotate(modelMatrix, RADIANS(dirAngle), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.25f, 0.075, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.15f, 1.5f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, mapTextures["gradient"]);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= rotationMatrix;
        modelMatrix *= movementMatrix;
        modelMatrix = glm::rotate(modelMatrix, RADIANS(dirAngle), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.25f, 0.075, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.15f, 1.5f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, mapTextures["gradient"]);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= rotationMatrix;
        modelMatrix = glm::translate(modelMatrix, glm::vec3(2, 0.25f, 10));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, mapTextures["gift"]);
    }

}


void Tema3::FrameEnd()
{
    //DrawCoordinateSystem();
}


void Tema3::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, 
                            Texture2D *texture)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);

    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    if (texture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture"), 0);
    }

    bool is_snow = false;

    if (texture == mapTextures["snow"]) {
        is_snow = true;
    }

    int is_snow_loc = glGetUniformLocation(shader->program, "isSnow");
    glUniform1i(is_snow_loc, is_snow);

    int loc_modif = glGetUniformLocation(shader->program, "texModif");
    glUniform2fv(loc_modif, 1, glm::value_ptr(modifTex));

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


void Tema3::OnInputUpdate(float deltaTime, int mods)
{
}


void Tema3::OnKeyPress(int key, int mods)
{
}


void Tema3::OnKeyRelease(int key, int mods)
{
}


void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    glm::ivec2 currResolution = window->GetResolution();

    dir = mouseX - currResolution.x / 2;

    dirAngle = dir * 0.075f;
}


void Tema3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}


void Tema3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}


void Tema3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema3::OnWindowResize(int width, int height)
{
}
