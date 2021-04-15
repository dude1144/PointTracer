#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
	if (argc > 1)
	{

		std::string pathname = "";
		int res = 800;

		//read command line arguments
		for (int i = 1; i < argc; i++)
		{
			//get the current arguments as a string
			std::string s = std::string(argv[i]);

			//if the argument is to declare a file path
			if (s == "-f")
			{
				if (i + 1 >= argc || std::string(argv[i+1]) == "-r" || std::string(argv[i + 1]) == "-f")
				{
					std::cout << "invalid path declaration" << std::endl;
					return 0;
				}
				pathname = std::string(argv[++i]);
			}

			//if the arguments is to set the resolution
			else if (s == "-r")
			{
				if (i + 1 >= argc || std::string(argv[i + 1]) == "-r" || std::string(argv[i + 1]) == "-f")
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

		}

		//if no path was specified
		if(pathname == "")
		{
			std::cout << "no path declared" << std::endl;
			return 0;
		}

		//create the point cloud

		//load in model
		//create render cam
		//render

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