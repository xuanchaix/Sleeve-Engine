#include "Core/XmlUtils.h"
#include "Core/StringUtils.h"

int ParseXmlAttribute( XmlElement const* element, char const* attributeName, int defaultValue )
{
	return element->IntAttribute( attributeName, defaultValue );
}

unsigned int ParseXmlAttribute( XmlElement const* element, char const* attributeName, unsigned int defaultValue )
{
	return element->UnsignedAttribute( attributeName, defaultValue );
}

char ParseXmlAttribute( XmlElement const* element, char const* attributeName, char defaultValue )
{
	char const* attrValue = element->Attribute( attributeName );
	if (attrValue) {
		return attrValue[0];
	}
	return defaultValue;
}

bool ParseXmlAttribute( XmlElement const* element, char const* attributeName, bool defaultValue )
{
	return element->BoolAttribute( attributeName, defaultValue );
}

float ParseXmlAttribute( XmlElement const* element, char const* attributeName, float defaultValue )
{
	return element->FloatAttribute( attributeName, defaultValue );
}

Rgba8 ParseXmlAttribute( XmlElement const* element, char const* attributeName, Rgba8 const& defaultValue )
{
	char const* attrValue = element->Attribute( attributeName );
	if (attrValue) {
		Rgba8 rgba8;
		Strings strs;
		strs.reserve( 4 );
		int numOfStrings = SplitStringOnDelimiter( strs, attrValue, ',' );
		if (numOfStrings == 3) {
			rgba8.r = (unsigned char)stoi( strs[0] );
			rgba8.g = (unsigned char)stoi( strs[1] );
			rgba8.b = (unsigned char)stoi( strs[2] );
			rgba8.a = 255;
		}
		else if (numOfStrings == 4) {
			rgba8.r = (unsigned char)stoi( strs[0] );
			rgba8.g = (unsigned char)stoi( strs[1] );
			rgba8.b = (unsigned char)stoi( strs[2] );
			rgba8.a = (unsigned char)stoi( strs[3] );
		}
		else {
			return defaultValue;
		}
		return rgba8;
	}
	return defaultValue;
}

Vec2 ParseXmlAttribute( XmlElement const* element, char const* attributeName, Vec2 const& defaultValue )
{
	char const* attrValue = element->Attribute( attributeName );
	if (attrValue) {
		Vec2 vec2;
		Strings strs;
		strs.reserve( 4 );
		int numOfStrings = SplitStringOnDelimiter( strs, attrValue, ',' );
		if (numOfStrings != 2) {
			return defaultValue;
		}
		vec2.x = (float)atof( strs[0].c_str() );
		vec2.y = (float)atof( strs[1].c_str() );
		return vec2;
	}
	return defaultValue;
}

Vec3 ParseXmlAttribute( XmlElement const* element, char const* attributeName, Vec3 const& defaultValue )
{
	char const* attrValue = element->Attribute( attributeName );
	if (attrValue) {
		Vec3 vec3;
		Strings strs;
		strs.reserve( 4 );
		int numOfStrings = SplitStringOnDelimiter( strs, attrValue, ',' );
		if (numOfStrings != 3) {
			return defaultValue;
		}
		vec3.x = (float)atof( strs[0].c_str() );
		vec3.y = (float)atof( strs[1].c_str() );
		vec3.z = (float)atof( strs[2].c_str() );
		return vec3;
	}
	return defaultValue;
}

Vec4 ParseXmlAttribute( XmlElement const* element, char const* attributeName, Vec4 const& defaultValue )
{
	char const* attrValue = element->Attribute( attributeName );
	if (attrValue) {
		Vec4 vec4;
		Strings strs;
		strs.reserve( 4 );
		int numOfStrings = SplitStringOnDelimiter( strs, attrValue, ',' );
		if (numOfStrings != 4) {
			return defaultValue;
		}
		vec4.x = (float)atof( strs[0].c_str() );
		vec4.y = (float)atof( strs[1].c_str() );
		vec4.z = (float)atof( strs[2].c_str() );
		vec4.w = (float)atof( strs[3].c_str() );
		return vec4;
	}
	return defaultValue;
}

std::string ParseXmlAttribute( XmlElement const* element, char const* attributeName, std::string const& defaultValue )
{
	char const* attrValue = element->Attribute( attributeName );
	if (attrValue) {
		return std::string( attrValue );
	}
	return defaultValue;
}

std::string ParseXmlAttribute( XmlElement const* element, char const* attributeName, char const* defaultValue )
{
	char const* attrValue = element->Attribute( attributeName );
	if (attrValue) {
		return std::string( attrValue );
	}
	return std::string( defaultValue );
}

Euler ParseXmlAttribute( XmlElement const* element, char const* attributeName, Euler const& defaultValue )
{
	char const* attrValue = element->Attribute( attributeName );
	if (attrValue) {
		Euler orientation;
		Strings strs;
		strs.reserve( 4 );
		int numOfStrings = SplitStringOnDelimiter( strs, attrValue, ',' );
		if (numOfStrings != 3) {
			return defaultValue;
		}
		orientation.yaw = (float)atof( strs[0].c_str() );
		orientation.pitch = (float)atof( strs[1].c_str() );
		orientation.roll = (float)atof( strs[2].c_str() );
		return orientation;
	}
	return defaultValue;
}
