#include "Core/Error.h"
#include "ThirdParty/tinyfiledialogs/tinyfiledialogs.h"

__declspec( noreturn ) void ThrowError( char const* filePath, char const* functionName, int lineNum, std::string const& errorMessage )
{
	std::string integratedErrorMessage = "An error occurred in " + std::string( filePath ) + " in function " + std::string( functionName )
		+ " on line " + std::to_string( lineNum ) + ".\n";
	integratedErrorMessage.append( errorMessage );
	int res = tinyfd_messageBox( "Error", integratedErrorMessage.c_str(), "okcancel", "error", 1 );
	if (res == 1) {
		__debugbreak();
	}

	exit( 0 );
}
