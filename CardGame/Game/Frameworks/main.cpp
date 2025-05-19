#include "Game/Frameworks/App.h"
#include "Engine/Core/EngineCommon.h"
#include "Game/Frameworks/GameCommon.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>


int main( int argc, char* argv[] )
{
	g_theApp = new App();

	try {
		g_theApp->Initialize();
		g_theApp->Run();
		g_theApp->Exit();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
