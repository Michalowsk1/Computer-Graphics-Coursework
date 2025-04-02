#pragma once

#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

class Quaternion
{
public:
	float w, x, y, z;

	//constructor
	Quaternion();
	Quaternion(const float w, const float x, const float y, const float z);

	mat4 matrix();

	Quaternion(const float pitch, const float yaw);
};

//Maths class
class Maths
{
public:
	//transformation matrices
	static mat4 translate(const vec3& v);

	static mat4 scale(const vec3& v);

	static float radians(float angle);

	static mat4 rotate(const float& angle, vec3 v);

	static mat4 view(const mat4 translate, const mat4 rotate);

	static Quaternion SLERP(const Quaternion q1, const Quaternion q2, const float t);
};

