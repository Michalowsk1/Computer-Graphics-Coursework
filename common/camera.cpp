#include <common/camera.hpp>

Camera::Camera(const vec3 Eye, const vec3 Target)
{
	eye = Eye;
	target = Target;
}

void Camera::calculateMatrices()
{
	// Calculate camera vectors
	calculateCameraVectors();

	//CALCULATE view matrix
	view = lookAt(eye, eye + front, worldUp);

	//calculate projection matrix
	projection = perspective(fov, aspect, near, far);
}

void Camera::calculateCameraVectors()
{
	front = glm::vec3(cos(yaw) * cos(pitch), sin(pitch), sin(yaw) * cos(pitch));
	right = glm::normalize(Maths::MathsCross(front, worldUp));
	up = Maths::MathsCross(right, front);
}

void Camera::quaternionCamera()
{
		// Calculate camera orientation quaternion from the Euler angles
		Quaternion newOrientation(-pitch, yaw);

		//Apply SLERP
		orientation = Maths::SLERP(orientation, newOrientation, 0.2f);

		// Calculate the view matrix
		view = orientation.matrix() * Maths::translate(-eye);

		// Calculate the projection matrix
		projection = glm::perspective(fov, aspect, near, far);

		// Calculate camera vectors from view matrix
		right = glm::vec3(view[0][0], view[1][0], view[2][0]);
		up = glm::vec3(view[0][1], view[1][1], view[2][1]);
		front = -glm::vec3(view[0][2], view[1][2], view[2][2]);

	
}

void Camera::ThirdPersonCamera()
{

	if (pitch > 0.5) //limits player so he cant do full 360 spin
		pitch = 0.5;

	else if (pitch < -0.6)
		pitch = -0.6;

	if (varBackOffset > -1.5) //limits player cam adjust distance
		varBackOffset = -1.5;

	else if (varBackOffset < -3.5)
		varBackOffset = -3.5;

	vec3 offset = (up * 0.2f) + (right * varBackOffset);


	Quaternion newOrientation(-pitch, yaw);

	orientation = Maths::SLERP(orientation, newOrientation, 1.0f);

	view = orientation.matrix() * Maths::translate(-eye + offset);

	projection = glm::perspective(fov, aspect, near, far);

	// Step 7: Recalculate camera basis vectors from the new view matrix
	right = glm::vec3(view[0][0], view[1][0], view[2][0]);
	up = glm::vec3(view[0][1], view[1][1], view[2][1]);
	front = -glm::vec3(view[0][2], view[1][2], view[2][2]);

	
}

