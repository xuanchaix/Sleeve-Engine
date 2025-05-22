#pragma once
#include "ThirdParty/tinyxml2/tinyxml2.h"
#include "Math/MathFwd.h"
#include "Core/Color.h"
#include <string>

typedef tinyxml2::XMLDocument XmlDocument;
typedef tinyxml2::XMLElement XmlElement;
typedef tinyxml2::XMLAttribute XmlAttribute;
typedef tinyxml2::XMLError XmlError;

int ParseXmlAttribute( XmlElement const* element, char const* attributeName, int defaultValue );
unsigned int ParseXmlAttribute( XmlElement const* element, char const* attributeName, unsigned int defaultValue );
char ParseXmlAttribute( XmlElement const* element, char const* attributeName, char defaultValue );
bool ParseXmlAttribute( XmlElement const* element, char const* attributeName, bool defaultValue );
float ParseXmlAttribute( XmlElement const* element, char const* attributeName, float defaultValue );
Rgba8 ParseXmlAttribute( XmlElement const* element, char const* attributeName, Rgba8 const& defaultValue );
Vec2 ParseXmlAttribute( XmlElement const* element, char const* attributeName, Vec2 const& defaultValue );
Vec3 ParseXmlAttribute( XmlElement const* element, char const* attributeName, Vec3 const& defaultValue );
Vec4 ParseXmlAttribute( XmlElement const* element, char const* attributeName, Vec4 const& defaultValue );
//IntVec2 ParseXmlAttribute( XmlElement const* element, char const* attributeName, IntVec2 const& defaultValue );
std::string ParseXmlAttribute( XmlElement const* element, char const* attributeName, std::string const& defaultValue );
//Strings ParseXmlAttribute( XmlElement const* element, char const* attributeName, Strings const& defaultValues );
std::string ParseXmlAttribute( XmlElement const* element, char const* attributeName, char const* defaultValue );
Euler ParseXmlAttribute( XmlElement const* element, char const* attributeName, Euler const& defaultValue );
//FloatRange ParseXmlAttribute( XmlElement const* element, char const* attributeName, FloatRange defaultValue );