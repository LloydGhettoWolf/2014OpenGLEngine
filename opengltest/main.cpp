#include "MainApp.h"
//#include "ShadowApp.h"
#include "DeferredSponza.h"
//#include "DeferredApp.h"
#include <iostream>

App* app;

int main(int argc, char** argv){

	app = new DeferredSponza();

	if (!app->Init()){
		std::cout << "init failed!" << endl;
		return 1;
	}

	app->Run();

	app->ShutDown();

	delete app;

	return 0;
}

void GLFWCALL MyResize(float fov,int width, int height,float nearZ,float farZ){
	float aspect = (float)width / (float)height;

	app->Resize(fov,aspect,width,height,nearZ,farZ);
}





