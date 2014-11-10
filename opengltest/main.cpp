#include "MainApp.h"
#include "SponzaApp.h"
#include <iostream>

int main(int argc, char** argv){

	App* app = new SponzaApp();

	if (!app->Init()){
		std::cout << "init failed!" << endl;
		return 1;
	}

	app->Run();

	app->ShutDown();

	delete app;

	return 0;
}




