#include "MainApp.h"
//#include "ShadowApp.h"
#include "DeferredApp.h"
#include <iostream>

App* app;

int main(int argc, char** argv){

	app = new DeferredApp();

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





