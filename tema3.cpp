#include "lab_m1/tema3/tema3.h"

using namespace std;
using namespace m1;


Tema3::Tema3()
{
}


Tema3::~Tema3()
{
}


/* Se definesc obiectele utilizate */
void Tema3::DefMeshes()
{
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
}


/* Se definesc texturile utilizate */
void Tema3::DefTextures()
{
    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, 
                                            "tema3", "textures");

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "snow.jpeg").c_str(), GL_REPEAT);
        mapTextures["snow"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "blue_pattern.png").c_str(), GL_REPEAT);
        mapTextures["blue_pattern"] = texture;
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
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "rock.jpg").c_str(), GL_REPEAT);
        mapTextures["rock"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "wood.jpeg").c_str(), GL_REPEAT);
        mapTextures["wood"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "pines.jpg").c_str(), GL_REPEAT);
        mapTextures["pines"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "concrete.jpg").c_str(), GL_REPEAT);
        mapTextures["concrete"] = texture;
    }
}


void Tema3::Init()
{
    DefMeshes();
    DefTextures();

    {
        Shader *shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    skierPosition = glm::vec3(0); // pozitia schiorului

    modifyTexture = glm::vec2(0); // valoarea cu care se modifica coordonatele de texturare

    speed = 5; // viteza de deplasare
    dir = 0; // directia de deplasare (pe ox)
    currAngle = 0; // unghiul dat de directia deplasarii

    currTime = 0; // timpul trecut de la inceputul jocului

    running = true; // jocul ruleaza pana la o coliziune cu un obstacol
    collision = false; // initial nu exista coliziune

    score = 0;

    // unghi de inclinare al planului
    rotationMatrix = glm::rotate(glm::mat4(1), RADIANS(30.0f), glm::vec3(1, 0, 0));

    GetSceneCamera()->RotateOX(-200); // pozitionare estetica a camerei
}


void Tema3::FrameStart()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
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

    int loc_modif = glGetUniformLocation(shader->program, "textureModif");
    glUniform2fv(loc_modif, 1, glm::value_ptr(modifyTexture));

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


void Tema3::RenderSkiTrack()
{
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix *= rotationMatrix;
    modelMatrix *= movementMatrix;
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
    RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix, mapTextures["snow"]);
}


void Tema3::RenderSkier()
{
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= rotationMatrix;
        modelMatrix *= movementMatrix;
        modelMatrix = glm::rotate(modelMatrix, RADIANS(dirAngle), glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.15f + 0.25f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, mapTextures["blue_pattern"]);
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
}


void Tema3::RenderRocks(glm::vec3 point)
{
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= rotationMatrix;
        modelMatrix = glm::translate(modelMatrix, glm::vec3(point.x - 0.5f, point.y + 0.25f, point.z));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
        RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix, mapTextures["rock"]);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= rotationMatrix;
        modelMatrix = glm::translate(modelMatrix, glm::vec3(point.x, point.y + 0.35f, point.z));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.7f));
        RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix, mapTextures["rock"]);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= rotationMatrix;
        modelMatrix = glm::translate(modelMatrix, glm::vec3(point.x + 0.5f, point.y + 0.15f, point.z));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
        RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix, mapTextures["rock"]);
    }
}


void Tema3::RenderTree(glm::vec3 point)
{
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= rotationMatrix;
        modelMatrix = glm::translate(modelMatrix, glm::vec3(point.x, point.y + 0.625f, point.z));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 1.25f, 0.25f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, mapTextures["wood"]);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= rotationMatrix;
        modelMatrix = glm::translate(modelMatrix, glm::vec3(point.x, point.y + 1.25f + 0.65f, point.z));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.65f, 0.5f));
        RenderSimpleMesh(meshes["cone"], shaders["LabShader"], modelMatrix, mapTextures["pines"]);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= rotationMatrix;
        modelMatrix = glm::translate(modelMatrix, glm::vec3(point.x, point.y + 1.25f + 0.65f * 2, point.z));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.35f, 0.5f, 0.35f));
        RenderSimpleMesh(meshes["cone"], shaders["LabShader"], modelMatrix, mapTextures["pines"]);
    }
}


void Tema3::RenderPole(glm::vec3 point)
{
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= rotationMatrix;
        modelMatrix = glm::translate(modelMatrix, glm::vec3(point.x, point.y + 1.5f, point.z));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 3, 0.1f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, mapTextures["concrete"]);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= rotationMatrix;
        modelMatrix = glm::translate(modelMatrix, glm::vec3(point.x, point.y + 3 + 0.05f, point.z));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.25f, 0.1f, 0.1f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, mapTextures["concrete"]);
    }
}


