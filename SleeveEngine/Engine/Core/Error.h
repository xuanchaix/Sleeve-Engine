#pragma once
#include <string>

__declspec( noreturn ) void ThrowError( char const* filePath, char const* functionName, int lineNum, std::string const& errorMessage );

#ifdef NO_THROW_ERROR
#define THROW_ERROR(message)											\
{																		\
	(void)message;														\
}
#else
#define THROW_ERROR(message)											\
{																		\
	ThrowError( __FILE__,  __FUNCTION__, __LINE__, message );			\
}
#endif

#ifdef NO_THROW_ERROR
#define ASSERT_OR_ERROR(condition, message)								\
{																		\
	(void)condition; (void)message;										\
}
#else
#define ASSERT_OR_ERROR(condition, message)								\
{																		\
	if(!(condition)){													\
		ThrowError( __FILE__, __FUNCTION__, __LINE__, message );		\
	}																	\
}
#endif