#include "BreakthroughApp.h"

int main() {
	
	// allocation
	auto app = new BreakthroughApp();

	// initialise and loop
	app->run("AIE", 1280, 720, false);

	// deallocation
	delete app;

	return 0;
}