void Tema3::RenderGift(glm::vec3 point) 
{
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix *= rotationMatrix;
    modelMatrix = glm::translate(modelMatrix, glm::vec3(point.x, point.y + 0.3f, point.z));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.6f));
    RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, mapTextures["gift"]);
}


int Tema3::GenObjectID()
{
    srand(time(0));
    return (rand() % 4);
}


void Tema3::RenderObject(int ID, glm::vec3 point)
{
    switch (ID) {
        case 0:
            RenderGift(point);
            break;

        case 1:
            RenderRocks(point);
            break;

        case 2:
            RenderTree(point);
            break;

        case 3:
            RenderPole(point);
            break;
        
        default:
            break;
    }

}


float Tema3::GenRandFloat(float min, float max)
{
    srand(time(0));
    return (min + static_cast<float>(rand()) * static_cast<float>(max - min) / RAND_MAX);
}


glm::vec3 Tema3::GenSpawnPoint()
{
    srand(time(0));

    float xMin = skierPosition.x - 7;
    float xMax = skierPosition.x + 7;
    float zMin = skierPosition.z + 5;
    float zMax = skierPosition.z + 10;

    glm::vec3 point = glm::vec3(0);

    point.x = GenRandFloat(xMin, xMax);
    point.z = GenRandFloat(zMin, zMax);

    return point;
}


bool Tema3::CheckCollision(int& ind)
{
    glm::vec3 skierPos = skierPosition;

    for (int i = 0; i < objects.size(); i++) {

        glm::vec3 objPos = objects[i].point;

        bool collision_x = skierPos.x + 0.75f >= objPos.x && objPos.x + 0.75f >= skierPos.x;
        bool collision_y = skierPos.z + 0.75f >= objPos.z && objPos.z + 0.75f >= skierPos.z;

        if (collision_x && collision_y) {

            if (objects[i].ID == 0) {
                ind = i;
            } else {
                ind = -1;
            }

            return true;
        }

    }

    return false;
}


void Tema3::Update(float deltaTimeSeconds)
{
    currTime += deltaTimeSeconds; // se actualizeaza timpul

    int ind;
    collision = CheckCollision(ind); // se verifica existenta unei coliziuni

    if (collision && ind == -1) { // colizune intre jucator si un obstacol
        if (running) {
            cout << "~~~~ Score = " << score << " ~~~~\n";
        }
        running = false;
    } else {

        if (collision) { // coliziune intre jucator si un cadou
            objects.erase(objects.begin() + ind);
            score++;
        }

        dirAngle = currAngle;

        skierPosition.x += dir * 0.00005f;
        skierPosition.z += speed * deltaTimeSeconds;

        float frac = currTime - floor(currTime);

        if ((frac >= 0.0000f && frac <= 0.0075f) || (frac >= 0.5000f && frac <= 0.5075f)) {
            objects.push_back(object(GenObjectID(), GenSpawnPoint()));
        }

        if (objects.size() >= 15) {
            objects.erase(objects.begin());
        }

    }

    modifyTexture.x = skierPosition.x * 0.025f;
    modifyTexture.y = skierPosition.z * 0.025f;

    movementMatrix = glm::translate(glm::mat4(1), skierPosition);

    /* Se pozitioneaza camera in functie de pozitia jucatorului pentru a il urmari
    pe acesta constant */
    glm::vec3 cameraPosition = rotationMatrix * glm::vec4(skierPosition, 1);
    cameraPosition.y += 4;
    cameraPosition.z += 7;

    GetSceneCamera()->SetPosition(cameraPosition);

    RenderSkiTrack();
    RenderSkier();

    for (int i = 0; i < objects.size(); i++) {
        RenderObject(objects[i].ID, objects[i].point);
    }

}


void Tema3::FrameEnd()
{
}


void Tema3::OnInputUpdate(float deltaTime, int mods)
{
}


void Tema3::OnKeyPress(int key, int mods)
{

    if (key == GLFW_KEY_SPACE && running == false) {
        running = true;
        collision = false;
        score = 0;
        skierPosition.z += 2;
    }

}


void Tema3::OnKeyRelease(int key, int mods)
{
}


void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    glm::ivec2 currResolution = window->GetResolution();

    dir = mouseX - currResolution.x / 2;
    currAngle = dir * 0.075f;
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
