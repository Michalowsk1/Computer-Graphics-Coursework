
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <common/maths.hpp>

class Camera
{
public:
	//projection parameters
	float fov = Maths::radians(45.0f);
	float aspect = 1024.0f / 768.0f;
	float near = 0.2f;
	float far = 100.0f;
	vec3 right = vec3(1.0f, 0.0f, 0.0f);
	vec3 up = vec3(0.0f, 1.0f, 0.0f);
	vec3 front = vec3(0.0f, 0.0f, -1.0f);

	vec3 offset = vec3(2.0f, 1.0f, 0.0f);

	// Camera Euler angles
	float yaw = 0.0f;
	float pitch = 0.0f;
	float roll = 0.0f;

	//camera vectors
	vec3 eye;
	vec3 target;
	vec3 worldUp = vec3(0.0f, 1.0f, 0.0f);

	//transformation matrices
	mat4 view;
	mat4 projection;


	//Quaternion camera
	Quaternion orientation = Quaternion(pitch, yaw);

	//constructor
	Camera(const vec3 eye, const vec3 target);

	//methods
	void calculateMatrices();

	void calculateCameraVectors();

	void quaternionCamera();

	void ThirdPersonCamera();
};
