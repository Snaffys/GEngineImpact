#ifndef WINDOW_CLASS_H
#define WINDOW_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <iostream>
#include <assert.h>

//#include "Texture_class.h"
#include "Camera_class.h"
#include "Model_class.h"

#include <unordered_map>
#include <map>
#include "FBO_class.h"

class Window;

extern std::unordered_map<GLFWwindow*, Window*> window_state_map;

class Window {
public:
	Window(unsigned int width = 2400, unsigned int height = 1269);

	// Callbacks
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double x_pos, double y_posintion);
	static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);

	void Go();

	~Window();
private:
	// Window's parameters
	int width, height;
	GLFWwindow* window;

	// Shaders
	Shader model_shader;
	Shader lighting_shader;
	Shader stencil_shader;
	Shader no_material_shader;
	Shader discard_shader;
	Shader framebuffer_shader;
	Shader cubemap_shader;
	Shader cubemap_reflection_shader;
	Shader instancing_shader;
	Shader object_shader;

	// Flashlight
	bool flashlight;

	// Textures
	//Texture texture_1;
	//Texture texture_2;
	Texture diffuse_map;
	Texture specular_map;
	Texture grass_tex;
	Texture window_tex;
	// Models
	Model loaded_model;
	Model reflected_model;

	Texture cubemap_tex;
	MultisampledTexture anti_aliasing_fbo_tex;
	Texture post_proc_fbo_tex;



	// Buffers
	VAO vao_model;
	VAO vao_light;
	VAO grass_transparent_vao;
	VAO windows_transparent_vao;
	FramebufferVAO framebuffer_vao;
	CubemapVAO cubemap_vao;
	VBO vbo;
	VBO transparent_vbo;
	VBO framebuffer_vbo;
	VBO cubemap_vbo;
	VBO light_pos_storage;
	VBO light_col_storage;
	VBO cubes_pos_storage;
	VBO grass_pos_storage;
	VBO windows_pos_storage;
	VBO models_pos_storage;
	EBO ebo;
	Framebuffer anti_aliasing_fbo;
	Framebuffer post_proc_fbo;

	// Camera
	Camera camera;
	float delta_time;
	float last_frame;
	float last_x, last_y;
	float fov;
	bool first_mouse;

	// Light settings
	void dir_light_settings(glm::vec3 dir_light_color, Shader& shader);
	void point_light_settings(int point_light_size, glm::vec3* point_light_pos, glm::vec3* point_ambient_power, glm::vec3* point_diff_power, glm::vec3* point_spec_power, glm::vec3* point_light_colors);
	void spot_light_settings(glm::vec3 spot_light_color, glm::vec3 cam_pos, glm::vec3 cam_front, Shader& shader);

	// Draws light blocks
	void draw_light_blocks(int point_light_size, glm::vec3* point_spec_power, glm::vec3* point_light_pos);

	// Draws ojbects
	void draw_ojbect(int objects_amount, glm::vec3* model_positions);

	// Draws models
	void draw_model(Shader& shader, glm::vec3 translate, glm::vec3 scale, float rotate_angle, glm::vec3 rotate, Model& loaded_model);

	void draw_model(Shader& shader, Model& loaded_model);

	void draw_model(Shader& shader, Model& loaded_model, unsigned int models_amount);

	// Sets projection and view matrices
	void set_coord_systems(Shader& shader, glm::mat4 proj, glm::mat4 view);

	glm::mat4 get_projection();

	void processInput(GLFWwindow* window);
};

#endif