#include "MainApp.h"
//#include "SponzaApp.h"
#include "ComparisonApp.h"
#include "TeapotApp.h"
#include <iostream>

App* app;


int main(int argc, char** argv){

	app = new ComparisonApp();

	if (!app->Init()){
		std::cout << "init failed!" << endl;
		return 1;
	}

	app->Run();

	app->ShutDown();

	delete app;

	return 0;
}

void GLFWCALL MyResize(int width, int height){
	float aspect = (float)width / (float)height;

	app->Resize(aspect,width,height);
}





