#include "MainApp.h"
#include <iostream>

int main(int argc, char** argv){

	App app;

	if (!app.Init()){
		std::cout << "init failed!" << endl;
		return 1;
	}

	app.Run();

	app.ShutDown();

	return 0;
}




