#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

#include "components/simple_scene.h"
#include "components/transform.h"

using namespace std;


namespace m1
{
    class Tema3 : public gfxc::SimpleScene
    {
    	public:

			struct object {
				object(int ID, glm::vec3 point): ID(ID), point(point) {}

				int ID;
				glm::vec3 point;
			};

        	Tema3();
        	~Tema3();

        	void Init() override;

     	private:
			void FrameStart() override;
			void Update(float deltaTimeSeconds) override;
			void FrameEnd() override;

			void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, 
								Texture2D *texture = NULL);

			void OnInputUpdate(float deltaTime, int mods) override;
			void OnKeyPress(int key, int mods) override;
			void OnKeyRelease(int key, int mods) override;
			void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
			void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
			void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
			void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
			void OnWindowResize(int width, int height) override;

			void DefTextures();
			void DefMeshes();

			void RenderSkier();
			void RenderSkiTrack();
			void RenderGift(glm::vec3 point);
			void RenderRocks(glm::vec3 point);
			void RenderTree(glm::vec3 point);
			void RenderPole(glm::vec3 point);

			void RenderObject(int ID, glm::vec3 point);
			int GenObjectID();
			glm::vec3 GenSpawnPoint();
			float GenRandFloat(float min, float max);

			bool CheckCollision(int& i);

        	std::unordered_map<std::string, Texture2D*> mapTextures;

			float speed, dir, dirAngle, currAngle;
			
			glm::vec3 skierPosition;

			glm::vec2 modifyTexture;

			glm::mat4 movementMatrix, rotationMatrix;

			vector<object> objects;

			float currTime;

			bool collision, running;

			int score;
    };
}
