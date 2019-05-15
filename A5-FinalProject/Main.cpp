// Winter 2019

#include "Final.hpp"

#include <iostream>
using namespace std;

int main( int argc, char **argv ) 
{
	if (argc > 1) {
		std::string luaSceneFile(argv[1]);
		std::string title("** Final Project - [");
		title += luaSceneFile;
		title += "]";

		CS488Window::launch(argc, argv, new Final(luaSceneFile), 1024, 768, title);

	} else {
		cout << "Must supply Lua file as First argument to program.\n";
        cout << "For example:\n";
        cout << "./Final Assets/simpleScene.lua\n";
	}

	return 0;
}
