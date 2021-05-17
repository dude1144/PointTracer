#include "Scene.h"

void Scene::readfile(std::string path)
{
	std::string line;
	std::fstream file;
	std::string currentName;

	std::cout << "opening file: " + path << std::endl;
	file.open(path, std::fstream::in);
	std::cout << file.is_open() << std::endl;

	if (file.is_open())
	{
		std::cout << "file opened" << std::endl;
		Mesh* m = new Mesh();
		std::string currentname = "";
		int linenum = 0;
		while (std::getline(file, line))
		{
			linenum++;
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
				m->verts.push_back(glm::vec3(stof(arguments.at(1)), stof(arguments.at(2)), stof(arguments.at(3))));
				m->names.push_back(currentname);

				//update the bounding box of the mesh
				m->max.x = std::max(m->max.x, stof(arguments.at(1)));
				m->min.x = std::min(m->min.x, stof(arguments.at(1)));
				m->max.y = std::max(m->max.y, stof(arguments.at(2)));
				m->min.y = std::min(m->min.y, stof(arguments.at(2)));
				m->max.z = std::max(m->max.z, stof(arguments.at(3)));
				m->min.z = std::min(m->min.z, stof(arguments.at(3)));
			}
			else if (arguments.at(0) == "vt")
			{
				m->uv.push_back(glm::vec2(stof(arguments.at(1)), stof(arguments.at(2))));
			}
			else if (arguments.at(0) == "vn")
			{
				m->normals.push_back(glm::vec3(stof(arguments.at(1)), stof(arguments.at(2)), stof(arguments.at(3))));
			}
			else if (arguments.at(0) == "f")
			{
				if (arguments.size() == 4)
					m->faces.push_back(glm::vec3(stoi(arguments.at(1)) - 1, stoi(arguments.at(2)) - 1, stoi(arguments.at(3)) - 1));
				else if (arguments.size() == 7)
					m->faces.push_back(glm::vec3(stoi(arguments.at(1)) - 1, stoi(arguments.at(3)) - 1, stoi(arguments.at(5)) - 1));
				else
					m->faces.push_back(glm::vec3(stoi(arguments.at(1)) - 1, stoi(arguments.at(4)) - 1, stoi(arguments.at(7)) - 1));
			}
			else if (arguments.at(0) == "o")
			{
				currentname = arguments.at(1);
			}
		}
				

		std::cout << "bounds max: " + glm::to_string(m->max) << std::endl;
		std::cout << "bounds min: " + glm::to_string(m->min) << std::endl;

		this->meshes.push_back(m);
	}
	else
	{
		std::cout << "failed to open file: " + path << std::endl;
		file.close();
		exit(1);
	}
	file.close();

	//std::cout << "num vertices: " << this->meshes.at(0)->verts.size() << std::endl;
	//std::cout << "num faces: " << this->meshes.at(0)->faces.size() << std::endl;
}

Mesh* Scene::getRotatedMesh(int mesh, int rot)
{
	Mesh* m = new Mesh();
	Mesh* meshToRotate = this->meshes.at(mesh);
	for (int i = 0; i < meshToRotate->verts.size(); i++)
	{
		m->verts.push_back(this->rotations.at(rot) * glm::vec4(meshToRotate->verts.at(i), 1));

		m->max.x = std::max(m->max.x, m->verts.at(i).x);
		m->min.x = std::min(m->min.x, m->verts.at(i).x);
		m->max.y = std::max(m->max.y, m->verts.at(i).y);
		m->min.y = std::min(m->min.y, m->verts.at(i).y);
		m->max.z = std::max(m->max.z, m->verts.at(i).z);
		m->min.z = std::min(m->min.z, m->verts.at(i).z);
	}

	m->uv = meshToRotate->uv;
	m->normals = meshToRotate->normals;
	m->faces = meshToRotate->faces;
	m->names = meshToRotate->names;

	return m;
}
