#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera {
public:
	Camera() = default;
	Camera(glm::vec3 camera_position, glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f),
		float movement_speed = 3.5f, float mouse_sensitivity = 0.1f, float actual_speed = 0.0f);
	glm::mat4 GetViewMatrix();
	void ProcessKeyPress(CameraMovement, float);
	void ProcessMouseMove(float x_offset, float y_offset);

	glm::vec3 GetCameraPosition();
	glm::vec3 GetCameraFront();
private:
	float yaw = -90.0f, pitch = 0.0f;
	float movement_speed, mouse_sensitivity, actual_speed;
	glm::vec3 camera_position, camera_front, camera_up;
};

#endif