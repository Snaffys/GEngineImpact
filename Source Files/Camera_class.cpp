#include"Camera_class.h"

Camera::Camera(glm::vec3 camera_position, glm::vec3 camera_front, glm::vec3 camera_up, 
				float movement_speed, float mouse_sensitivity, float actual_speed)
				: camera_position(camera_position), camera_front(camera_front), camera_up(camera_up), 
				movement_speed(movement_speed), mouse_sensitivity(mouse_sensitivity), actual_speed(actual_speed){}

// Moving through space
void Camera::ProcessKeyPress(CameraMovement direction, float delta_time) {
	actual_speed = delta_time * movement_speed;
	// Moves camera
	if (direction == FORWARD)
		camera_position += actual_speed * camera_front;
	if (direction == BACKWARD)
		camera_position -= actual_speed * camera_front;
	if (direction == RIGHT)
		camera_position -= actual_speed * glm::normalize(glm::cross(camera_front, camera_up));
	if (direction == LEFT)
		camera_position += actual_speed * glm::normalize(glm::cross(camera_front, camera_up));
}

// Mouse movement
void Camera::ProcessMouseMove(float x_offset, float y_offset) {
	x_offset *= mouse_sensitivity;
	y_offset *= mouse_sensitivity;
	// Changes pitch and yaw
	yaw += x_offset;
	pitch += y_offset;

	// Restrictons for the pitch
	if (pitch > 89.9f)
		pitch = 89.9f;
	if (pitch < -89.9f)
		pitch = -89.9f;
	
	// Calculates directions
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	// Sets camera_front
	camera_front = glm::normalize(direction);

	// For FPS camera
	//direction.x = cos(glm::radians(yaw)) * std::abs(sin(glm::radians(pitch)));
	//direction.y = 0.0f;
	//direction.z = sin(glm::radians(yaw)) * std::abs(sin(glm::radians(pitch)));
	//camera_front = glm::normalize(direction);
}

glm::vec3 Camera::GetCameraPosition() {
	return camera_position;
}

glm::vec3 Camera::GetCameraFront() {
	return camera_front;
}
glm::mat4 Camera::GetViewMatrix() {
	return glm::lookAt(camera_position, camera_position + camera_front, camera_up);
}

