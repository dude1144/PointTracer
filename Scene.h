#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>

#include <glm/glm.hpp>

#include "pointCam.h"

class Mesh;

class Scene
{
public:
	std::vector<Mesh*> meshes;
	void readfile(std::string path);
};

class Mesh
{
public:
	float maxX = std::numeric_limits<float>::min();
	float minX = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::min();
	float minY = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::min();
	float minZ = std::numeric_limits<float>::max();

	std::vector<glm::vec3*> verts;
	std::vector<glm::vec2*> uv;
	std::vector<glm::vec3*> normals;
	std::vector<glm::vec3*> faces;
	std::vector<std::string*> names;
};
