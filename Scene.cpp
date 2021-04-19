#include "Scene.h"

void Scene::readfile(std::string path)
{
	std::string line;
	std::fstream file;
	std::string currentName;

	file.open(path, std::ios::in);

	if (file.is_open())
	{
		Mesh* m = new Mesh();
		std::string* currentname = new std::string("");
		while (std::getline(file, line))
		{
			//split the string along spaces
			std::vector<std::string> arguments;
			size_t current = 0;
			size_t end =  std::min(line.find(" "), line.find("/"));

			while (end != -1)
			{
				std::string newstr = line.substr(current, end - current);
				arguments.push_back(newstr);
				current = end + 1;
				end = std::min(line.find(" ", current), line.find("/", current));
			}
			arguments.push_back(line.substr(current, line.length() - current));

			if (arguments.at(0) == "v")
			{
				m->verts.push_back(new glm::vec3(stof(arguments.at(1)), stof(arguments.at(2)), stof(arguments.at(3))));
				m->names.push_back(currentname);

				//update the bounding box of the mesh
				m->maxX = std::max(m->maxX, stof(arguments.at(1)));
				m->minX = std::min(m->minX, stof(arguments.at(1)));
				m->maxY = std::max(m->maxY, stof(arguments.at(2)));
				m->minY = std::min(m->minY, stof(arguments.at(2)));
				m->maxZ = std::max(m->maxZ, stof(arguments.at(3)));
				m->minZ = std::min(m->minZ, stof(arguments.at(3)));
			}
			else if (arguments.at(0) == "vt")
			{
				m->uv.push_back(new glm::vec2(stof(arguments.at(1)), stof(arguments.at(2))));
			}
			else if (arguments.at(0) == "vn")
			{
				m->normals.push_back(new glm::vec3(stof(arguments.at(1)), stof(arguments.at(2)), stof(arguments.at(3))));
			}
			else if (arguments.at(0) == "f")
			{
				m->faces.push_back(new glm::vec3(stof(arguments.at(1)) - 1, stof(arguments.at(2)) - 1, stof(arguments.at(3)) - 1));
			}
			else if (arguments.at(0) == "o")
			{
				delete currentname;
				currentname = new std::string(arguments.at(1));
			}
		}

		std::cout << m->maxX << std::endl;
		std::cout << m->maxY << std::endl;
		std::cout << m->maxZ << std::endl;
		std::cout << std::endl;
		std::cout << m->minX << std::endl;
		std::cout << m->minY << std::endl;
		std::cout << m->minZ << std::endl;

		this->meshes.push_back(m);
	}
}
