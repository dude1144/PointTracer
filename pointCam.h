#pragma once

#include <glm/glm.hpp>
#include <queue>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

class PointCam
{
public:
	glm::vec3 position;
	glm::vec3 aim;


	PointCam()
	{
		position = glm::vec3(0, 10, 0);
		aim = glm::vec3(0, 0, -1);

		std::cout << std::thread::hardware_concurrency() << std::endl;
	}
};