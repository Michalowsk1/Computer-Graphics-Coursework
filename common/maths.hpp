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

	static vec3 MathsNormalize(const vec3 coords);

	static float MathsLength(const vec3& object1, const vec3& object2);

	static float MathsDot(const vec3& vec1, const vec3& vec2);

	static mat4 MathsPerspective(const float fov, const float aspect, const float near, const float far);

	static vec3 MathsCross(const vec3& vec1, const vec3& vec2);

	static mat4 MathsLookAt(const vec3 eye, const vec3 centre, const vec3 up);

	static Quaternion SLERP(const Quaternion q1, const Quaternion q2, const float t);
};

