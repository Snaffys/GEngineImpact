#ifndef WINDOW_CLASS_H
#define WINDOW_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <iostream>
#include <assert.h>

#include "Camera_class.h"
#include "Model_class.h"

#include <unordered_map>
#include <map>
#include "FBO_class.h"
#include "UBO_class.h"

class Window;

extern std::unordered_map<GLFWwindow*, Window*> window_state_map;

class Window {
public:
	Window(unsigned int width = 1900, unsigned int height = 1284);

	// Callbacks
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double x_pos, double y_posintion);
	static void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);

	void Go();

	~Window();
private:
	const unsigned int samples = 22;

	float near_plane_camera = 0.1f;
	float far_plane_camera = 100.0f;

	unsigned int shadow_size = 2048;
	float near_plane_shadow = 0.1f;
	float far_plane_shadow = 100.0f;

	unsigned int point_shadow_map_id[4];

	unsigned int cubemap_shadow_size = 2048;
	float near_plane_cubamap_shadow = 0.1f;
	float far_plane_cubemap_shadow = 15.0f;

	// Window's parameters
	int width, height;
	GLFWwindow* window;

	// Shaders
	Shader lighting_shader;
	Shader stencil_shader;
	Shader no_material_shader;
	Shader discard_shader;
	Shader framebuffer_shader;
	Shader cubemap_shader;
	Shader cubemap_reflection_shader;
	Shader instancing_shader;
	Shader object_shader;
	Shader normal_map_shader;
	
	//Shader shadow_shader;
	
	Shader shadow_objects;
	Shader discard_shadow;
	Shader geom_test;
	Shader normal_visualizing;
	Shader point_shadow_objects;
	Shader point_discard_shadow;
	Shader normal_map_shadow;
	Shader point_normal_map_shadow;

	Shader blur_shader;

	unsigned int ubo_matrices;


	// Flashlight
	bool flashlight;

	// Textures
	Texture diffuse_map;
	Texture specular_map;
	Texture grass_tex;
	Texture window_tex;

	ShadowTexture directional_shadow_fbo_tex;
	ShadowTexture spot_shadow_fbo_tex;

	Texture diffuse_brick_map;
	Texture black_specular_map;
	Texture normal_map_tex;

	Texture displacement_map_tex;
	Texture ping_fbo_tex, pong_fbo_tex;

	// Models
	Model loaded_model;
	Model reflected_model;

	Texture cubemap_tex;
	
	CubemapShadowTexture point_shadow_fbo_tex1;
	CubemapShadowTexture point_shadow_fbo_tex2;
	CubemapShadowTexture point_shadow_fbo_tex3;
	CubemapShadowTexture point_shadow_fbo_tex4;

	MultisampledTexture anti_aliasing_fbo_tex1;
	MultisampledTexture anti_aliasing_fbo_tex2;

	Texture post_proc_fbo_tex1, post_proc_fbo_tex2;


	MultisampledTexture transparent_fbo_tex;


	// Buffers
	VAO vao_model;
	VAO vao_light;
	VAO grass_transparent_vao;
	VAO windows_transparent_vao;
	NormalVAO normal_map_vao;
	FramebufferVAO geom_vao;
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
	VBO geom_vbo;
	VBO normal_map_vbo;
	VBO normal_map_vbo_storage;
	EBO ebo;
	Framebuffer anti_aliasing_fbo;
	Framebuffer post_proc_fbo;
	Framebuffer transparent_fbo;
	Framebuffer ping_fbo, pong_fbo;
	
	ShadowFramebuffer directional_shadow_fbo;
	ShadowFramebuffer spot_shadow_fbo;
	ShadowFramebuffer cubemap_shadow_fbo1;
	ShadowFramebuffer cubemap_shadow_fbo2;
	ShadowFramebuffer cubemap_shadow_fbo3;
	ShadowFramebuffer cubemap_shadow_fbo4;

	UniformBuffer pos_matrix_ubo;
	UniformBuffer light_matrix_ubo;
	UniformBuffer point_light_matrix_ubo;
	UniformBuffer light_data_ubo;

	// Camera
	Camera camera;
	float delta_time;
	float last_frame;
	float last_x, last_y;
	float camera_fov;
	bool first_mouse;



	float exposure = 1.0f;

	// Light settings
	void dir_light_settings(glm::vec3 dir_light_color, Shader& shader, UniformBuffer& ubo);
	void point_light_settings(int point_light_size, glm::vec3* point_light_pos, glm::vec3* point_ambient_power, glm::vec3* point_diff_power, glm::vec3* point_spec_power, glm::vec3* point_light_colors);
	void spot_light_settings(glm::vec3 spot_light_color, glm::vec3 cam_pos, glm::vec3 cam_front, Shader& shader, UniformBuffer& ubo);

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

	glm::mat4 get_projection_perspective(float pov, float aspect, float near_plane, float far_plane);

	glm::mat4 get_projection_ortho(float near_plane, float far_plane);

	void bind_uniform_block(unsigned int shader_id[], const unsigned int arr_size, const char* uniform_block, unsigned int binding_point);

	void processInput(GLFWwindow* window);

	void render_shadow_map(Shader& default_shader, Shader& discard_shader, Shader& normal_map_shader, std::map<float, glm::vec3> sorted, glm::mat4 windows_pos_mat[], glm::mat4 grass_pos_mat[], unsigned int objects_amount, unsigned int normal_map_objects_amount, int reflected_models_amount, int models_amount);

	std::vector<glm::mat4> CalcPointShadowMatrices(glm::mat4 shadow_projection_mat, glm::vec3 light_pos);
};

#endif