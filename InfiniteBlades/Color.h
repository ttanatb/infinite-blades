#pragma once
#include <DirectXMath.h>
#include <iostream>
#define vec4 DirectX::XMFLOAT4
#define vec3 DirectX::XMFLOAT3
#define vec2 DirectX::XMFLOAT2
#define mat4 DirectX::XMFLOAT4X4

//Wraps a vector4 and deals with color
struct Color
{
private:
	vec4 color;		//actual color

	//Makes sure the value is between 0 and 1
	float validateColor(float c) {
		if (c > 1.0f) return 1.0f;
		else if (c < 0.0f) return 0.0f;
		else return c;
	}

public:
	//Creates a new color from a directX vec4
	Color(vec4 c) {
		color = vec4(validateColor(c.x),
			validateColor(c.y),
			validateColor(c.z),
			validateColor(c.w));
	}

	//Creates a color based off of rgba values
	Color(float r, float g, float b, float a = 1.0f)
		: Color(vec4(r, g, b, a)) { }

	//the vec4 form of color
	vec4 GetVec4() const { return color; }

	//Value of red (0.0 - 1.0)
	float GetR() const { return color.x; }
	//Value of green (0.0 - 1.0)
	float GetG() const { return color.y; }
	//Value of blue (0.0 - 1.0)
	float GetB() const { return color.z; }
	//Alpha value (0.0 - 1.0)
	float GetA() const { return color.w; }

	//Value of red (0.0 - 1.0)
	void SetR(float c) { color.x = validateColor(c); }
	//Value of green (0.0 - 1.0)
	void SetG(float c) { color.y = validateColor(c); }
	//Value of blue (0.0 - 1.0)
	void SetB(float c) { color.z = validateColor(c); }
	//Alpha value (0.0 - 1.0)
	void SetA(float c) { color.w = validateColor(c); }

	//255, 0, 0
	static Color Red()    { return Color(1.0f, 0.0f, 0.0f); }
	//0, 255, 0
	static Color Green()  { return Color(0.0f, 1.0f, 0.0f); }
	//0, 0, 255
	static Color Blue()   { return Color(0.0f, 0.0f, 1.0f); }
	//255, 105, 180
	static Color Pink()   { return Color(1.0f, 0.412f, 0.706f); }
	//128, 0, 128
	static Color Purple() { return Color(0.5f, 0.0f, 0.5f); }
	//0, 255, 255
	static Color Cyan()   { return Color(0.0f, 1.0f, 1.0f); }
	//255, 255, 0
	static Color Yellow() { return Color(1.0f, 1.0f, 0.0f); }
	//255, 165, 0
	static Color Orange() { return Color(1.0f, 0.647f, 0.0f); }

	//255, 255, 255
	static Color White()  { return Color(1.0f, 1.0f, 1.0f); }
	//128, 128, 128
	static Color Gray()   { return Color(0.5f, 0.5f, 0.5f); }
	//0, 0, 0
	static Color Black()  { return Color(0.0f, 0.0f, 0.0f); }

};

//std::ostream& operator<<(std::ostream& os, const Color& c) {
//	os << "(" << c.GetR() << ", " << c.GetG() << ", " << c.GetB() << ", " << c.GetA() << ")";
//	return os;
//}