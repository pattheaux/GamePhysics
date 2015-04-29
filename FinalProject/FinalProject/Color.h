#ifndef COLOR_H
#define COLOR_H

class Color
{
public:
	Color();
	Color(float red, float green, float blue);
	Color(float red, float green, float blue, float alpha);

	float r;
	float g;
	float b;
	float a;
};

#endif