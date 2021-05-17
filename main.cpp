#pragma once

#include <iostream>
#include <string>
#include "FreeImage.h"

#include "Scene.h"
#include "pointCam.h"

void cloudToImage(Scene* scene, std::vector<glm::vec3>* points, std::string outpathname);

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		Scene scene;
		PointCam cam = PointCam();
		std::vector<glm::vec3> points;
		std::string pathname = "";
		std::string outpathname = "";
		int res = 800;
		int rotRes = 8;
		bool useRotations = true;
		bool asImage = false;

		if (std::string(argv[1]) == "--help")
		{
			std::cout << "-f [path]: path to the 3D Model to recreate" << std::endl;
			std::cout << "-o [path]: path to output the results" << std::endl;
			std::cout << "-r [int]: set the resolution to render at" << std::endl;
			std::cout << "-rr [int]: set the amount to rotate each rotation, should be less than 360" << std::endl;
			std::cout << "--norot: only render a single image from straight on, without rotations" << std::endl;
			std::cout << "--asimage: save the point cloud as an image in addition to an OBJ file" << std::endl;
			return 0;
		}

		//read command line arguments
		for (int i = 1; i < argc; i++)
		{
			//get the current arguments as a string
			std::string s = std::string(argv[i]);

			//if the argument is to declare a file path
			if (s == "-f")
			{
				if (i + 1 >= argc || std::string(argv[i+1]) == "-r" || std::string(argv[i + 1]) == "-f" || std::string(argv[i + 1]) == "-o" || std::string(argv[i + 1]) == "-rr" || std::string(argv[i + 1]) == "--norot" || std::string(argv[i + 1]) == "--asimage")
				{
					std::cout << "invalid path declaration" << std::endl;
					return 0;
				}

				std::string cstr = std::string(argv[++i]);
				
				pathname = cstr;
			}

			if (s == "-o")
			{
				if (i + 1 >= argc || std::string(argv[i + 1]) == "-r" || std::string(argv[i + 1]) == "-f" || std::string(argv[i + 1]) == "-o" || std::string(argv[i + 1]) == "-rr" || std::string(argv[i + 1]) == "--norot" || std::string(argv[i + 1]) == "--asimage")
				{
					std::cout << "invalid output path declaration" << std::endl;
					return 0;
				}

				std::string cstr = std::string(argv[++i]);

				outpathname = cstr;
			}

			//if the arguments is to set the resolution
			else if (s == "-r")
			{
				if (i + 1 >= argc || std::string(argv[i + 1]) == "-r" || std::string(argv[i + 1]) == "-f" || std::string(argv[i + 1]) == "-o" || std::string(argv[i + 1]) == "-rr" || std::string(argv[i + 1]) == "--norot" || std::string(argv[i + 1]) == "--asimage")
				{
					std::cout << "invalid resolution declaration" << std::endl;
					return 0;
				}

				//check that the argument is an int
				for (char& c : std::string(argv[i+1]))
					if (!std::isdigit(c))
					{
						std::cout << "invalid resolution declaration" << std::endl;
						return 0;
					}

				res = atoi(argv[++i]);
			}

			//if the argument is to set the rotation resolution
			else if (s == "-rr")
			{
				if (i + 1 >= argc || std::string(argv[i + 1]) == "-r" || std::string(argv[i + 1]) == "-f" || std::string(argv[i + 1]) == "-o" || std::string(argv[i + 1]) == "-rr" || std::string(argv[i + 1]) == "--norot" || std::string(argv[i + 1]) == "--asimage")
				{
					std::cout << "invalid rotation resolution declaration" << std::endl;
					return 0;
				}

				//check that the argument is an int
				for (char& c : std::string(argv[i + 1]))
					if (!std::isdigit(c))
					{
						std::cout << "invalid resolution declaration" << std::endl;
						return 0;
					}

				rotRes = atoi(argv[++i]);
			}
			else if (s == "--norot")
			{
				useRotations = false;
			}
			else if (s == "--asimage")
			{
				asImage = true;
			}
		}

		//if no path was specified
		if(pathname == "")
		{
			std::cout << "no path declared" << std::endl;
			return 0;
		}


		FreeImage_Initialise(false);
		std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

		FIBITMAP* bitmap = FreeImage_Allocate(res, res, 24);
		if (!bitmap)
		{
			std::cout << "failed to allocate bitmap" << std::endl;
			return 0;
		}
		else
			std::cout << "successfully allocated bitmap" << std::endl;

		scene = Scene(rotRes);
		scene.readfile(pathname);

		if (useRotations)
		{
			for (int i = 0; i < scene.rotations.size(); i++)
			{
				cam.renderImage(&scene, bitmap, &points, i);
				std::string name = std::string("render" + scene.rotNames.at(i) + ".png");
				if (FreeImage_Save(FIF_PNG, bitmap, (outpathname + name).c_str(), 0))
					std::cout << "image saved" << std::endl;
			}
		}
		else
		{
			cam.renderImage(&scene, bitmap, &points);
			std::string name = std::string("renderNOROT.png");
			if (FreeImage_Save(FIF_PNG, bitmap, (outpathname + name).c_str(), 0))
				std::cout << "image saved" << std::endl;
		}

		std::fstream file;
		file.open(outpathname + "points.obj", std::fstream::out);
		if (file.is_open())
		{
			for(int i = 0; i < points.size(); i++)
				file << "v " << points.at(i).x << " " << points.at(i).y << " " << points.at(i).z << std::endl;
		}
		if (asImage)
		{
			cloudToImage(&scene, &points, outpathname);
		}

		float time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count() / 1000000000.0f;
		std::cout << "total time: " << time << std::endl;
		file.close();
		
		FreeImage_DeInitialise();
	}
	else
	{
		std::cout << "no arguments" << std::endl;
		return 0;
	}

	return 0;
}


