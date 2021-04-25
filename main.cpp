#pragma once

#include <iostream>
#include <string>
#include "FreeImage.h"

#include "Scene.h"
#include "pointCam.h"

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		Scene scene;
		PointCam cam = PointCam();
		std::vector<glm::vec3> points;
		std::string pathname = "";
		int res = 800;
		int rotRes = 8;
		bool useRotations = true;

		//read command line arguments
		for (int i = 1; i < argc; i++)
		{
			//get the current arguments as a string
			std::string s = std::string(argv[i]);

			//if the argument is to declare a file path
			if (s == "-f")
			{
				if (i + 1 >= argc || std::string(argv[i+1]) == "-r" || std::string(argv[i + 1]) == "-f" || std::string(argv[i + 1]) == "-rr" || std::string(argv[i + 1]) == "--norot")
				{
					std::cout << "invalid path declaration" << std::endl;
					return 0;
				}
				pathname = std::string(argv[++i]);
			}

			//if the arguments is to set the resolution
			else if (s == "-r")
			{
				if (i + 1 >= argc || std::string(argv[i + 1]) == "-r" || std::string(argv[i + 1]) == "-f" || std::string(argv[i + 1]) == "-rr" || std::string(argv[i + 1]) == "--norot")
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
				if (i + 1 >= argc || std::string(argv[i + 1]) == "-r" || std::string(argv[i + 1]) == "-f" || std::string(argv[i + 1]) == "-rr" || std::string(argv[i + 1]) == "--norot")
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
		}

		//if no path was specified
		if(pathname == "")
		{
			std::cout << "no path declared" << std::endl;
			return 0;
		}


		FreeImage_Initialise(false);

		FIBITMAP* bitmap = FreeImage_Allocate(res, res, 24);
		if (!bitmap)
		{
			std::cout << "failed to allocate bitmap" << std::endl;
			return 0;
		}

		scene = Scene(rotRes);
		scene.readfile(pathname);

		if (useRotations)
		{
			for (int i = 0; i < scene.rotations.size(); i++)
			{
				cam.renderImage(&scene, bitmap, &points, i);
				std::string name = std::string("render" + scene.rotNames.at(i) + ".png");
				if (FreeImage_Save(FIF_PNG, bitmap, name.c_str(), 0))
					std::cout << "image saved" << std::endl;
			}
		}
		else
		{
			cam.renderImage(&scene, bitmap, &points);
			std::string name = std::string("renderNOROT.png");
			if (FreeImage_Save(FIF_PNG, bitmap, name.c_str(), 0))
				std::cout << "image saved" << std::endl;
		}

		std::fstream file;
		file.open("points.obj", std::fstream::out);
		if (file.is_open())
		{
			for(int i = 0; i < points.size(); i++)
				file << "v " << points.at(i).x << " " << points.at(i).y << " " << points.at(i).z << std::endl;
		}

		FreeImage_DeInitialise();
	}
	else
	{
		std::cout << "no arguments" << std::endl;
		return 0;
	}

	return 0;
}


void readCommandLine(int argc, char* argv[])
{

}
