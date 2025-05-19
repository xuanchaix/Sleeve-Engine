#include "Game/Frameworks/App.h"
#include "Engine/Core/EngineCommon.h"
#include "Game/Frameworks/GameCommon.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>


int main( int argc, char* argv[] )
{
	g_theApp = new App;
	g_theApp->Initialize();
	g_theApp->Run();
	g_theApp->Exit();

	return 0;
}
