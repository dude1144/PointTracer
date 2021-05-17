#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>
#include <queue>
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>

#include "Scene.h"
#include "FreeImage.h"

class ray
{
public:
	glm::vec3 point;
	glm::vec3 dir;

	ray(glm::vec3 p, glm::vec3 d)
	{
		this->point = p;
		this->dir = d;
	}

	bool intersectTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3* bary);
	bool intersectPlane(glm::vec3 min, glm::vec3 max);
	/*
	Ray-box intersection using IEEE numerical properties to ensure that the
	test is both robust and efficient, as described in:

		Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
		"An Efficient and Robust Ray-Box Intersection Algorithm"
		Journal of graphics tools, 10(1):49-54, 2005
	
	 */
	bool intersectCube(glm::vec3 min, glm::vec3 max);
	bool intersectMesh(Mesh* m, glm::vec3 *bary);
};

class viewPlane
{
public:
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 min;
	glm::vec2 max;
	float offset;

	viewPlane(glm::vec3 position, glm::vec3 normal, glm::vec2 min = glm::vec2(-3,-3), glm::vec2 max = glm::vec2(3, 3), float offset = 5)
	{
		this->position = position;
		this->normal = normal;
		this->min = min;
		this->max = max;
		this->offset = offset;
	}


};

class PointCam
{
public:
	glm::vec3 position;
	glm::vec3 aim;

	glm::vec3 viewMin;
	glm::vec3 viewMax;

	PointCam()
	{
		position = glm::vec3(0, 0, 10);
		aim = glm::normalize(glm::vec3(1, 1, -1));

		viewMin = glm::vec3(this->position.x + this->aim.x * -3, this->position.y + this->aim.y * -3, this->position.z + this->aim.z * 5);
		viewMax = glm::vec3(3, 3, this->position.z + this->aim.z * 5);
	}

	ray getRay(float u, float v);
	void adjustToBounds(glm::vec3 min, glm::vec3 max);
	void renderImage(Scene* scene, FIBITMAP* image, std::vector<glm::vec3>* points, int rot = 0);

private:
	bool contains(glm::vec3 min, glm::vec3 max, glm::vec3 p)
	{
		return (p.x > min.x && p.x < max.x) && (p.y > min.y && p.y < max.y) && (p.z > min.z && p.z < max.z);
	}
};