void cloudToImage(Scene* scene, std::vector<glm::vec3>* points, std::string outpathname)
{
	std::cout << "saving as image" << std::endl;
	float xSize = scene->meshes[0]->max.x - scene->meshes[0]->min.x;
	float ySize = scene->meshes[0]->max.y - scene->meshes[0]->min.y;
	float zSize = scene->meshes[0]->max.z - scene->meshes[0]->min.z;
	FIBITMAP* bitmap = FreeImage_AllocateT(FIT_FLOAT, 1024, (ySize/xSize)*1024, 32);
	if (!bitmap)
	{
		std::cout << "failed to allocate bitmap" << std::endl;
		return;
	}
	else
		std::cout << "successfully allocated bitmap" << std::endl;

	int width = FreeImage_GetWidth(bitmap);
	int height = FreeImage_GetHeight(bitmap);
	std::cout << std::to_string(width) + "," + std::to_string(height) << std::endl;

	
	std::cout << xSize << ", " << ySize << ", " << zSize << std::endl;

	for (int i = 0; i < points->size(); i++)
	{
		int x = (int)( (1 - (std::abs(points->at(i).x - scene->meshes[0]->min.x)/xSize) ) * width);
		int y = (int)((std::abs(points->at(i).y - scene->meshes[0]->min.y) / ySize) * height);
		
		float* scanline = (float*)FreeImage_GetScanLine(bitmap, y);
		scanline[x] = points->at(i).z - scene->meshes[0]->min.z;
	}
	std::string name = "(" + std::to_string(scene->meshes[0]->min.x) + "," + std::to_string(scene->meshes[0]->min.y) + "," + std::to_string(scene->meshes[0]->min.z) + ")" + "(" + std::to_string(scene->meshes[0]->max.x) + "," + std::to_string(scene->meshes[0]->max.y) + "," + std::to_string(scene->meshes[0]->max.z) + ")";
	std::cout << "saving as: " + name << std::endl;
	if (FreeImage_Save(FIF_TIFF, bitmap, (outpathname + name + ".tiff").c_str(), TIFF_ADOBE_DEFLATE))
		std::cout << "image saved" << std::endl;
	else
		std::cout << "failed to save" << std::endl;
}