#pragma once

struct Rgba8 {
	Rgba8() = default;
	Rgba8( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255 ) 
	:r(red), g(green), b(blue), a(alpha) { }
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

};