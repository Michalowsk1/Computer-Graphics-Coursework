
#include <common/maths.hpp>

mat4 Maths::translate(const vec3& v) //moving image
{
	mat4 translate(1.0f);
	translate[3][0] = v.x, translate[3][1] = v.y, translate[3][2] = v.z;
	return translate;
}

mat4 Maths::scale(const vec3& v) //inc-decreasing size
{
	mat4 scale(1.0f);
	scale[0][0] = v.x;; scale[1][1] = v.y; scale[2][2] = v.z;
	return scale;
}

float Maths::radians(float angle)
{
	return angle * 3.1416f / 180.0f;
}

mat4 Maths::rotate(const float& angle, vec3 v)
{
	v = normalize(v);
	float c = cos(0.5f * angle);
	float s = sin(0.5f * angle);
	Quaternion q(c, s * v.x, s * v.y, s * v.z);

	return q.matrix();
}

float Maths::MathsLength(const vec3& object1, const vec3& object2)
{
	vec3 vecLength = object2 - object1;

	float length = sqrt((vecLength.x * vecLength.x) + (vecLength.y * vecLength.y) + (vecLength.z * vecLength.z));

	return length;
}

float Maths::MathsDot(const vec3& vec1, const vec3& vec2)
{
	float x = vec1.x * vec2.x;
	float y = vec1.y * vec2.y;
	float z = vec1.z * vec2.z;

	float DotProduct = x + y + z;

	return DotProduct;
}

vec3 Maths::MathsCross(const vec3& vec1, const vec3& vec2)
{
	float x1 = vec1.x;
	float x2 = vec2.x;
	float y1 = vec1.y;
	float y2 = vec2.y;
	float z1 = vec1.z;
	float z2 = vec2.z;

	float a = (y1 * z2) - (z1 * y2);
	float b = (z1 * x2) - (x1 * z2);
	float c = (x1 * y2) - (y1 * x2);

	return vec3(a, b, c);
}

mat4 Maths::MathsLookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up) 
{
	vec3 Z = Maths::MathsNormalize(eye - center);
	vec3 X = Maths::MathsNormalize(Maths::MathsCross(Z, up));
	vec3 Y = Maths::MathsCross(X, Z);

	glm::mat4 result;
	result[0][0] = X.x; result[1][0] = X.y; result[2][0] = X.z;
	result[0][1] = Y.x; result[1][1] = Y.y; result[2][1] = Y.z;
	result[0][2] = -Z.x; result[1][2] = -Z.y; result[2][2] = -Z.z;
	result[3][0] = -Maths::MathsDot(Y, eye);
	result[3][1] = -Maths::MathsDot(Z, eye);
	result[3][2] = Maths::MathsDot(X, eye);
	return result;
}

mat4 Maths::MathsPerspective(const float fov, const float aspect, const float near, const float far)
{
	float top = near * (tan(fov / 2));

	float right = aspect * top; 

	float left = -right;

	float bottom = -top;

	mat4 result = {};

	result[0][0] = near / right;
	result[1][1] = near / top;
	result[2][2] = -(far + near) / (far - near);
	result[2][3] = -1.0f;
	result[3][2] = -(2 * far * near) / (far - near);
	result[3][3] = 0.0f;

	return result;
}

vec3 Maths::MathsNormalize(const vec3 coords)
{
	float denominator1 = (coords.x * coords.x) + (coords.y * coords.y) + (coords.z * coords.z);

	float denominator = sqrt(denominator1);

	return vec3((coords.x / denominator), (coords.y / denominator), (coords.z / denominator));

}

Quaternion::Quaternion() {}

Quaternion::Quaternion(const float w, const float x, const float y, const float z)
{
	this->w = w;
	this->x = x;
	this->y = y;
	this->z = z;
}

mat4 Quaternion::matrix()
{
	float s = 2.0f / (w * w + x * x + y * y + z * z);
	float xs = x * s, ys = y * s, zs = z * s;
	float xx = x * xs, xy = x * ys, xz = x * zs;
	float yy = y * ys, yz = y * zs, zz = z * zs;
	float xw = w * xs, yw = w * ys, zw = w * zs;

	glm::mat4 rotate;
	rotate[0][0] = 1.0f - (yy + zz);
	rotate[0][1] = xy + zw;
	rotate[0][2] = xz - yw;
	rotate[1][0] = xy - zw;
	rotate[1][1] = 1.0f - (xx + zz);
	rotate[1][2] = yz + xw;
	rotate[2][0] = xz + yw;
	rotate[2][1] = yz - xw;
	rotate[2][2] = 1.0f - (xx + yy);

	return rotate;
}

Quaternion::Quaternion(const float pitch, const float yaw)
{
	float cosPitch = cos(0.5f * pitch);
	float sinPitch = sin(0.5f * pitch);
	float cosYaw = cos(0.5f * yaw);
	float sinYaw = sin(0.5f * yaw);

	this->w = cosPitch * cosYaw;
	this->x = sinPitch * cosYaw;
	this->y = cosPitch * sinYaw;
	this->z = sinPitch * sinYaw;
}

Quaternion Maths::SLERP(Quaternion q1, Quaternion q2, const float t)
{
	// Calculate cos(theta)
	float cosTheta = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;

	// If q1 and q2 are close together return q2 to avoid divide by zero errors
	if (cosTheta > 0.9999f)
		return q2;

	// Avoid taking the long path around the sphere by reversing sign of q2
	if (cosTheta < 0)
	{
		q2 = Quaternion(-q2.w, -q2.x, -q2.y, -q2.z);
		cosTheta = -cosTheta;
	}

	// Calculate SLERP
	Quaternion q;
	float theta = acos(cosTheta);
	float a = sin((1.0f - t) * theta) / sin(theta);
	float b = sin(t * theta) / sin(theta);
	q.w = a * q1.w + b * q2.w;
	q.x = a * q1.x + b * q2.x;
	q.y = a * q1.y + b * q2.y;
	q.z = a * q1.z + b * q2.z;

	return q;
}

