#include "MainApp.h"
//#include "SponzaApp.h"
#include "TeapotApp.h"
#include <iostream>

int main(int argc, char** argv){

	App* app = new TeapotApp();

	if (!app->Init()){
		std::cout << "init failed!" << endl;
		return 1;
	}

	app->Run();

	app->ShutDown();

	delete app;

	return 0;
}




