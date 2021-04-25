#include "pointCam.h"

#define MAX_THREADS 4
#define TILE_SIZE 10

ray PointCam::getRay(float u, float v)
{
	float w = viewMax.x - viewMin.x;
	float h = viewMax.y - viewMin.y;
	glm::vec3 pointOnPlane = (glm::vec3((u * w) + viewMin.x, (v * h) + viewMin.y, viewMin.z));
	return ray(this->position, glm::normalize(pointOnPlane - this->position));
}

void PointCam::adjustToBounds(glm::vec3 min, glm::vec3 max)
{
	this->position = glm::vec3((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2);
	viewMin = glm::vec3(this->position.x + this->aim.x * -3, this->position.y + this->aim.y * -3, this->position.z + this->aim.z * 5);
	viewMax = glm::vec3(this->position.x + this->aim.x * 3, this->position.y + this->aim.y * 3, this->position.z + this->aim.z * 5);

	ray r1 = ray(this->position, min - this->position);
	ray r2 = ray(this->position, glm::vec3(min.x, min.y, max.z) - this->position);
	ray r3 = ray(this->position, glm::vec3(min.x, max.y, min.z) - this->position);
	ray r4 = ray(this->position, glm::vec3(min.x, max.y, max.z) - this->position);
	ray r5 = ray(this->position, glm::vec3(max.x, min.y, min.z) - this->position);
	ray r6 = ray(this->position, glm::vec3(max.x, min.y, max.z) - this->position);
	ray r7 = ray(this->position, glm::vec3(max.x, max.y, min.z) - this->position);
	ray r8 = ray(this->position, max - this->position);

	while (!(!contains(min,max,this->position) && r1.intersectPlane(viewMin, viewMax) && r2.intersectPlane(viewMin, viewMax) && r3.intersectPlane(viewMin, viewMax) && r4.intersectPlane(viewMin, viewMax) &&
		   r5.intersectPlane(viewMin, viewMax) && r6.intersectPlane(viewMin, viewMax) && r7.intersectPlane(viewMin, viewMax) && r8.intersectPlane(viewMin, viewMax)))
	{
		this->position.z += 6;
		viewMin = glm::vec3(this->position.x + this->aim.x * -3, this->position.y + this->aim.y * -3, this->position.z + this->aim.z * 5);
		viewMax = glm::vec3(this->position.x + this->aim.x * 3, this->position.y + this->aim.y * 3, this->position.z + this->aim.z * 5);

		r1 = ray(this->position, min - this->position);
		r2 = ray(this->position, glm::vec3(min.x, min.y, max.z) - this->position);
		r3 = ray(this->position, glm::vec3(min.x, max.y, min.z) - this->position);
		r4 = ray(this->position, glm::vec3(min.x, max.y, max.z) - this->position);
		r5 = ray(this->position, glm::vec3(max.x, min.y, min.z) - this->position);
		r6 = ray(this->position, glm::vec3(max.x, min.y, max.z) - this->position);
		r7 = ray(this->position, glm::vec3(max.x, max.y, min.z) - this->position);
		r8 = ray(this->position, max - this->position);
	}
	std::cout << "position of camera: " << glm::to_string(this->position) << std::endl;
}

void PointCam::renderImage(Scene* scene, FIBITMAP* image, std::vector<glm::vec3>* points, int rot)
{
	Mesh* meshToRender = scene->getRotatedMesh(0, rot);

	adjustToBounds(meshToRender->min, meshToRender->max);
	int imageWidth = FreeImage_GetWidth(image);
	int imageHeight = FreeImage_GetHeight(image);
	std::cout << "num verts: " << meshToRender->verts.size() << std::endl;
	std::cout << "num faces: " << meshToRender->faces.size() << std::endl;
	std::cout << "rendering with resolution: " << imageWidth << " x " << imageHeight << std::endl;

	RGBQUAD black;
	black.rgbRed = 0;
	black.rgbGreen = 0;
	black.rgbBlue = 0;

	RGBQUAD white;
	white.rgbRed = 255;
	white.rgbGreen = 255;
	white.rgbBlue = 255;

	int currentI = 0;
	int currentJ = 0;

	std::vector<std::thread> threads;
	for (int t = 0; t < std::min(MAX_THREADS, (int)std::thread::hardware_concurrency()); t++)
	{
		threads.push_back(std::thread([&]()
			{
				bool notEmpty = currentI != imageWidth && currentJ != imageHeight;
				while (notEmpty)
				{
					indexLock.lock();
					int startI = currentI;
					int startJ = currentJ;
					int endI = std::min(startI + TILE_SIZE, imageWidth);
					int endJ = std::min(startJ + TILE_SIZE, imageHeight);
					if (endJ == imageHeight && endI == imageWidth)
					{
						currentI = endI;
						currentJ = endJ;
					}
					else if (endJ == imageHeight)
					{
						currentI = endI;
						currentJ = 0;
					}
					else
					{
						currentJ = endJ;
					}
					indexLock.unlock();

					for (int i = startI; i < endI; i++)
					{
						for (int j = startJ; j < endJ; j++)
						{
							float u = (float)(i) / imageWidth;
							float v = (float)(j) / imageHeight;

							ray raycast = getRay(u, v);

							if (raycast.intersectCube(meshToRender->min, meshToRender->max))
							{
								glm::vec3 bary = glm::vec3();
								if (raycast.intersectMesh(meshToRender, &bary))
								{
									bary = glm::inverse(scene->rotations.at(rot)) * glm::vec4(bary, 1);

									pushLock.lock();
									points->push_back(bary);
									pushLock.unlock();

									FreeImage_SetPixelColor(image, i, j, &white);
								}
								else
									FreeImage_SetPixelColor(image, i, j, &black);
							}
							else
							{
								FreeImage_SetPixelColor(image, i, j, &black);
							}
						}
					}

				}
			}));
	}



	
	std::cout << std::endl;
}

bool ray::intersectTriangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3* bary)
{
	glm::vec2 tempBary;
	float f;
	bool i = glm::intersectRayTriangle(this->point, glm::normalize(this->dir), v0, v1, v2, tempBary, f);
	bary->x = tempBary.x;
	bary->y = tempBary.y;
	bary->z = f;
	return i;
}

