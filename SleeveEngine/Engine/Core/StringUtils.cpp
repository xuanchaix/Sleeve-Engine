#include "Core/StringUtils.h"

int SplitStringOnDelimiter( Strings& resultStringVector, std::string const& originalString, char delimiterToSplitOn /*= ','*/, bool removeExtraSpace /*= false */ )
{
	resultStringVector.clear();

	size_t findRes;
	size_t firstPos = 0;
	do {
		findRes = originalString.find_first_of( delimiterToSplitOn, firstPos );
		std::string strSplited = originalString.substr( firstPos, findRes - firstPos );
		if (removeExtraSpace && findRes != std::string::npos) {
			size_t nextNoSpacePos = originalString.find_first_not_of( ' ', findRes + 1 );
			if (nextNoSpacePos != 0 && nextNoSpacePos != std::string::npos) {
				firstPos = nextNoSpacePos;
			}
			else if (nextNoSpacePos == std::string::npos) {
				findRes = std::string::npos;
			}
			else {
				firstPos = findRes + 1;
			}
			strSplited.erase( remove( strSplited.begin(), strSplited.end(), ' ' ), strSplited.end() );
		}
		else {
			firstPos = findRes + 1;
		}

		resultStringVector.push_back( strSplited );
	} while (findRes != std::string::npos);

	return (int)resultStringVector.size();
}
