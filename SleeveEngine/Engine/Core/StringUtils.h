#pragma once
#include <string>
#include <vector>


typedef std::vector<std::string> Strings;
typedef std::vector<std::wstring> WStrings;

int SplitStringOnDelimiter( Strings& resultStringVector, std::string const& originalString, char delimiterToSplitOn = ',', bool removeExtraSpace = false );