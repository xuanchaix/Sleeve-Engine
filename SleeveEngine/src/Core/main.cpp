#include "Core/App.h"
#include "Core/EngineCommon.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

int main() {
	g_theApp = new App();

	try {
		g_theApp->Initialize();
		g_theApp->Run();
		g_theApp->Exit();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}