bool ray::intersectPlane(glm::vec3 min, glm::vec3 max)
{
	glm::vec3 m1 = glm::vec3(min.x, max.y, min.z);
	glm::vec3 m2 = glm::vec3(max.x, min.y, min.z);
	glm::vec2 bary;
	float f;
	return glm::intersectRayTriangle(this->point, glm::normalize(this->dir), min, m1, m2, bary, f) || glm::intersectRayTriangle(this->point, glm::normalize(this->dir), max, m1, m2, bary, f);
}

bool ray::intersectCube(glm::vec3 min, glm::vec3 max)
{
	/*
		//https://medium.com/@bromanz/another-view-on-the-classic-ray-aabb-intersection-algorithm-for-bvh-traversal-41125138b525
		glm::vec3 invD = glm::normalize(this->dir);
		invD.x = 1 / invD.x;
		invD.y = 1 / invD.y;
		invD.z = 1 / invD.z;

		glm::vec3 t0s = (min - this->point) * invD;
		glm::vec3 t1s = (max - this->point) * invD;

		glm::vec3 tsmaller = std::min(t0s, t1s);
		glm::vec3 tbigger = std::max(t0s, t1s);

		bary->x = std::max(bary->x, std::max(tsmaller[0], std::max(tsmaller[1], tsmaller[2])));
		bary->y = std::min(bary->y, std::min(tbigger[0], std::min(tbigger[1], tbigger[2])));
		*/
	glm::vec3 params[2] = { min, max };
	glm::vec3 invDir = glm::vec3(1 / this->dir.x, 1 / this->dir.y, 1 / this->dir.z);
	int xSign = invDir.x < 0;
	int ySign = invDir.y < 0;
	int zSign = invDir.z < 0;

	float tXMin = (params[xSign].x - this->point.x) * invDir.x;
	float tXMax = (params[1 - xSign].x - this->point.x) * invDir.x;
	float tYMin = (params[ySign].y - this->point.y) * invDir.y;
	float tYMax = (params[1 - ySign].y - this->point.y) * invDir.y;
	if (tXMin > tYMax || tYMin > tXMax)
		return false;
	if (tYMin > tXMin)
		tXMin = tYMin;
	if (tYMax < tXMax)
		tXMax = tYMax;
	float tZMin = (params[zSign].z - this->point.z) * invDir.z;
	float tZMax = (params[1 - zSign].z - this->point.z) * invDir.z;
	if ((tXMin > tZMax) || (tZMin > tXMax))
		return false;
	if (tZMin > tXMin)
		tXMin = tZMin;
	if (tZMax < tXMax)
		tXMax = tZMax;
	return ((tXMin < 1000) && (tXMax > -1000));
}

bool ray::intersectMesh(Mesh* m, glm::vec3* bary)
{
	float dist = std::numeric_limits<float>::max();
	glm::vec3 baryToReturn = glm::vec3();
	for (int i = 0; i < m->faces.size(); i++)
	{
		glm::vec3 baryTemp = glm::vec3();
		if (this->intersectTriangle(m->verts.at(m->faces.at(i).x), m->verts.at(m->faces.at(i).y), m->verts.at(m->faces.at(i).z), &baryTemp))
		{
			if (baryTemp.z < dist)
			{
				dist = baryTemp.z;
				baryToReturn = this->point + this->dir * dist;
			}
		}
	}

	if (dist < std::numeric_limits<float>::max())
	{
		bary->x = baryToReturn.x;
		bary->y = baryToReturn.y;
		bary->z = baryToReturn.z;
		return true;
	}

	return false;

}
