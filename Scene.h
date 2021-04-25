#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>

class Mesh;

class Scene
{
public:
	std::vector<Mesh*> meshes;
	std::vector<glm::mat4> rotations;
	std::vector<std::string> rotNames;

	Scene(int rotRes = 8)
	{

		for (int i = 0; i < 360; i += (360/rotRes))
			for (int j = 0; j < 360; j += (360 / rotRes))
			{
				this->rotations.push_back(glm::eulerAngleYXZ(glm::radians((float)(i)), glm::radians((float)(j)), glm::radians(0.0f)));
				this->rotNames.push_back("x" + std::to_string(i) + "y" + std::to_string(j));
			}
	}

	void readfile(std::string path);
	Mesh* getRotatedMesh(int mesh, int rot);


};

class Mesh
{
public:
	glm::vec3 max = glm::vec3(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
	glm::vec3 min = glm::vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

	
	std::vector<glm::vec3> verts;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> faces;
	std::vector<std::string> names;
};
