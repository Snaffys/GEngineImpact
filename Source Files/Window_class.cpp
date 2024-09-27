#include"Window_class.h"

std::unordered_map<GLFWwindow*, Window*> window_state_map;

Window::Window(unsigned int width, unsigned int height) {
	// WINDOW PREPARATION__________________________________________________________________________________
	// Initializes width and height
	this->width = width;
	this->height = height;
	// Creates window
	window = glfwCreateWindow(width, height, "Prototype", NULL, NULL);
	// Checks for window creation errors
	assert(window != NULL && "Failed to create GLFW window");
	// Makes the window's context current
	glfwMakeContextCurrent(window);
	// Checks for GLAD errors
	assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) && "Failed to initialize GLAD");


	// CALLBACKS___________________________________________________________________________________________
	// Map for callbacks
	window_state_map[window] = this;
	// Sets objects in window stretching callback
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// Sets mouse callback
	glfwSetCursorPosCallback(window, mouse_callback);
	// Sets croll callback
	glfwSetScrollCallback(window, scroll_callback);

	// CAMERA______________________________________________________________________________________________
	// Makes cursor invisible
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Sets first_mouse to true to set cursor's position properly
	first_mouse = true;
	// Sets cursor on the screen center
	last_x = width / 2;
	last_y = height / 2;
	// Creates camera
	camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	// Initializes the field of view
	camera_fov = 90.0f;

	// Enables z-buffer to output objects properly (initial value of glDepthFunc is GL_LESS, that means that ojbect which further from you replaces with object which nearer)
	glEnable(GL_DEPTH_TEST);

	// Enables stencil buffer
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);	// Control how to pass/fail a discard_shadow. Whenever the stencil value of a fragment is not equal to 1 the fragment passes the discard_shadow and is drawn, otherwise discarded
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);	// Control result of passing/failing a discard_shadow. If both stencil and depth tests pass, values update to ref in glStencilFunc(1 in this case)

	// Enables blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enables face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	// Variables for proper movement
	delta_time = 0.0f;
	last_frame = 0.0f;

	// SHADERS_______________________________________________________________________________________________


	instancing_shader = { "Shaders/vertex/Test.vert", "Shaders/fragment/Test.frag" };
	lighting_shader = { "Shaders/vertex/lighting.vert", "Shaders/fragment/lighting.frag" };

	no_material_shader = { "Shaders/vertex/no_material.vert", "Shaders/fragment/no_material.frag" };
	discard_shader = { "Shaders/vertex/no_material.vert", "Shaders/fragment/discard.frag" };
	
	stencil_shader = { "Shaders/vertex/stencil_buffer.vert", "Shaders/fragment/stencil_buffer.frag" };

	framebuffer_shader = { "Shaders/vertex/framebuffer_shader.vert", "Shaders/fragment/framebuffer_shader.frag" };
	cubemap_shader = { "Shaders/vertex/cubemap_shader.vert", "Shaders/fragment/cubemap_shader.frag" };
	cubemap_reflection_shader = { "Shaders/vertex/cubemap_reflection.vert", "Shaders/fragment/cubemap_reflection.frag" };
	
	object_shader = { "Shaders/vertex/cubes_instancing.vert", "Shaders/fragment/default.frag", "Shaders/geometry/test.geom" };
	//shadow_shader = { "Shaders/vertex/framebuffer_shadow.vert", "Shaders/fragment/framebuffer_shadow.frag" };
	shadow_objects = { "Shaders/vertex/shadow_objects.vert", "Shaders/fragment/framebuffer_shadow.frag" };
	discard_shadow = { "Shaders/vertex/shadow_blending.vert", "Shaders/fragment/shadow_discard.frag" };
	normal_map_shadow = { "Shaders/vertex/normal_map_shadow.vert", "Shaders/fragment/framebuffer_shadow.frag" };

	normal_visualizing = { "Shaders/vertex/normal_visualizing.vert", "Shaders/fragment/normal_visualizing.frag", "Shaders/geometry/normal_visualizing.geom" };

	point_shadow_objects = { "Shaders/vertex/point_shadow_objects.vert", "Shaders/fragment/point_shadow_objects.frag", "Shaders/geometry/point_shadow_objects.geom" };
	point_discard_shadow = { "Shaders/vertex/point_shadow_blending.vert", "Shaders/fragment/point_shadow_discard.frag", "Shaders/geometry/shadow_blending.geom" };
	point_normal_map_shadow = { "Shaders/vertex/point_normal_map_shadow.vert", "Shaders/fragment/point_shadow_objects.frag", "Shaders/geometry/point_shadow_objects.geom" };

	normal_map_shader = { "Shaders/vertex/normal_map.vert", "Shaders/fragment/normal_map.frag"};

	blur_shader = { "Shaders/vertex/gaussian_blur.vert", "Shaders/fragment/gaussian_blur.frag"};

	pos_matrix_ubo = { 2 * sizeof(glm::mat4) };
	unsigned int shader_id[] = { object_shader.GetID(), instancing_shader.GetID(), lighting_shader.GetID(), stencil_shader.GetID(), discard_shader.GetID(), no_material_shader.GetID(), cubemap_reflection_shader.GetID(), normal_visualizing.GetID(), normal_map_shader.GetID()};
	bind_uniform_block(shader_id, sizeof(shader_id) / sizeof(shader_id[0]), "matrices", pos_matrix_ubo.GetId());

	light_matrix_ubo = { sizeof(glm::mat4) };
	unsigned int shader_id_light[] = { object_shader.GetID(), discard_shadow.GetID(), shadow_objects.GetID(), normal_map_shadow.GetID(), normal_map_shader.GetID()};
	bind_uniform_block(shader_id_light, sizeof(shader_id_light) / sizeof(shader_id_light[0]), "light_matrix", light_matrix_ubo.GetId());

	// vec3 because float's allignment requirements is 4 bites 
	point_light_matrix_ubo = { sizeof(glm::vec3) + sizeof(float)};
	unsigned int shader_id_point_light[] = { point_shadow_objects.GetID(), point_discard_shadow.GetID(), point_normal_map_shadow.GetID()};
	bind_uniform_block(shader_id_point_light, sizeof(shader_id_point_light) / sizeof(shader_id_point_light[0]), "pointLight", point_light_matrix_ubo.GetId());

	//int  asdf = sizeof(glm::vec4) * 14 + sizeof(float) * 9 + sizeof(glm::vec4) * 4 * 3 + sizeof(float) * 3 * 3;

	light_data_ubo = { sizeof(glm::vec4) * 21 + sizeof(glm::vec3) * 5 + sizeof(float) * 26 + 12};
	unsigned int shader_light_data_id[] = { object_shader.GetID(), normal_map_shader.GetID() };
	bind_uniform_block(shader_light_data_id, sizeof(shader_light_data_id) / sizeof(shader_light_data_id[0]), "lightData", light_data_ubo.GetId());



	// TEXTURES______________________________________________________________________________________________
	// model's texture settings
	diffuse_map = { "Textures/steeled_wooden_container.png", GL_CLAMP_TO_EDGE, GL_RGBA };
	specular_map = { "Textures/steeled_border_2.png", GL_CLAMP_TO_EDGE, GL_RGBA };
	grass_tex = { "Textures/grass.png", GL_CLAMP_TO_EDGE, GL_RGBA, GL_SRGB_ALPHA };
	window_tex = { "Textures/window_tex.png", GL_CLAMP_TO_EDGE, GL_RGBA, GL_SRGB_ALPHA };
	char faces[][100] =
	{ "Textures/Cubemaps/right.jpg",
	"Textures/Cubemaps/left.jpg",
	"Textures/Cubemaps/top.jpg",
	"Textures/Cubemaps/bottom.jpg",
	"Textures/Cubemaps/front.jpg",
	"Textures/Cubemaps/back.jpg" };
	cubemap_tex = { (int)sizeof(faces) / sizeof(faces)[0], (int)sizeof(faces[0]) / sizeof(faces[0])[0], (char*)faces };

	diffuse_brick_map = { "Textures/red_brickwall.jpg", GL_CLAMP_TO_EDGE, GL_RGB, NULL, false };

	black_specular_map = { "Textures/black_texture.png", GL_CLAMP_TO_EDGE, GL_RGBA };

	normal_map_tex = { "Textures/red_brickwall_normal_map.jpg", GL_CLAMP_TO_EDGE, GL_RGB, NULL, false };

	displacement_map_tex = { "Textures/red_brickwall_disp.jpg", GL_CLAMP_TO_EDGE, GL_RGB, GL_RED, false };


	// BUFFERS______________________________________________________________________________________________

	vbo = {
		// positions				// normals				// texture coords	
		// front square
		{ -0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f},	// bottom-left
		{  0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f},	// bottom-right
		{  0.5f,  0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f},	// top-right
		{  0.5f,  0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f},	// top-right
		{ -0.5f,  0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f},	// top-left
		{ -0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f},	// bottom-left
		// right square
		{ 0.5f,  0.5f,  0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f},	// top-left
		{ 0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f},	// bottom-right
		{ 0.5f,  0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f},	// top-right
		{ 0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f},	// bottom-right
		{ 0.5f,  0.5f,  0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f},	// top-left
		{ 0.5f, -0.5f,  0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f},	// bottom-left
		// left square 
		{ -0.5f,  0.5f,  0.5f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f},	// top-right
		{ -0.5f,  0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f},	// top-left
		{ -0.5f, -0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f},	// bottom-left
		{ -0.5f, -0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f},	// bottom-left
		{ -0.5f, -0.5f,  0.5f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f},	// bottom-right
		{ -0.5f,  0.5f,  0.5f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f},	// top-right
		// back square
		{ -0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f},	// bottom-left
		{  0.5f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f},	// top-right
		{  0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f},	// bottom-right
		{  0.5f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f},	// top-right
		{ -0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f},	// bottom-left
		{ -0.5f,  0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f},	// top-left
		// top square
		{ -0.5f,  0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f},	// top-left
		{  0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f},	// bottom-right
		{  0.5f,  0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f},	// top-right
		{  0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f},	// bottom-right
		{ -0.5f,  0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f},	// top-left
		{ -0.5f,  0.5f,  0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f},	// bottom-left
		// bottom square
		{  0.5f, -0.5f,  0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f},	// bottom-right
		{ -0.5f, -0.5f,  0.5f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f},	// bottom-left
		{ -0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f},	// top-left
		{ -0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f},	// top-left
		{  0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f},	// top-right
		{  0.5f, -0.5f,  0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f}		// bottom-right
	};
	
	vao_model = VAO(vbo);
	vao_light = VAO(vbo);

	// BUFFERS
	//vbo = {
	//	// positions				// colors				// texture coords	// normals
	//	// front square
	//	{ -0.5f, -0.5f, 0.5f,		0.0f, 0.7f, 0.2f,		0.0f, 0.0f,			0.0f, 0.0f, 1.0f},
	//	{ -0.5f,  0.5f, 0.5f,		0.0f, 0.4f, 0.6f,		0.0f, 1.0f,			0.0f, 0.0f, 1.0f},
	//	{  0.5f,  0.5f, 0.5f,		0.0f, 0.2f, 0.8f,		1.0f, 1.0f,			0.0f, 0.0f, 1.0f},
	//	{  0.5f,  0.5f, 0.5f,		0.0f, 0.3f, 0.6f,		1.0f, 1.0f,			0.0f, 0.0f, 1.0f},
	//	{  0.5f, -0.5f, 0.5f,		0.0f, 0.1f, 0.4f,		1.0f, 0.0f,			0.0f, 0.0f, 1.0f},
	//	{ -0.5f, -0.5f, 0.5f,		0.0f, 0.2f, 0.8f,		0.0f, 0.0f,			0.0f, 0.0f, 1.0f},
	//	// right square
	//	{ 0.5f, -0.5f,  0.5f,		0.0f, 0.7f, 0.2f,		0.0f, 0.0f,			1.0f, 0.0f, 0.0f},
	//	{ 0.5f,  0.5f,  0.5f,		0.0f, 0.5f, 0.1f,		0.0f, 1.0f,			1.0f, 0.0f, 0.0f},
	//	{ 0.5f,  0.5f, -0.5f,		0.0f, 0.6f, 0.3f,		1.0f, 1.0f,			1.0f, 0.0f, 0.0f},
	//	{ 0.5f,  0.5f, -0.5f,		0.0f, 0.8f, 0.3f,		1.0f, 1.0f,			1.0f, 0.0f, 0.0f},
	//	{ 0.5f, -0.5f, -0.5f,		0.0f, 0.5f, 0.2f,		1.0f, 0.0f,			1.0f, 0.0f, 0.0f},
	//	{ 0.5f, -0.5f,  0.5f,		0.0f, 0.4f, 0.1f,		0.0f, 0.0f,			1.0f, 0.0f, 0.0f},
	//	// left square 
	//	{ -0.5f, -0.5f,  0.5f,		0.0f, 0.7f, 0.2f,		0.0f, 0.0f,			-1.0f, 0.0f, 0.0f},
	//	{ -0.5f,  0.5f,  0.5f,		0.0f, 0.5f, 0.1f,		0.0f, 1.0f,			-1.0f, 0.0f, 0.0f},
	//	{ -0.5f,  0.5f, -0.5f,		0.0f, 0.6f, 0.3f,		1.0f, 1.0f,			-1.0f, 0.0f, 0.0f},
	//	{ -0.5f,  0.5f, -0.5f,		0.0f, 0.8f, 0.3f,		1.0f, 1.0f,			-1.0f, 0.0f, 0.0f},
	//	{ -0.5f, -0.5f, -0.5f,		0.0f, 0.5f, 0.2f,		1.0f, 0.0f,			-1.0f, 0.0f, 0.0f},
	//	{ -0.5f, -0.5f,  0.5f,		0.0f, 0.4f, 0.1f,		0.0f, 0.0f,			-1.0f, 0.0f, 0.0f},
	//	// back square
	//	{ -0.5f, -0.5f, -0.5f,		0.0f, 0.7f, 0.7f,		0.0f, 0.0f,			0.0f, 0.0f, -1.0f},
	//	{ -0.5f,  0.5f, -0.5f,		0.1f, 0.4f, 0.4f,		0.0f, 1.0f,			0.0f, 0.0f, -1.0f},
	//	{  0.5f,  0.5f, -0.5f,		0.2f, 0.7f, 0.8f,		1.0f, 1.0f,			0.0f, 0.0f, -1.0f},
	//	{  0.5f,  0.5f, -0.5f,		0.3f, 0.3f, 0.4f,		1.0f, 1.0f,			0.0f, 0.0f, -1.0f},
	//	{  0.5f, -0.5f, -0.5f,		0.2f, 0.4f, 0.2f,		1.0f, 0.0f,			0.0f, 0.0f, -1.0f},
	//	{ -0.5f, -0.5f, -0.5f,		0.1f, 0.2f, 0.3f,		0.0f, 0.0f,			0.0f, 0.0f, -1.0f},
	//	// top square
	//	{ -0.5f,  0.5f,  0.5f,		0.5f, 0.2f, 0.2f,		0.0f, 0.0f,			0.0f, 1.0f, 0.0f},
	//	{ -0.5f,  0.5f, -0.5f,		0.6f, 0.1f, 0.2f,		0.0f, 1.0f,			0.0f, 1.0f, 0.0f},
	//	{  0.5f,  0.5f, -0.5f,		0.8f, 0.2f, 0.3f,		1.0f, 1.0f,			0.0f, 1.0f, 0.0f},
	//	{  0.5f,  0.5f, -0.5f,		0.7f, 0.3f, 0.2f,		1.0f, 1.0f,			0.0f, 1.0f, 0.0f},
	//	{  0.5f,  0.5f,  0.5f,		0.5f, 0.1f, 0.4f,		1.0f, 0.0f,			0.0f, 1.0f, 0.0f},
	//	{ -0.5f,  0.5f,  0.5f,		0.6f, 0.2f, 0.1f,		0.0f, 0.0f,			0.0f, 1.0f, 0.0f},
	//	// bottom square
	//	{ -0.5f, -0.5f,  0.5f,		0.3f, 0.0f, 0.5f,		0.0f, 0.0f,			0.0f, -1.0f, 0.0f},
	//	{ -0.5f, -0.5f, -0.5f,		0.4f, 0.0f, 0.6f,		0.0f, 1.0f,			0.0f, -1.0f, 0.0f},
	//	{  0.5f, -0.5f, -0.5f,		0.4f, 0.0f, 0.8f,		1.0f, 1.0f,			0.0f, -1.0f, 0.0f},
	//	{  0.5f, -0.5f, -0.5f,		0.3f, 0.0f, 0.9f,		1.0f, 1.0f,			0.0f, -1.0f, 0.0f},
	//	{  0.5f, -0.5f,  0.5f,		0.1f, 0.0f, 0.8f,		1.0f, 0.0f,			0.0f, -1.0f, 0.0f},
	//	{ -0.5f, -0.5f,  0.5f,		0.1f, 0.0f, 0.8f,		0.0f, 0.0f,			0.0f, -1.0f, 0.0f}
	//};

	transparent_vbo = {
		{ 0.0f,  0.5f, 0.0f,	0.0f, 0.0f, 0.0f,		0.0f, 1.0f},
		{ 0.0f, -0.5f, 0.0f,	0.0f, 0.0f, 0.0f,		0.0f, 0.0f},
		{ 1.0f, -0.5f, 0.0f,	0.0f, 0.0f, 0.0f,		1.0f, 0.0f},
		{ 0.0f,  0.5f, 0.0f,	0.0f, 0.0f, 0.0f,		0.0f, 1.0f},
		{ 1.0f, -0.5f, 0.0f,	0.0f, 0.0f, 0.0f,		1.0f, 0.0f},
		{ 1.0f,  0.5f, 0.0f,	0.0f, 0.0f, 0.0f,		1.0f, 1.0f}
	};
	grass_transparent_vao = VAO(transparent_vbo);
	
	windows_transparent_vao = VAO(transparent_vbo);

	cubemap_vbo = {
		// positions   
		{-1.0f,  1.0f, -1.0f},
		{-1.0f, -1.0f, -1.0f},
		{ 1.0f, -1.0f, -1.0f},
		{ 1.0f, -1.0f, -1.0f},
		{ 1.0f,  1.0f, -1.0f},
		{-1.0f,  1.0f, -1.0f},
	
		{-1.0f, -1.0f,  1.0f},
		{-1.0f, -1.0f, -1.0f},
		{-1.0f,  1.0f, -1.0f},
		{-1.0f,  1.0f, -1.0f},
		{-1.0f,  1.0f,  1.0f},
		{-1.0f, -1.0f,  1.0f},
	
		{1.0f, -1.0f, -1.0f},
		{1.0f, -1.0f,  1.0f},
		{1.0f,  1.0f,  1.0f},
		{1.0f,  1.0f,  1.0f},
		{1.0f,  1.0f, -1.0f},
		{1.0f, -1.0f, -1.0f},
	
		{-1.0f, -1.0f,  1.0f},
		{-1.0f,  1.0f,  1.0f},
		{ 1.0f,  1.0f,  1.0f},
		{ 1.0f,  1.0f,  1.0f},
		{ 1.0f, -1.0f,  1.0f},
		{-1.0f, -1.0f,  1.0f},
	
		{-1.0f,  1.0f, -1.0f},
		{ 1.0f,  1.0f, -1.0f},
		{ 1.0f,  1.0f,  1.0f},
		{ 1.0f,  1.0f,  1.0f},
		{-1.0f,  1.0f,  1.0f},
		{-1.0f,  1.0f, -1.0f},
	
		{-1.0f, -1.0f, -1.0f},
		{-1.0f, -1.0f,  1.0f},
		{ 1.0f, -1.0f, -1.0f},
		{ 1.0f, -1.0f, -1.0f},
		{-1.0f, -1.0f,  1.0f},
		{ 1.0f, -1.0f,  1.0f}
	};
	cubemap_vao = CubemapVAO(cubemap_vbo);



	// front square
	glm::vec3 pos1(-0.5f, 0.5f, 0.5f);
	glm::vec3 pos2(-0.5f, -0.5f, 0.5f);
	glm::vec3 pos3(0.5f, -0.5f, 0.5f);
	glm::vec3 pos4(0.5f, 0.5f, 0.5f);

	glm::vec2 uv1(0.0f, 1.0f);
	glm::vec2 uv2(0.0f, 0.0f);
	glm::vec2 uv3(1.0f, 0.0f);
	glm::vec2 uv4(1.0f, 1.0f);

	glm::vec3 tangent1, bitangent1;
	glm::vec3 tangent2, bitangent2;

	// first triangle
	glm::vec3 edge1 = pos2 - pos1;
	glm::vec3 edge2 = pos3 - pos1;
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
	tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

	// second triangle
	glm::vec3 edge3 = pos3 - pos1;
	glm::vec3 edge4 = pos4 - pos1;
	glm::vec2 deltaUV3 = uv3 - uv1;
	glm::vec2 deltaUV4 = uv4 - uv1;

	f = 1.0f / (deltaUV3.x * deltaUV4.y - deltaUV4.x * deltaUV3.y);
	tangent2.x = f * (deltaUV4.y * edge3.x - deltaUV3.y * edge4.x);
	tangent2.y = f * (deltaUV4.y * edge3.y - deltaUV3.y * edge4.y);
	tangent2.z = f * (deltaUV4.y * edge3.z - deltaUV3.y * edge4.z);
	bitangent2.x = f * (-deltaUV4.x * edge3.x + deltaUV3.x * edge4.x);
	bitangent2.y = f * (-deltaUV4.x * edge3.y + deltaUV3.x * edge4.y);
	bitangent2.z = f * (-deltaUV4.x * edge3.z + deltaUV3.x * edge4.z);


	// right square
	glm::vec3 pos5(0.5f,  0.5f,  0.5f);
	glm::vec3 pos6(0.5f, -0.5f,  0.5f);
	glm::vec3 pos7(0.5f, -0.5f, -0.5f);
	glm::vec3 pos8(0.5f,  0.5f, -0.5f);

	glm::vec2 uv5(0.0f, 1.0f);
	glm::vec2 uv6(0.0f, 0.0f);
	glm::vec2 uv7(1.0f, 0.0f);
	glm::vec2 uv8(1.0f, 1.0f);

	glm::vec3 tangent3, bitangent3;
	glm::vec3 tangent4, bitangent4;

	// first triangle
	glm::vec3 edge5 = pos6 - pos5;
	glm::vec3 edge6 = pos7 - pos5;
	glm::vec2 deltaUV5 = uv6 - uv5;
	glm::vec2 deltaUV6 = uv7 - uv5;

	f = 1.0f / (deltaUV5.x * deltaUV6.y - deltaUV6.x * deltaUV5.y);
	tangent3.x = f * (deltaUV6.y * edge5.x - deltaUV5.y * edge6.x);
	tangent3.y = f * (deltaUV6.y * edge5.y - deltaUV5.y * edge6.y);
	tangent3.z = f * (deltaUV6.y * edge5.z - deltaUV5.y * edge6.z);
	bitangent3.x = f * (-deltaUV6.x * edge5.x + deltaUV5.x * edge6.x);
	bitangent3.y = f * (-deltaUV6.x * edge5.y + deltaUV5.x * edge6.y);
	bitangent3.z = f * (-deltaUV6.x * edge5.z + deltaUV5.x * edge6.z);

	// second triangle
	glm::vec3 edge7 = pos7 - pos5;
	glm::vec3 edge8 = pos8 - pos5;
	glm::vec2 deltaUV7 = uv7 - uv5;
	glm::vec2 deltaUV8 = uv8 - uv5;

	f = 1.0f / (deltaUV7.x * deltaUV8.y - deltaUV8.x * deltaUV7.y);
	tangent4.x = f * (deltaUV8.y * edge7.x - deltaUV7.y * edge8.x);
	tangent4.y = f * (deltaUV8.y * edge7.y - deltaUV7.y * edge8.y);
	tangent4.z = f * (deltaUV8.y * edge7.z - deltaUV7.y * edge8.z);
	bitangent4.x = f * (-deltaUV8.x * edge7.x + deltaUV7.x * edge8.x);
	bitangent4.y = f * (-deltaUV8.x * edge7.y + deltaUV7.x * edge8.y);
	bitangent4.z = f * (-deltaUV8.x * edge7.z + deltaUV7.x * edge8.z);


	// left square 
	glm::vec3 pos9( -0.5f,  0.5f, -0.5f);
	glm::vec3 pos10(-0.5f, -0.5f, -0.5f);
	glm::vec3 pos11(-0.5f, -0.5f,  0.5f);
	glm::vec3 pos12(-0.5f,  0.5f,  0.5f);

	glm::vec2 uv9( 0.0f, 1.0f);
	glm::vec2 uv10(0.0f, 0.0f);
	glm::vec2 uv11(1.0f, 0.0f);
	glm::vec2 uv12(1.0f, 1.0f);


	glm::vec3 tangent5, bitangent5;
	glm::vec3 tangent6, bitangent6;

	// first triangle
	glm::vec3 edge9 = pos10 - pos9;
	glm::vec3 edge10 = pos11 - pos9;
	glm::vec2 deltaUV9 = uv10 - uv9;
	glm::vec2 deltaUV10 = uv11 - uv9;

	f = 1.0f / (deltaUV9.x * deltaUV10.y - deltaUV10.x * deltaUV9.y);
	tangent5.x = f * (deltaUV10.y * edge9.x - deltaUV9.y * edge10.x);
	tangent5.y = f * (deltaUV10.y * edge9.y - deltaUV9.y * edge10.y);
	tangent5.z = f * (deltaUV10.y * edge9.z - deltaUV9.y * edge10.z);
	bitangent5.x = f * (-deltaUV10.x * edge9.x + deltaUV9.x * edge10.x);
	bitangent5.y = f * (-deltaUV10.x * edge9.y + deltaUV9.x * edge10.y);
	bitangent5.z = f * (-deltaUV10.x * edge9.z + deltaUV9.x * edge10.z);

	// second triangle
	glm::vec3 edge11 = pos11 - pos9;
	glm::vec3 edge12 = pos12 - pos9;
	glm::vec2 deltaUV11 = uv11 - uv9;
	glm::vec2 deltaUV12 = uv12 - uv9;

	f = 1.0f / (deltaUV11.x * deltaUV12.y - deltaUV12.x * deltaUV11.y);
	tangent6.x = f * (deltaUV12.y * edge11.x - deltaUV11.y * edge12.x);
	tangent6.y = f * (deltaUV12.y * edge11.y - deltaUV11.y * edge12.y);
	tangent6.z = f * (deltaUV12.y * edge11.z - deltaUV11.y * edge12.z);
	bitangent6.x = f * (-deltaUV12.x * edge11.x + deltaUV11.x * edge12.x);
	bitangent6.y = f * (-deltaUV12.x * edge11.y + deltaUV11.x * edge12.y);
	bitangent6.z = f * (-deltaUV12.x * edge11.z + deltaUV11.x * edge12.z);



	// back square
	glm::vec3 pos13(-0.5f, -0.5f, -0.5f);
	glm::vec3 pos14(-0.5f, 0.5f, -0.5f);
	glm::vec3 pos15(0.5f, 0.5f, -0.5f);
	glm::vec3 pos16(0.5f, -0.5f, -0.5f);


	glm::vec2 uv13(1.0f, 0.0f);
	glm::vec2 uv14(1.0f, 1.0f);
	glm::vec2 uv15(0.0f, 1.0f);
	glm::vec2 uv16(0.0f, 0.0f);


	glm::vec3 tangent7, bitangent7;
	glm::vec3 tangent8, bitangent8;

	// first triangle
	glm::vec3 edge13 = pos14 - pos15;
	glm::vec3 edge14 = pos13 - pos15;
	glm::vec2 deltaUV13 = uv14 - uv15;
	glm::vec2 deltaUV14 = uv13 - uv15;

	f = 1.0f / (deltaUV13.x * deltaUV14.y - deltaUV14.x * deltaUV13.y);
	tangent7.x = f * (deltaUV14.y * edge13.x - deltaUV13.y * edge14.x);
	tangent7.y = f * (deltaUV14.y * edge13.y - deltaUV13.y * edge14.y);
	tangent7.z = f * (deltaUV14.y * edge13.z - deltaUV13.y * edge14.z);
	bitangent7.x = f * (-deltaUV14.x * edge13.x + deltaUV13.x * edge14.x);
	bitangent7.y = f * (-deltaUV14.x * edge13.y + deltaUV13.x * edge14.y);
	bitangent7.z = f * (-deltaUV14.x * edge13.z + deltaUV13.x * edge14.z);

	// second triangle
	glm::vec3 edge15 = pos13 - pos15;
	glm::vec3 edge16 = pos16 - pos15;
	glm::vec2 deltaUV15 = uv13 - uv15;
	glm::vec2 deltaUV16 = uv16 - uv15;

	f = 1.0f / (deltaUV15.x * deltaUV16.y - deltaUV16.x * deltaUV15.y);
	tangent8.x = f * (deltaUV16.y * edge15.x - deltaUV15.y * edge16.x);
	tangent8.y = f * (deltaUV16.y * edge15.y - deltaUV15.y * edge16.y);
	tangent8.z = f * (deltaUV16.y * edge15.z - deltaUV15.y * edge16.z);
	bitangent8.x = f * (-deltaUV16.x * edge15.x + deltaUV15.x * edge16.x);
	bitangent8.y = f * (-deltaUV16.x * edge15.y + deltaUV15.x * edge16.y);
	bitangent8.z = f * (-deltaUV16.x * edge15.z + deltaUV15.x * edge16.z);




	// top square
	glm::vec3 pos17(-0.5f, 0.5f, -0.5f);
	glm::vec3 pos18(-0.5f, 0.5f, 0.5f);
	glm::vec3 pos19( 0.5f, 0.5f, 0.5f);
	glm::vec3 pos20( 0.5f, 0.5f, -0.5f);

	glm::vec2 uv17(0.0f, 1.0f);
	glm::vec2 uv18(0.0f, 0.0f);
	glm::vec2 uv19(1.0f, 0.0f);
	glm::vec2 uv20(1.0f, 1.0f);


	glm::vec3 tangent9, bitangent9;
	glm::vec3 tangent10, bitangent10;

	// first triangle
	glm::vec3 edge17 = pos18 - pos17;
	glm::vec3 edge18 = pos19 - pos17;
	glm::vec2 deltaUV17 = uv18 - uv17;
	glm::vec2 deltaUV18 = uv19 - uv17;

	f = 1.0f / (deltaUV17.x * deltaUV18.y - deltaUV18.x * deltaUV17.y);
	tangent9.x = f * (deltaUV18.y * edge17.x - deltaUV17.y * edge18.x);
	tangent9.y = f * (deltaUV18.y * edge17.y - deltaUV17.y * edge18.y);
	tangent9.z = f * (deltaUV18.y * edge17.z - deltaUV17.y * edge18.z);
	bitangent9.x = f * (-deltaUV18.x * edge17.x + deltaUV17.x * edge18.x);
	bitangent9.y = f * (-deltaUV18.x * edge17.y + deltaUV17.x * edge18.y);
	bitangent9.z = f * (-deltaUV18.x * edge17.z + deltaUV17.x * edge18.z);

	// second triangle
	glm::vec3 edge19 = pos19 - pos17;
	glm::vec3 edge20 = pos20 - pos17;
	glm::vec2 deltaUV19 = uv19 - uv17;
	glm::vec2 deltaUV20 = uv20 - uv17;

	f = 1.0f / (deltaUV19.x * deltaUV20.y - deltaUV20.x * deltaUV19.y);
	tangent10.x = f * (deltaUV20.y * edge19.x - deltaUV19.y * edge20.x);
	tangent10.y = f * (deltaUV20.y * edge19.y - deltaUV19.y * edge20.y);
	tangent10.z = f * (deltaUV20.y * edge19.z - deltaUV19.y * edge20.z);
	bitangent10.x = f * (-deltaUV20.x * edge19.x + deltaUV19.x * edge20.x);
	bitangent10.y = f * (-deltaUV20.x * edge19.y + deltaUV19.x * edge20.y);
	bitangent10.z = f * (-deltaUV20.x * edge19.z + deltaUV19.x * edge20.z);


	// bottom square
	glm::vec3 pos21(-0.5f, -0.5f,  0.5f);
	glm::vec3 pos22(-0.5f, -0.5f, -0.5f);
	glm::vec3 pos23( 0.5f, -0.5f, -0.5f);
	glm::vec3 pos24( 0.5f, -0.5f,  0.5f);


	glm::vec2 uv21(1.0f, 0.0f);
	glm::vec2 uv22(1.0f, 1.0f);
	glm::vec2 uv23(0.0f, 1.0f);
	glm::vec2 uv24(0.0f, 0.0f);

	glm::vec3 tangent11, bitangent11;
	glm::vec3 tangent12, bitangent12;

	// first triangle
	glm::vec3 edge21 = pos22 - pos23;
	glm::vec3 edge22 = pos21 - pos23;
	glm::vec2 deltaUV21 = uv22 - uv23;
	glm::vec2 deltaUV22 = uv21 - uv23;

	f = 1.0f / (deltaUV21.x * deltaUV22.y - deltaUV22.x * deltaUV21.y);
	tangent11.x = f * (deltaUV22.y * edge21.x - deltaUV21.y * edge22.x);
	tangent11.y = f * (deltaUV22.y * edge21.y - deltaUV21.y * edge22.y);
	tangent11.z = f * (deltaUV22.y * edge21.z - deltaUV21.y * edge22.z);
	bitangent11.x = f * (-deltaUV22.x * edge21.x + deltaUV21.x * edge22.x);
	bitangent11.y = f * (-deltaUV22.x * edge21.y + deltaUV21.x * edge22.y);
	bitangent11.z = f * (-deltaUV22.x * edge21.z + deltaUV21.x * edge22.z);

	// second triangle
	glm::vec3 edge23 = pos21 - pos23;
	glm::vec3 edge24 = pos24 - pos23;
	glm::vec2 deltaUV23 = uv21 - uv23;
	glm::vec2 deltaUV24 = uv24 - uv23;

	f = 1.0f / (deltaUV23.x * deltaUV24.y - deltaUV24.x * deltaUV23.y);
	tangent12.x = f * (deltaUV24.y * edge23.x - deltaUV23.y * edge24.x);
	tangent12.y = f * (deltaUV24.y * edge23.y - deltaUV23.y * edge24.y);
	tangent12.z = f * (deltaUV24.y * edge23.z - deltaUV23.y * edge24.z);
	bitangent12.x = f * (-deltaUV24.x * edge23.x + deltaUV23.x * edge24.x);
	bitangent12.y = f * (-deltaUV24.x * edge23.y + deltaUV23.x * edge24.y);
	bitangent12.z = f * (-deltaUV24.x * edge23.z + deltaUV23.x * edge24.z);


	normal_map_vbo = {
			// front square
			{ pos1.x, pos1.y, pos1.z,		0.0f, 0.0f, 1.0f,		uv1.x, uv1.y,		tangent1.x, tangent1.y, tangent1.z,			bitangent1.x, bitangent1.y, bitangent1.z },	
			{ pos2.x, pos2.y, pos2.z,		0.0f, 0.0f, 1.0f,		uv2.x, uv2.y,		tangent1.x, tangent1.y, tangent1.z,			bitangent1.x, bitangent1.y, bitangent1.z },	
			{ pos3.x, pos3.y, pos3.z,		0.0f, 0.0f, 1.0f,		uv3.x, uv3.y,		tangent1.x, tangent1.y, tangent1.z,			bitangent1.x, bitangent1.y, bitangent1.z },	
			{ pos1.x, pos1.y, pos1.z,		0.0f, 0.0f, 1.0f,		uv1.x, uv1.y,		tangent2.x, tangent2.y, tangent2.z,			bitangent2.x, bitangent2.y, bitangent2.z },	
			{ pos3.x, pos3.y, pos3.z,		0.0f, 0.0f, 1.0f,		uv3.x, uv3.y,		tangent2.x, tangent2.y, tangent2.z,			bitangent2.x, bitangent2.y, bitangent2.z },	
			{ pos4.x, pos4.y, pos4.z,		0.0f, 0.0f, 1.0f,		uv4.x, uv4.y,		tangent2.x, tangent2.y, tangent2.z,			bitangent2.x, bitangent2.y, bitangent2.z },	
			// right square
			{ pos5.x, pos5.y, pos5.z,		1.0f, 0.0f, 0.0f,		uv5.x, uv5.y,		tangent3.x, tangent3.y, tangent3.z,			bitangent3.x, bitangent3.y, bitangent3.z },	
			{ pos6.x, pos6.y, pos6.z,		1.0f, 0.0f, 0.0f,		uv6.x, uv6.y,		tangent3.x, tangent3.y, tangent3.z,			bitangent3.x, bitangent3.y, bitangent3.z },	
			{ pos7.x, pos7.y, pos7.z,		1.0f, 0.0f, 0.0f,		uv7.x, uv7.y,		tangent3.x, tangent3.y, tangent3.z,			bitangent3.x, bitangent3.y, bitangent3.z },	
			{ pos5.x, pos5.y, pos5.z,		1.0f, 0.0f, 0.0f,		uv5.x, uv5.y,		tangent4.x, tangent4.y, tangent4.z,			bitangent4.x, bitangent4.y, bitangent4.z },	
			{ pos7.x, pos7.y, pos7.z,		1.0f, 0.0f, 0.0f,		uv7.x, uv7.y,		tangent4.x, tangent4.y, tangent4.z,			bitangent4.x, bitangent4.y, bitangent4.z },	
			{ pos8.x, pos8.y, pos8.z,		1.0f, 0.0f, 0.0f,		uv8.x, uv8.y,		tangent4.x, tangent4.y, tangent4.z,			bitangent4.x, bitangent4.y, bitangent4.z },	
			// left square 
			{ pos9.x , pos9.y , pos9.z ,	-1.0f, 0.0f, 0.0f,		uv9.x , uv9.y ,		tangent5.x, tangent5.y, tangent5.z,			bitangent5.x, bitangent5.y, bitangent5.z },	
			{ pos10.x, pos10.y, pos10.z,	-1.0f, 0.0f, 0.0f,		uv10.x, uv10.y,		tangent5.x, tangent5.y, tangent5.z,			bitangent5.x, bitangent5.y, bitangent5.z },	
			{ pos11.x, pos11.y, pos11.z,	-1.0f, 0.0f, 0.0f,		uv11.x, uv11.y,		tangent5.x, tangent5.y, tangent5.z,			bitangent5.x, bitangent5.y, bitangent5.z },	
			{ pos9.x,  pos9.y,  pos9.z,		-1.0f, 0.0f, 0.0f,		uv9.x,  uv9.y,		tangent6.x, tangent6.y, tangent6.z,			bitangent6.x, bitangent6.y, bitangent6.z },	
			{ pos11.x, pos11.y, pos11.z,	-1.0f, 0.0f, 0.0f,		uv11.x, uv11.y,		tangent6.x, tangent6.y, tangent6.z,			bitangent6.x, bitangent6.y, bitangent6.z },	
			{ pos12.x, pos12.y, pos12.z,	-1.0f, 0.0f, 0.0f,		uv12.x, uv12.y,		tangent6.x, tangent6.y, tangent6.z,			bitangent6.x, bitangent6.y, bitangent6.z },	
			// back square
			{ pos13.x, pos13.y, pos13.z,	0.0f, 0.0f, -1.0f,		uv13.x, uv13.y,		tangent7.x, tangent7.y, tangent7.z,			bitangent7.x, bitangent7.y, bitangent7.z },	
			{ pos14.x, pos14.y, pos14.z,	0.0f, 0.0f, -1.0f,		uv14.x, uv14.y,		tangent7.x, tangent7.y, tangent7.z,			bitangent7.x, bitangent7.y, bitangent7.z },	
			{ pos15.x, pos15.y, pos15.z,	0.0f, 0.0f, -1.0f,		uv15.x, uv15.y,		tangent7.x, tangent7.y, tangent7.z,			bitangent7.x, bitangent7.y, bitangent7.z },	
			{ pos13.x, pos13.y, pos13.z,	0.0f, 0.0f, -1.0f,		uv13.x, uv13.y,		tangent8.x, tangent8.y, tangent8.z,			bitangent8.x, bitangent8.y, bitangent8.z },	
			{ pos15.x, pos15.y, pos15.z,	0.0f, 0.0f, -1.0f,		uv15.x, uv15.y,		tangent8.x, tangent8.y, tangent8.z,			bitangent8.x, bitangent8.y, bitangent8.z },	
			{ pos16.x, pos16.y, pos16.z,	0.0f, 0.0f, -1.0f,		uv16.x, uv16.y,		tangent8.x, tangent8.y, tangent8.z,			bitangent8.x, bitangent8.y, bitangent8.z },	
			// top square
			{ pos17.x, pos17.y, pos17.z,	0.0f, 1.0f, 0.0f,		uv17.x, uv17.y,		tangent9.x , tangent9.y , tangent9.z ,		bitangent9.x , bitangent9.y , bitangent9.z },	
			{ pos18.x, pos18.y, pos18.z,	0.0f, 1.0f, 0.0f,		uv18.x, uv18.y,		tangent9.x , tangent9.y , tangent9.z ,		bitangent9.x , bitangent9.y , bitangent9.z },	
			{ pos19.x, pos19.y, pos19.z,	0.0f, 1.0f, 0.0f,		uv19.x, uv19.y,		tangent9.x , tangent9.y , tangent9.z ,		bitangent9.x , bitangent9.y , bitangent9.z },	
			{ pos17.x, pos17.y, pos17.z,	0.0f, 1.0f, 0.0f,		uv17.x, uv17.y,		tangent10.x, tangent10.y, tangent10.z,		bitangent10.x, bitangent10.y, bitangent10.z },	
			{ pos19.x, pos19.y, pos19.z,	0.0f, 1.0f, 0.0f,		uv19.x, uv19.y,		tangent10.x, tangent10.y, tangent10.z,		bitangent10.x, bitangent10.y, bitangent10.z },	
			{ pos20.x, pos20.y, pos20.z,	0.0f, 1.0f, 0.0f,		uv20.x, uv20.y,		tangent10.x, tangent10.y, tangent10.z,		bitangent10.x, bitangent10.y, bitangent10.z },	
			// bottom square
			{ pos21.x, pos21.y, pos21.z,	0.0f, -1.0f, 0.0f,		uv21.x, uv21.y,		tangent11.x, tangent11.y, tangent11.z,		bitangent11.x, bitangent11.y, bitangent11.z },	
			{ pos22.x, pos22.y, pos22.z,	0.0f, -1.0f, 0.0f,		uv22.x, uv22.y,		tangent11.x, tangent11.y, tangent11.z,		bitangent11.x, bitangent11.y, bitangent11.z },	
			{ pos23.x, pos23.y, pos23.z,	0.0f, -1.0f, 0.0f,		uv23.x, uv23.y,		tangent11.x, tangent11.y, tangent11.z,		bitangent11.x, bitangent11.y, bitangent11.z },	
			{ pos21.x, pos21.y, pos21.z,	0.0f, -1.0f, 0.0f,		uv21.x, uv21.y,		tangent12.x, tangent12.y, tangent12.z,		bitangent12.x, bitangent12.y, bitangent12.z },	
			{ pos23.x, pos23.y, pos23.z,	0.0f, -1.0f, 0.0f,		uv23.x, uv23.y,		tangent12.x, tangent12.y, tangent12.z,		bitangent12.x, bitangent12.y, bitangent12.z },	
			{ pos24.x, pos24.y, pos24.z,	0.0f, -1.0f, 0.0f,		uv24.x, uv24.y,		tangent12.x, tangent12.y, tangent12.z,		bitangent12.x, bitangent12.y, bitangent12.z }	
	};




























	normal_map_vao = NormalVAO(normal_map_vbo);
	
	framebuffer_vbo = {
		// Coords			// texCoords
		{-1.0f,  1.0f,		0.0f, 1.0f},
		{ 1.0f, -1.0f,		1.0f, 0.0f},
		{-1.0f, -1.0f,		0.0f, 0.0f},
		{-1.0f,  1.0f,		0.0f, 1.0f},
		{ 1.0f,  1.0f,		1.0f, 1.0f},
		{ 1.0f, -1.0f,		1.0f, 0.0f}
	};
	framebuffer_vao = FramebufferVAO(framebuffer_vbo);


	// Creating and binding framebuffer

	MultisampledTexture* anti_aliasing_fbo_textures[] = { &anti_aliasing_fbo_tex1, &anti_aliasing_fbo_tex2 };

	anti_aliasing_fbo = { width, height, samples, anti_aliasing_fbo_textures, sizeof(anti_aliasing_fbo_textures)/sizeof(anti_aliasing_fbo_textures[0])};

	Texture* fbo_textures[] = {&post_proc_fbo_tex1, &post_proc_fbo_tex2};

	post_proc_fbo = { width, height, fbo_textures, sizeof(fbo_textures)/sizeof(fbo_textures[0])};


	directional_shadow_fbo = { shadow_size, shadow_size, directional_shadow_fbo_tex };

	spot_shadow_fbo = {shadow_size, shadow_size, spot_shadow_fbo_tex};

	cubemap_shadow_fbo1 = { cubemap_shadow_size, cubemap_shadow_size, point_shadow_fbo_tex1 };
	cubemap_shadow_fbo2 = { cubemap_shadow_size, cubemap_shadow_size, point_shadow_fbo_tex2 };
	cubemap_shadow_fbo3 = { cubemap_shadow_size, cubemap_shadow_size, point_shadow_fbo_tex3 };
	cubemap_shadow_fbo4 = { cubemap_shadow_size, cubemap_shadow_size, point_shadow_fbo_tex4 };


	ping_fbo = { width, height, ping_fbo_tex };
	pong_fbo = { width, height, pong_fbo_tex };

}

void Window::processInput(GLFWwindow* window) {
	// Closes the window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Processes movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyPress(FORWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyPress(BACKWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyPress(RIGHT, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyPress(LEFT, delta_time);

	// Flashlight
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		flashlight = true;
	else
		flashlight = false;
}

// CALLBACKS_____________________________________________________________________________________________

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// Changes the viewport
	glViewport(0, 0, width, height);


	window_state_map[window]->anti_aliasing_fbo_tex1.BindMultisample();
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, window_state_map[window]->samples, GL_RGB, width, height, GL_TRUE);
	window_state_map[window]->anti_aliasing_fbo_tex2.BindMultisample();
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, window_state_map[window]->samples, GL_RGB, width, height, GL_TRUE);
	window_state_map[window]->anti_aliasing_fbo.BindRenderbuffer();
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, window_state_map[window]->samples, GL_DEPTH24_STENCIL8, width, height);


	window_state_map[window]->post_proc_fbo_tex1.Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	window_state_map[window]->post_proc_fbo_tex2.Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	window_state_map[window]->post_proc_fbo.BindRenderbuffer();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);



	window_state_map[window]->ping_fbo_tex.Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	window_state_map[window]->pong_fbo_tex.Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	//window_state_map[window]->directional_shadow_fbo_tex.Bind();
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, window_state_map[window]->shadow_size, window_state_map[window]->shadow_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//
	//window_state_map[window]->point_shadow_fbo_tex1.Bind();
	//for (unsigned int i = 0; i < 6; i++)
	//	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, window_state_map[window]->cubemap_shadow_size, window_state_map[window]->cubemap_shadow_size, 0, format, type, NULL);

	window_state_map[window]->width = width;
	window_state_map[window]->height = height;
}

void Window::mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
	// Checks for big jumps if mouse is far away from the screen center
	if (window_state_map[window]->first_mouse) {
		window_state_map[window]->last_x = x_pos;
		window_state_map[window]->last_y = y_pos;
		window_state_map[window]->first_mouse = false;
	}
	float x_offset = x_pos - window_state_map[window]->last_x;
	float y_offset = window_state_map[window]->last_y - y_pos;
	window_state_map[window]->last_x = x_pos;
	window_state_map[window]->last_y = y_pos;

	window_state_map[window]->camera.ProcessMouseMove(x_offset, y_offset);
}

void Window::scroll_callback(GLFWwindow* window, double x_scroll, double y_scroll) {
	// Changes the camera_fov
	window_state_map[window]->camera_fov -= (float)y_scroll;
	// Restrictions for the camera_fov
	if (window_state_map[window]->camera_fov < 1.0f)
		window_state_map[window]->camera_fov = 1.0f;
	if (window_state_map[window]->camera_fov > 90.0f)
		window_state_map[window]->camera_fov = 90.0f;
}

void Window::Go() {

	const unsigned int light_amount = 4;
	glm::mat4 light_pos_mat[light_amount];
	glm::vec3 light_col_vec[light_amount];

	glm::vec3 point_light_pos[light_amount] = {
		glm::vec3(-1.0f,   1.0f,  -3.0f),
		glm::vec3(-2.0f,   3.0f,   2.0f),
		glm::vec3(-4.0f,   2.0f,   0.5f),
		glm::vec3( 2.0f,  -2.0f,  -3.5f)
	};

	glm::vec3 point_light_colors[light_amount] = {
		glm::vec3(4.0, 0.0, 4.0),
		glm::vec3(4.0, 4.0, 0.0),
		glm::vec3(0.0, 4.0, 4.0),
		glm::vec3(5.0, 0.0, 0.0)
	};

	for (unsigned int i = 0; i < light_amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, point_light_pos[i]);
		model = glm::scale(model, glm::vec3(0.3f));
		light_pos_mat[i] = model;
		light_col_vec[i] = point_light_colors[i];
	}

	light_pos_storage = { light_pos_mat, light_amount };

	vao_light.MatInstance(3, light_pos_storage);

	light_col_storage = { light_col_vec, light_amount };

	vao_light.VecInstance(7, light_col_storage);



	const unsigned int objects_amount = 10;
	glm::mat4 cubes_pos_mat[objects_amount];
	glm::mat4 grass_pos_mat[objects_amount];
	glm::mat4 windows_pos_mat[objects_amount];

	// cube's positions
	glm::vec3 cubes_positions[objects_amount] = {
		glm::vec3(-5.5f,   -6.2f,      -15.7f),
		glm::vec3( 0.2f,	0.7f,      -4.8f),
		glm::vec3(-2.9f,   -0.4f,      -3.9f),
		glm::vec3( 2.8f,   -3.0f,      -3.2f),
		glm::vec3( 2.0f,	1.3f,      -5.6f),
		glm::vec3(-0.8f,	3.9f,      -6.6f),
		glm::vec3( 0.0f,	0.0f,		3.0f),
		glm::vec3(-20.6f,   3.4f,     -13.4f),
		glm::vec3(-4.4f,   -5.8f,      -6.4f),
		glm::vec3(-6.0f,   -14.75f,    -1.0f)
	};

	for (unsigned int i = 0; i < objects_amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubes_positions[i]);
		if (i == objects_amount - 1 || i == objects_amount - 3)
			model = glm::scale(model, glm::vec3(30.0f, 20.0f, 60.0f));

		grass_pos_mat[i] = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));

		windows_pos_mat[i] = glm::translate(model, glm::vec3(0.0f, 3.0f, 2.0f));

		if (i % 3 == 0)
			model = glm::rotate(model, (float)(glm::radians(0.0f)), glm::vec3(1.0f, 1.0f, 1.0f));
		else
			model = glm::rotate(model, -(float)(i * 0.0), glm::vec3(0.2f, 1.0f, 0.5f));

		cubes_pos_mat[i] = model;
	}

	cubes_pos_storage = { cubes_pos_mat, objects_amount };

	vao_model.MatInstance(3, cubes_pos_storage);

	grass_pos_storage = { grass_pos_mat, objects_amount };

	grass_transparent_vao.MatInstance(3, grass_pos_storage);

	windows_pos_storage = {windows_pos_mat, objects_amount};

	windows_transparent_vao.MatInstance(3, windows_pos_storage);

	int objects_pos_size = sizeof(cubes_positions) / sizeof(cubes_positions[0]);

	const unsigned int normal_map_cubes_amount = 3;
	glm::mat4 normal_map_cubes_mat[normal_map_cubes_amount];

	glm::vec3 normal_map_cubes_pos[normal_map_cubes_amount] = {
		glm::vec3(-1.2f,   -2.5f,    -3.3f),
		glm::vec3(2.0f,   -1.6f,    -2.4f),
		glm::vec3(2.2f,   -2.5f,    -1.3f)
	};

	for (unsigned int i = 0; i < normal_map_cubes_amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, normal_map_cubes_pos[i]);

		

		if (i % 2 == 0)
			model = glm::rotate(model, (float)(glm::radians(40.0f)), glm::vec3(1.0f, 1.0f, 1.0f));
		else
			model = glm::rotate(model, -(float)(i * 20.0), glm::vec3(0.2f, 1.0f, 0.5f));

		normal_map_cubes_mat[i] = model;
	}

	normal_map_vbo_storage = { normal_map_cubes_mat, normal_map_cubes_amount };

	normal_map_vao.MatInstance(5, normal_map_vbo_storage);


	const unsigned int reflected_models_amount = 2;
	glm::mat4 reflected_models_pos_mat[reflected_models_amount];
	glm::vec3 reflected_models_positions[reflected_models_amount] = {
		glm::vec3(0.0f, 4.0f, -2.0f),
		glm::vec3(1.0f, 4.0f, -4.0f)
	};
	for (unsigned int i = 0; i < reflected_models_amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, reflected_models_positions[i]);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(2.0f, 2.0f, 2.0f));
		reflected_models_pos_mat[i] = model;
	}

	reflected_model = { "objects/mushroom/model.fbx" , reflected_models_pos_mat, reflected_models_amount };



	const unsigned int models_amount = 2;
	glm::mat4 models_pos_mat[models_amount];
	glm::vec3 models_positions[models_amount] = {
		glm::vec3(-6.0f, -6.0f, -4.0f),
		glm::vec3(-3.0f, 5.0f, 3.0f)
	};
	for (unsigned int i = 0; i < models_amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, models_positions[i]);
		model = glm::scale(model, glm::vec3(1.1f,1.1f,1.1f));
		if(i == 0)
			model = glm::rotate(model, glm::radians(120.0f), glm::vec3(1.0f, 1.0f, 0.0f));
		models_pos_mat[i] = model;
	}
	// model's path
	loaded_model = { "objects/mushroom/model.fbx", models_pos_mat, models_amount };


	const int point_light_size = sizeof(point_light_pos) / sizeof(point_light_pos[0]);

	glm::vec3 dir_light_color = glm::vec3(1.0, 1.0, 1.0);

	glm::vec3 spot_light_color = glm::vec3(1.0, 1.0, 1.0);

	glm::vec3 direct_light_pos = glm::vec3(1.0f, 60.0f, 50.0f);







	









	while (!glfwWindowShouldClose(window))
	{
		// For proper movement for every PC
		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		processInput(window);

		glClearColor(0.9f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		// DIRECTIONAL SHADOW MAP FRAMEBUFFER__________________________________________________________________________________________________________________
		glm::mat4 light_projection = get_projection_ortho(near_plane_shadow, far_plane_shadow);
		glm::mat4 light_view = glm::lookAt(direct_light_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 light_space_matrix = light_projection * light_view;
		light_matrix_ubo.SubData(0, sizeof(glm::mat4), &light_space_matrix);

		glViewport(0, 0, shadow_size, shadow_size);
		directional_shadow_fbo.BindFramebuffer();

		// sorts map by distance between direct_light and object positions
		std::map<float, glm::vec3> sorted_direct;
		for (unsigned int i = 0; i < objects_pos_size; i++) {
			float distance = glm::length(direct_light_pos - cubes_positions[i]);	// distance between camera and object position
			sorted_direct[distance] = cubes_positions[i];	// map initialising
		}

		// anti peter panning
		glEnable(GL_CULL_FACE);
		glClear(GL_DEPTH_BUFFER_BIT);

		shadow_objects.Activate();
		render_shadow_map(shadow_objects, discard_shadow, normal_map_shadow, sorted_direct, windows_pos_mat, grass_pos_mat, objects_amount, normal_map_cubes_amount, reflected_models_amount, models_amount);
		directional_shadow_fbo.UnbindFramebuffer();







		//POINT SHADOW MAP FRAMEBUFFER___________________________________________________________________________________________________________________
		glm::mat4 shadow_proj = get_projection_perspective(90.0f, (float)cubemap_shadow_size / (float)cubemap_shadow_size, near_plane_cubamap_shadow, far_plane_cubemap_shadow);
		std::vector<glm::mat4> shadow_transforms;
		std::map<float, glm::vec3> point_light_sorted;

		// 1 POINT SHADW MAP_____________________________________________________________________________________________________________________________
		glViewport(0, 0, cubemap_shadow_size, cubemap_shadow_size);
		cubemap_shadow_fbo1.BindFramebuffer();

		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		shadow_transforms = CalcPointShadowMatrices(shadow_proj, point_light_pos[0]);
		for (unsigned int i = 0; i < 6; i++) {
			std::string point_shadow_matrices_variable = "shadow_matrices[" + std::to_string(i) + "]";
			point_shadow_objects.Activate();
			point_shadow_objects.setMat4(point_shadow_matrices_variable.c_str(), shadow_transforms[i]);
			point_discard_shadow.Activate();
			point_discard_shadow.setMat4(point_shadow_matrices_variable.c_str(), shadow_transforms[i]);
			point_normal_map_shadow.Activate();
			point_normal_map_shadow.setMat4(point_shadow_matrices_variable.c_str(), shadow_transforms[i]);
		}

		// sorts map by distance between light and object positions
		for (unsigned int i = 0; i < objects_pos_size; i++) {
			float distance = glm::length(point_light_pos[0] - cubes_positions[i]);	// distance between camera and object position
			point_light_sorted[distance] = cubes_positions[i];	// map initialising
		}

		// anti peter panning
		glEnable(GL_CULL_FACE);
		glClear(GL_DEPTH_BUFFER_BIT);
		// anti peter panning
		glCullFace(GL_FRONT);

		point_shadow_objects.Activate();

		point_light_matrix_ubo.SubData(0, sizeof(glm::vec3), &point_light_pos[0]);
		point_light_matrix_ubo.SubData(sizeof(glm::vec3), sizeof(float), &far_plane_cubemap_shadow);

		render_shadow_map(point_shadow_objects, point_discard_shadow, point_normal_map_shadow, point_light_sorted, windows_pos_mat, grass_pos_mat, objects_amount, normal_map_cubes_amount, reflected_models_amount, models_amount);
		point_light_sorted.clear();

		point_shadow_map_id[0] = point_shadow_fbo_tex1.GetId();

		cubemap_shadow_fbo1.UnbindFramebuffer();






		// 2 POINT SHADW MAP_____________________________________________________________________________________________________________________________
		glViewport(0, 0, cubemap_shadow_size, cubemap_shadow_size);
		cubemap_shadow_fbo2.BindFramebuffer();

		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		shadow_transforms = CalcPointShadowMatrices(shadow_proj, point_light_pos[1]);
		for (unsigned int i = 0; i < 6; i++) {
			std::string point_shadow_matrices_variable = "shadow_matrices[" + std::to_string(i) + "]";
			point_shadow_objects.Activate();
			point_shadow_objects.setMat4(point_shadow_matrices_variable.c_str(), shadow_transforms[i]);
			point_discard_shadow.Activate();
			point_discard_shadow.setMat4(point_shadow_matrices_variable.c_str(), shadow_transforms[i]);
			point_normal_map_shadow.Activate();
			point_normal_map_shadow.setMat4(point_shadow_matrices_variable.c_str(), shadow_transforms[i]);
		}

		// sorts map by distance between light and object positions
		for (unsigned int i = 0; i < objects_pos_size; i++) {
			float distance = glm::length(point_light_pos[1] - cubes_positions[i]);	// distance between camera and object position
			point_light_sorted[distance] = cubes_positions[i];	// map initialising
		}

		// anti peter panning
		glEnable(GL_CULL_FACE);
		glClear(GL_DEPTH_BUFFER_BIT);
		// anti peter panning
		glCullFace(GL_FRONT);

		point_shadow_objects.Activate();

		point_light_matrix_ubo.SubData(0, sizeof(glm::vec3), &point_light_pos[1]);
		point_light_matrix_ubo.SubData(sizeof(glm::vec3), sizeof(float), &far_plane_cubemap_shadow);

		render_shadow_map(point_shadow_objects, point_discard_shadow, point_normal_map_shadow, point_light_sorted, windows_pos_mat, grass_pos_mat, objects_amount, normal_map_cubes_amount, reflected_models_amount, models_amount);
		point_light_sorted.clear();

		point_shadow_map_id[1] = point_shadow_fbo_tex2.GetId();

		cubemap_shadow_fbo2.UnbindFramebuffer();

























		// 3 POINT SHADW MAP_____________________________________________________________________________________________________________________________	
		glViewport(0, 0, cubemap_shadow_size, cubemap_shadow_size);
		cubemap_shadow_fbo3.BindFramebuffer();

		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		shadow_transforms = CalcPointShadowMatrices(shadow_proj, point_light_pos[2]);
		for (unsigned int i = 0; i < 6; i++) {
			std::string point_shadow_matrices_variable = "shadow_matrices[" + std::to_string(i) + "]";
			point_shadow_objects.Activate();
			point_shadow_objects.setMat4(point_shadow_matrices_variable.c_str(), shadow_transforms[i]);
			point_discard_shadow.Activate();
			point_discard_shadow.setMat4(point_shadow_matrices_variable.c_str(), shadow_transforms[i]);
			point_normal_map_shadow.Activate();
			point_normal_map_shadow.setMat4(point_shadow_matrices_variable.c_str(), shadow_transforms[i]);
		}

		// sorts map by distance between light and object positions
		for (unsigned int i = 0; i < objects_pos_size; i++) {
			float distance = glm::length(point_light_pos[2] - cubes_positions[i]);	// distance between camera and object position
			point_light_sorted[distance] = cubes_positions[i];	// map initialising
		}

		// anti peter panning
		glEnable(GL_CULL_FACE);
		glClear(GL_DEPTH_BUFFER_BIT);
		// anti peter panning
		glCullFace(GL_FRONT);

		point_shadow_objects.Activate();

		point_light_matrix_ubo.SubData(0, sizeof(glm::vec3), &point_light_pos[2]);
		point_light_matrix_ubo.SubData(sizeof(glm::vec3), sizeof(float), &far_plane_cubemap_shadow);

		render_shadow_map(point_shadow_objects, point_discard_shadow, point_normal_map_shadow, point_light_sorted, windows_pos_mat, grass_pos_mat, objects_amount, normal_map_cubes_amount, reflected_models_amount, models_amount);
		point_light_sorted.clear();

		point_shadow_map_id[2] = point_shadow_fbo_tex3.GetId();

		cubemap_shadow_fbo3.UnbindFramebuffer();

		// 4 POINT SHADW MAP_____________________________________________________________________________________________________________________________	
		glViewport(0, 0, cubemap_shadow_size, cubemap_shadow_size);
		cubemap_shadow_fbo4.BindFramebuffer();

		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		shadow_transforms = CalcPointShadowMatrices(shadow_proj, point_light_pos[3]);
		for (unsigned int i = 0; i < 6; i++) {
			std::string point_shadow_matrices_variable = "shadow_matrices[" + std::to_string(i) + "]";
			point_shadow_objects.Activate();
			point_shadow_objects.setMat4(point_shadow_matrices_variable.c_str(), shadow_transforms[i]);
			point_discard_shadow.Activate();
			point_discard_shadow.setMat4(point_shadow_matrices_variable.c_str(), shadow_transforms[i]);
			point_normal_map_shadow.Activate();
			point_normal_map_shadow.setMat4(point_shadow_matrices_variable.c_str(), shadow_transforms[i]);
		}

		// sorts map by distance between light and object positions
		for (unsigned int i = 0; i < objects_pos_size; i++) {
			float distance = glm::length(point_light_pos[3] - cubes_positions[i]);	// distance between camera and object position
			point_light_sorted[distance] = cubes_positions[i];	// map initialising
		}

		// anti peter panning
		glEnable(GL_CULL_FACE);
		glClear(GL_DEPTH_BUFFER_BIT);
		// anti peter panning
		glCullFace(GL_FRONT);

		point_shadow_objects.Activate();

		point_light_matrix_ubo.SubData(0, sizeof(glm::vec3), &point_light_pos[3]);
		point_light_matrix_ubo.SubData(sizeof(glm::vec3), sizeof(float), &far_plane_cubemap_shadow);

		render_shadow_map(point_shadow_objects, point_discard_shadow, point_normal_map_shadow, point_light_sorted, windows_pos_mat, grass_pos_mat, objects_amount, normal_map_cubes_amount, reflected_models_amount, models_amount);
		point_light_sorted.clear();

		point_shadow_map_id[3] = point_shadow_fbo_tex4.GetId();

		cubemap_shadow_fbo4.UnbindFramebuffer();



		// SPOT SHADOW MAP FRAMEBUFFER__________________________________________________________________________________________________________________
		glClearColor(0.9f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glm::mat4 spot_light_projection = get_projection_perspective(camera_fov, 1.0f, near_plane_camera, far_plane_camera);
		glm::mat4 spot_light_view = camera.GetViewMatrix();
		glm::mat4 spot_light_space_matrix = spot_light_projection * spot_light_view;
		light_matrix_ubo.SubData(0, sizeof(glm::mat4), &spot_light_space_matrix);

		glViewport(0, 0, shadow_size, shadow_size);
		spot_shadow_fbo.BindFramebuffer();

		// anti peter panning
		glEnable(GL_CULL_FACE);
		glClear(GL_DEPTH_BUFFER_BIT);

		// anti peter panning
		glCullFace(GL_FRONT);

		// sorts map by distance between camera and object positions
		std::map<float, glm::vec3> sorted;
		for (unsigned int i = 0; i < objects_pos_size; i++) {
			float distance = glm::length(camera.GetCameraPosition() - cubes_positions[i]);	// distance between camera and object position
			sorted[distance] = cubes_positions[i];	// map initialising
		}

		// CUBES______________________________________________________________________________________________________________________________________
		shadow_objects.Activate();

		render_shadow_map(shadow_objects, discard_shadow, normal_map_shadow, sorted, windows_pos_mat, grass_pos_mat, objects_amount, normal_map_cubes_amount, reflected_models_amount, models_amount);

		spot_shadow_fbo.UnbindFramebuffer();




		// POST PROCESSING FRAMEBUFFER_________________________________________________________________________________________________________________
		glViewport(0, 0, width, height);
		anti_aliasing_fbo.BindFramebuffer();
		//anti_aliasing_fbo.BindFramebuffer();
		
		glEnable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		//glStencilMask(0x00);
		glEnable(GL_DEPTH_TEST);

		light_matrix_ubo.SubData(0, sizeof(glm::mat4), &light_space_matrix);



		glm::mat4 projection = get_projection_perspective(camera_fov, float(width)/ float(height), near_plane_camera, far_plane_camera);
		pos_matrix_ubo.SubData(0, sizeof(glm::mat4), &projection);

		glm::mat4 view = camera.GetViewMatrix();
		pos_matrix_ubo.SubData(sizeof(glm::mat4), sizeof(glm::mat4), &view);


		normal_map_shader.Activate();
		normal_map_shader.setInt("shadow_map_direct", directional_shadow_fbo_tex.GetId());
		normal_map_shader.setInt("shadow_map_spot", spot_shadow_fbo_tex.GetId());
		for (unsigned int i = 0; i < sizeof(point_shadow_map_id) / sizeof(point_shadow_map_id[0]); i++)
			normal_map_shader.setInt(("shadow_map_point[" + std::to_string(i) + "]").c_str(), point_shadow_map_id[i]);
		normal_map_shader.setFloat("material.shininess", 255.0f);

		object_shader.Activate();
		glm::vec3 camera_position = camera.GetCameraPosition();
		glm::vec3 camera_front = camera.GetCameraFront();
		light_data_ubo.SubData(0, sizeof(glm::vec4), &camera_position);
		object_shader.setInt("shadow_map_direct", directional_shadow_fbo_tex.GetId());
		object_shader.setInt("shadow_map_spot", spot_shadow_fbo_tex.GetId());
		for (unsigned int i = 0; i < sizeof(point_shadow_map_id) / sizeof(point_shadow_map_id[0]); i++) 
			object_shader.setInt(("shadow_map_point[" + std::to_string(i) + "]").c_str(), point_shadow_map_id[i]);

		// LIGHTING__________________________________________________________________________________________________________________
		// point
		glm::vec3 point_ambient_power[point_light_size];
		glm::vec3 point_diffuse_power[point_light_size];
		glm::vec3 point_specular_power[point_light_size];
		
		object_shader.setFloat("material.shininess", 255.0f);
		
		dir_light_settings(dir_light_color, object_shader, light_data_ubo);
		
		// Point light
		//for (int i = 0; i < point_light_size; i++) {
		//	// lighting maps power
		//	point_ambient_power[i] = point_light_colors[i] * glm::vec3(0.05f);
		//	point_diffuse_power[i] = point_light_colors[i] * glm::vec3(0.5f);
		//	point_specular_power[i] = point_light_colors[i] * glm::vec3(1.0f);
		//
		//	light_data_ubo.SubData(sizeof(glm::vec4) * 5, sizeof(glm::vec4), &point_light_pos[i]);
		//	float point_light_constant = 1.0f;
		//	light_data_ubo.SubData(sizeof(glm::vec4) * 6, sizeof(float), &point_light_constant);
		//	float point_light_linear = 0.09f;
		//	light_data_ubo.SubData(sizeof(glm::vec4) * 6 + sizeof(float), sizeof(float), &point_light_linear);
		//	float point_light_quadratic = 0.032f;
		//	light_data_ubo.SubData(sizeof(glm::vec4) * 6 + sizeof(float) * 2, sizeof(float), &point_light_quadratic);
		//	light_data_ubo.SubData(sizeof(glm::vec4) * 6 + sizeof(float) * 3, sizeof(glm::vec4), &point_ambient_power[i]);
		//	light_data_ubo.SubData(sizeof(glm::vec4) * 7 + sizeof(float) * 3, sizeof(glm::vec4), &point_diffuse_power[i]);
		//	light_data_ubo.SubData(sizeof(glm::vec4) * 8 + sizeof(float) * 3, sizeof(glm::vec4), &point_specular_power[i]);
		//}
		// 
		


			// lighting maps power
			point_ambient_power[0] = point_light_colors[0] * glm::vec3(0.05f);
			point_diffuse_power[0] = point_light_colors[0] * glm::vec3(0.5f);
			point_specular_power[0] = point_light_colors[0] * glm::vec3(1.0f);

			light_data_ubo.SubData(sizeof(glm::vec4) * 5, sizeof(glm::vec4), &point_light_pos[0]);
			light_data_ubo.SubData(sizeof(glm::vec4) * 6, sizeof(glm::vec4), &point_ambient_power[0]);
			light_data_ubo.SubData(sizeof(glm::vec4) * 7, sizeof(glm::vec4), &point_diffuse_power[0]);
			light_data_ubo.SubData(sizeof(glm::vec4) * 8, sizeof(glm::vec3), &point_specular_power[0]);
			float point_light_constant = 1.0f;
			light_data_ubo.SubData(sizeof(glm::vec4) * 8 + sizeof(glm::vec3), sizeof(float), &point_light_constant);
			float point_light_linear = 0.09f;
			light_data_ubo.SubData(sizeof(glm::vec4) * 8 + sizeof(glm::vec3) + sizeof(float), sizeof(float), &point_light_linear);
			float point_light_quadratic = 0.032f;
			light_data_ubo.SubData(sizeof(glm::vec4) * 8 + sizeof(glm::vec3) + sizeof(float) * 2, sizeof(float), &point_light_quadratic);



			point_ambient_power[1] = point_light_colors[1] * glm::vec3(0.05f);
			point_diffuse_power[1] = point_light_colors[1] * glm::vec3(0.5f);
			point_specular_power[1] = point_light_colors[1] * glm::vec3(1.0f);

			light_data_ubo.SubData(sizeof(glm::vec4) * 8 + sizeof(glm::vec3) + sizeof(float) * 5, sizeof(glm::vec4), &point_light_pos[1]);
			light_data_ubo.SubData(sizeof(glm::vec4) * 9 + sizeof(glm::vec3) + sizeof(float) * 5, sizeof(glm::vec4), &point_ambient_power[1]);
			light_data_ubo.SubData(sizeof(glm::vec4) * 10 + sizeof(glm::vec3) + sizeof(float) * 5, sizeof(glm::vec4), &point_diffuse_power[1]);
			light_data_ubo.SubData(sizeof(glm::vec4) * 11 + sizeof(glm::vec3) + sizeof(float) * 5, sizeof(glm::vec3), &point_specular_power[1]);
			light_data_ubo.SubData(sizeof(glm::vec4) * 11 + sizeof(glm::vec3) * 2 + sizeof(float) * 5, sizeof(float), &point_light_constant);
			light_data_ubo.SubData(sizeof(glm::vec4) * 11 + sizeof(glm::vec3) * 2 + sizeof(float) * 6, sizeof(float), &point_light_linear);
			light_data_ubo.SubData(sizeof(glm::vec4) * 11 + sizeof(glm::vec3) * 2 + sizeof(float) * 7, sizeof(float), &point_light_quadratic);



			point_ambient_power[2] = point_light_colors[2] * glm::vec3(0.05f);
			point_diffuse_power[2] = point_light_colors[2] * glm::vec3(0.5f);
			point_specular_power[2] = point_light_colors[2] * glm::vec3(1.0f);

			light_data_ubo.SubData(sizeof(glm::vec4) * 11 + sizeof(glm::vec3) * 2 + sizeof(float) * 10, sizeof(glm::vec4), &point_light_pos[2]);
			light_data_ubo.SubData(sizeof(glm::vec4) * 12 + sizeof(glm::vec3) * 2 + sizeof(float) * 10, sizeof(glm::vec4), &point_ambient_power[2]);
			light_data_ubo.SubData(sizeof(glm::vec4) * 13 + sizeof(glm::vec3) * 2 + sizeof(float) * 10, sizeof(glm::vec4), &point_diffuse_power[2]);
			light_data_ubo.SubData(sizeof(glm::vec4) * 14 + sizeof(glm::vec3) * 2 + sizeof(float) * 10, sizeof(glm::vec3), &point_specular_power[2]);
			light_data_ubo.SubData(sizeof(glm::vec4) * 14 + sizeof(glm::vec3) * 3 + sizeof(float) * 10, sizeof(float), &point_light_constant);
			light_data_ubo.SubData(sizeof(glm::vec4) * 14 + sizeof(glm::vec3) * 3 + sizeof(float) * 11, sizeof(float), &point_light_linear);
			light_data_ubo.SubData(sizeof(glm::vec4) * 14 + sizeof(glm::vec3) * 3 + sizeof(float) * 12, sizeof(float), &point_light_quadratic);




			point_ambient_power[3] = point_light_colors[3] * glm::vec3(0.05f);
			point_diffuse_power[3] = point_light_colors[3] * glm::vec3(0.5f);
			point_specular_power[3] = point_light_colors[3] * glm::vec3(1.0f);

			light_data_ubo.SubData(sizeof(glm::vec4) * 14 + sizeof(glm::vec3) * 3 + sizeof(float) * 15, sizeof(glm::vec4), &point_light_pos[3]);
			light_data_ubo.SubData(sizeof(glm::vec4) * 15 + sizeof(glm::vec3) * 3 + sizeof(float) * 15, sizeof(glm::vec4), &point_ambient_power[3]);
			light_data_ubo.SubData(sizeof(glm::vec4) * 16 + sizeof(glm::vec3) * 3 + sizeof(float) * 15, sizeof(glm::vec4), &point_diffuse_power[3]);
			light_data_ubo.SubData(sizeof(glm::vec4) * 17 + sizeof(glm::vec3) * 3 + sizeof(float) * 15, sizeof(glm::vec3), &point_specular_power[3]);
			light_data_ubo.SubData(sizeof(glm::vec4) * 17 + sizeof(glm::vec3) * 4 + sizeof(float) * 15, sizeof(float), &point_light_constant);
			light_data_ubo.SubData(sizeof(glm::vec4) * 17 + sizeof(glm::vec3) * 4 + sizeof(float) * 16, sizeof(float), &point_light_linear);
			light_data_ubo.SubData(sizeof(glm::vec4) * 17 + sizeof(glm::vec3) * 4 + sizeof(float) * 17, sizeof(float), &point_light_quadratic);



		//point_light_settings(point_light_size, point_light_pos, point_ambient_power, point_diffuse_power, point_specular_power, point_light_colors);
		
		spot_light_settings(spot_light_color, camera_position, camera_front, object_shader, light_data_ubo);

		light_data_ubo.SubData(sizeof(glm::vec4) * 21 + sizeof(glm::vec3) * 5 + sizeof(float) * 25, sizeof(float), &far_plane_cubemap_shadow);

		
		// CUBES____________________________________________________________________________________________________________________________
		object_shader.setInt("material.texture_diffuse1", diffuse_map.GetId());
		object_shader.setInt("material.texture_specular1", specular_map.GetId());
		vao_model.Bind();
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, objects_amount);

		normal_map_shader.Activate();
		normal_map_shader.setInt("material.texture_diffuse1", diffuse_brick_map.GetId());
		normal_map_shader.setInt("material.texture_specular1", black_specular_map.GetId());
		normal_map_shader.setInt("material.normal_map1", normal_map_tex.GetId());
		normal_map_shader.setInt("material.displacement_map1", displacement_map_tex.GetId());
		normal_map_shader.setBool("normal_map", true);
		normal_map_shader.setBool("parallax_map", true);
		normal_map_shader.setFloat("height_scale", 0.1f);
		normal_map_vao.Bind();
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, normal_map_cubes_amount);
		
		
		// LIGHTING BLOCKS____________________________________________________________________________________________________________________
		instancing_shader.Activate();
		vao_light.Bind();
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, light_amount);
		vao_light.Unbind();
		
		
		// CUBEMAP____________________________________________________________________________________________________________________________
		glDepthFunc(GL_LEQUAL);
		cubemap_shader.Activate();
		cubemap_shader.setInt("cubemap", cubemap_tex.GetId());
		glm::mat4 cubemap_view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		set_coord_systems(cubemap_shader, projection, cubemap_view);
		cubemap_vao.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LESS);
		
		
		// MODEL REFLECTION__________________________________________________________________________________________________________________
		cubemap_reflection_shader.Activate();
		cubemap_reflection_shader.setVec3("camera_pos", camera_position);
		cubemap_reflection_shader.setInt("cubemap", cubemap_tex.GetId());
		draw_model(cubemap_reflection_shader, reflected_model, reflected_models_amount);
		
		// MODEL______________________________________________________________________________________________________________________________
		normal_map_shader.Activate();
		//object_shader.setInt("material.texture_specular1", black_specular_map.GetId());
		normal_map_shader.setInt("shadow_map", directional_shadow_fbo_tex.GetId());
		
		glStencilFunc(GL_ALWAYS, 1, 0xFF);	// stencil buffer will always fill with 1s
		glStencilMask(0xFF);				// writes data in the stencil buffer
		draw_model(normal_map_shader, loaded_model, models_amount);

		// MODEL NORMALS______________________________________________________________________________________________________________________
		normal_visualizing.Activate();
		draw_model(normal_visualizing, loaded_model, models_amount);
		
		// MODEL OUTLINE______________________________________________________________________________________________________________________
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);	// stencil buffer will fill with values which are not equal to 1 (the outlining)
		glStencilMask(0x00);					// doesn't write in the stencil buffer
		glDisable(GL_DEPTH_TEST);				// show object through other pbjects
		
		stencil_shader.Activate();
		stencil_shader.setFloat("scale", 0.01f);
		draw_model(stencil_shader, loaded_model, models_amount);
		
		glStencilFunc(GL_ALWAYS, 0, 0xFF);	// stencil buffer will always fill with 0s
		glStencilMask(0xFF);				// writes data in the stencil buffer(stencil buffer cannot be cleared while start's in read only mode)
		glEnable(GL_DEPTH_TEST);			// enables depth discard_shadow to proper display
		
		// TRANSPARENT OBJECTS________________________________________________________________________________________________________________
		glDisable(GL_CULL_FACE);
		




		// GRASS______________________________________________________________________________________________________________________________
		discard_shader.Activate();
		discard_shader.setInt("texture1", grass_tex.GetId());

		unsigned int grass_index = 0;
		for (std::map<float, glm::vec3>::reverse_iterator start = sorted.rbegin(); start != sorted.rend(); ++start, grass_index++) {	// from pointer to the end of the map to pointer to the start of the map
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, start->second);	// pointer to the model position in the map
			model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
			grass_pos_mat[grass_index] = model;
		}
		grass_pos_storage.Bind();
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(grass_pos_mat), &grass_pos_mat[0]);
		grass_pos_storage.Unbind();
		grass_transparent_vao.MatInstance(3, grass_pos_storage);
		grass_transparent_vao.Bind();
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, objects_amount);
		

		
		// WINDOWS____________________________________________________________________________________________________________________________
		no_material_shader.Activate();
		no_material_shader.setInt("texture1", window_tex.GetId());


		unsigned int windows_index = 0;
		for (std::map<float, glm::vec3>::reverse_iterator start = sorted.rbegin(); start != sorted.rend(); start++, windows_index++) {	// from pointer to the end of the map to pointer to the start of the map
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, start->second);	// pointer to the model position in the map
			model = glm::translate(model, glm::vec3(0.0f, 3.0f, 2.0f));
			windows_pos_mat[windows_index] = model;
		}
		windows_pos_storage.Bind();
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(windows_pos_mat), &windows_pos_mat[0]);
		windows_pos_storage.Unbind();
		windows_transparent_vao.MatInstance(3, windows_pos_storage);
		windows_transparent_vao.Bind();
		glDepthMask(GL_FALSE);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, objects_amount);
		glDepthMask(GL_TRUE);




		anti_aliasing_fbo.UnbindFramebuffer();





		Framebuffer* pingpongFbo[] = {&ping_fbo, &pong_fbo};
		Texture* pingpongFboTexture[] = {&ping_fbo_tex, &pong_fbo_tex};


		

		bool horizontal = true, first_iteration = true;
		int amount = 4;
		blur_shader.Activate();
		for (unsigned int i = 0; i < amount; i++) {
			pingpongFbo[horizontal]->BindFramebuffer();
			blur_shader.setBool("horizontal", horizontal);

			if (first_iteration){
				blur_shader.setInt("image", post_proc_fbo_tex2.GetId());
				first_iteration = false;
			}
			else
				blur_shader.setInt("image", pingpongFboTexture[!horizontal]->GetId());
		
			framebuffer_vao.Bind();
			glDisable(GL_DEPTH_TEST);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		
			horizontal = !horizontal;
				
		}
		
		
		pingpongFbo[horizontal]->UnbindFramebuffer();






		// FRAMEBUFFER______________________________________________________________________________________________________________________
		anti_aliasing_fbo.BindFramebufferR();
		post_proc_fbo.BindFramebufferD();
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		anti_aliasing_fbo.BlitFramebuffer(width, height);
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glDrawBuffer(GL_COLOR_ATTACHMENT1);
		anti_aliasing_fbo.BlitFramebuffer(width, height);
		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);
		anti_aliasing_fbo.UnbindFramebuffer();

		//post_proc_fbo.UnbindFramebuffer();

		//glClearColor(1.0f, 1.0f, 0.5f, 0.5f);
		//glClear(GL_COLOR_BUFFER_BIT);
		
		glDisable(GL_DEPTH_TEST);
		framebuffer_shader.Activate();
		framebuffer_shader.setInt("screen_texture", post_proc_fbo_tex1.GetId());
		framebuffer_shader.setInt("bloom_blur", pingpongFboTexture[!horizontal]->GetId());
		framebuffer_shader.setFloat("exposure", exposure);
		framebuffer_shader.setFloat("near_plane", 0.1f);
		framebuffer_shader.setFloat("far_plane", 100.0f);
		framebuffer_vao.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glEnable(GL_DEPTH_TEST);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}


void Window::render_shadow_map(Shader& default_shader, Shader& discard_shader, Shader& normal_map_shader, std::map<float, glm::vec3> sorted, glm::mat4 windows_pos_mat[], glm::mat4 grass_pos_mat[], unsigned int objects_amount, unsigned int normal_map_objects_amount, int reflected_models_amount, int models_amount) {
	
	// anti peter panning
	glCullFace(GL_FRONT);
	vao_model.Bind();
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, objects_amount);

	normal_map_shader.Activate();
	normal_map_vao.Bind();
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, normal_map_objects_amount);

	// REFLECTED MODELS____________________________________________________________________________________________________________________________
	draw_model(normal_map_shader, reflected_model, reflected_models_amount);
	// MODELS______________________________________________________________________________________________________________________________________
	draw_model(normal_map_shader, loaded_model, models_amount);
	
	// for 2D objects
	glDisable(GL_CULL_FACE);
	// GRASS______________________________________________________________________________________________________________________________________
	discard_shader.Activate();
	discard_shader.setInt("texture1", grass_tex.GetId());
	
	
	unsigned int grass_index = 0;
	for (std::map<float, glm::vec3>::reverse_iterator start = sorted.rbegin(); start != sorted.rend(); ++start, grass_index++) {	// from pointer to the end of the map to pointer to the start of the map
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, start->second);	// pointer to the model position in the map
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		grass_pos_mat[grass_index] = model;
	}
	grass_pos_storage.Bind();
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(grass_pos_mat), &grass_pos_mat[0]);
	grass_pos_storage.Unbind();
	grass_transparent_vao.MatInstance(3, grass_pos_storage);
	grass_transparent_vao.Bind();
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, objects_amount);
	

	// WINDOWS____________________________________________________________________________________________________________________________________
	default_shader.Activate();
	unsigned int windows_index = 0;
	for (std::map<float, glm::vec3>::reverse_iterator start = sorted.rbegin(); start != sorted.rend(); ++start, windows_index++) {	// from pointer to the end of the map to pointer to the start of the map
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, start->second);	// pointer to the model position in the map
		model = glm::translate(model, glm::vec3(0.0f, 3.0f, 2.0f));
		windows_pos_mat[windows_index] = model;
	}
	windows_pos_storage.Bind();
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(windows_pos_mat), &windows_pos_mat[0]);
	windows_pos_storage.Unbind();
	windows_transparent_vao.MatInstance(3, windows_pos_storage);
	windows_transparent_vao.Bind();
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, objects_amount);

	// anti peter panning
	glCullFace(GL_BACK);
}


std::vector<glm::mat4> Window::CalcPointShadowMatrices(glm::mat4 shadow_projection_mat, glm::vec3 light_pos) {
	std::vector<glm::mat4> shadow_transforms;
	// RIGHT
	shadow_transforms.push_back(shadow_projection_mat *
		glm::lookAt(light_pos, light_pos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	// LEFT
	shadow_transforms.push_back(shadow_projection_mat *
		glm::lookAt(light_pos, light_pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	// TOP
	shadow_transforms.push_back(shadow_projection_mat *
		glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	// BOTTOM
	shadow_transforms.push_back(shadow_projection_mat *
		glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	// NEAR
	shadow_transforms.push_back(shadow_projection_mat *
		glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	// FAR
	shadow_transforms.push_back(shadow_projection_mat *
		glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	return shadow_transforms;
}



void Window::dir_light_settings(glm::vec3 dir_light_color, Shader& shader, UniformBuffer& ubo) {
	// directional light power
	glm::vec3 dir_ambient_power = dir_light_color * glm::vec3(0.05f);
	glm::vec3 dir_diff_power = dir_light_color * glm::vec3(0.2f);
	glm::vec3 dir_spec_power = dir_light_color * glm::vec3(0.3f);
	// directional light settings
	glm::vec3 direction(0.0f, -0.1f, 0.0f);
	ubo.SubData(sizeof(glm::vec4), sizeof(glm::vec4), &direction);
	ubo.SubData(sizeof(glm::vec4) * 2, sizeof(glm::vec4), &dir_ambient_power);
	ubo.SubData(sizeof(glm::vec4) * 3, sizeof(glm::vec4), &dir_diff_power);
	ubo.SubData(sizeof(glm::vec4) * 4, sizeof(glm::vec4), &dir_spec_power);
	//shader.setVec3("dir_light.direction", 0.0f, -0.1f, 0.0f);
	//shader.setVec3("dir_light.ambient", dir_ambient_power);
	//shader.setVec3("dir_light.diffuse", dir_diff_power);
	//shader.setVec3("dir_light.specular", dir_spec_power);
}

void Window::point_light_settings(int point_light_size, glm::vec3* point_light_pos, glm::vec3* point_ambient_power, glm::vec3* point_diff_power, glm::vec3* point_spec_power, glm::vec3* point_light_colors) {
	for (int i = 0; i < point_light_size; i++) {
		// lighting maps power
		point_ambient_power[i] = point_light_colors[i] * glm::vec3(0.05f);
		point_diff_power[i] = point_light_colors[i] * glm::vec3(0.8f);
		point_spec_power[i] = point_light_colors[i] * glm::vec3(1.0f);

		// sets point ligh
		char buff[100]{};
		sprintf(buff, "point_lights[%d].%s", i, "position");
		object_shader.setVec3(buff, point_light_pos[i]);
		sprintf(buff, "point_lights[%d].%s", i, "constant");
		object_shader.setFloat(buff, 1.0f);
		sprintf(buff, "point_lights[%d].%s", i, "linear");
		object_shader.setFloat(buff, 0.09f);
		sprintf(buff, "point_lights[%d].%s", i, "quadratic");
		object_shader.setFloat(buff, 0.032f);
		sprintf(buff, "point_lights[%d].%s", i, "ambient");
		object_shader.setVec3(buff, point_ambient_power[i]);
		sprintf(buff, "point_lights[%d].%s", i, "diffuse");
		object_shader.setVec3(buff, point_diff_power[i]);
		sprintf(buff, "point_lights[%d].%s", i, "specular");
		object_shader.setVec3(buff, point_spec_power[i]);
	}
}

void Window::spot_light_settings(glm::vec3 spot_light_color, glm::vec3 cam_pos, glm::vec3 cam_front, Shader& shader, UniformBuffer& ubo) {
	// spot light power
	glm::vec3 spot_ambient_power = spot_light_color * glm::vec3(0.0f);
	glm::vec3 spot_diff_power = spot_light_color * glm::vec3(1.0f);
	glm::vec3 spot_spec_power = spot_light_color * glm::vec3(1.0f);
	// spot light settings
	ubo.SubData(sizeof(glm::vec4) * 17 + sizeof(glm::vec3) * 4 + sizeof(float) * 20, sizeof(glm::vec4), &cam_pos);
	ubo.SubData(sizeof(glm::vec4) * 18 + sizeof(glm::vec3) * 4 + sizeof(float) * 20, sizeof(glm::vec4), &cam_front);
	ubo.SubData(sizeof(glm::vec4) * 19 + sizeof(glm::vec3) * 4 + sizeof(float) * 20, sizeof(glm::vec4), &spot_ambient_power);
	ubo.SubData(sizeof(glm::vec4) * 20 + sizeof(glm::vec3) * 4 + sizeof(float) * 20, sizeof(glm::vec4), &spot_diff_power);
	ubo.SubData(sizeof(glm::vec4) * 21 + sizeof(glm::vec3) * 4 + sizeof(float) * 20, sizeof(glm::vec3), &spot_spec_power);
	float spot_light_const = 1.0f;
	ubo.SubData(sizeof(glm::vec4) * 21 + sizeof(glm::vec3) * 5 + sizeof(float) * 20, sizeof(float), &spot_light_const);
	float spot_light_linear = 0.35f;
	ubo.SubData(sizeof(glm::vec4) * 21 + sizeof(glm::vec3) * 5 + sizeof(float) * 21, sizeof(float), &spot_light_linear);
	float spot_light_quadratic = 0.55f;
	ubo.SubData(sizeof(glm::vec4) * 21 + sizeof(glm::vec3) * 5 + sizeof(float) * 22, sizeof(float), &spot_light_quadratic);

	//shader.setVec3("spot_light.position", cam_pos);
	//shader.setVec3("spot_light.direction", cam_front);
	//shader.setVec3("spot_light.ambient", spot_ambient_power);
	//shader.setVec3("spot_light.diffuse", spot_diff_power);
	//shader.setVec3("spot_light.specular", spot_spec_power);
	//shader.setFloat("spot_light.constant", 1.0f);
	//shader.setFloat("spot_light.linear", 0.35f);
	//shader.setFloat("spot_light.quadratic", 0.55f);
	if (flashlight) {
		float flashlight_cut_off = glm::cos(glm::radians(12.5f));
		float flashlight_outer_cut_off = glm::cos(glm::radians(20.0f));
		ubo.SubData(sizeof(glm::vec4) * 21 + sizeof(glm::vec3) * 5 + sizeof(float) * 23, sizeof(float), &flashlight_cut_off);
		ubo.SubData(sizeof(glm::vec4) * 21 + sizeof(glm::vec3) * 5 + sizeof(float) * 24, sizeof(float), &flashlight_outer_cut_off);
		//shader.setFloat("spot_light.cut_off", glm::cos(glm::radians(12.5f)));
		shader.setFloat("spot_light.outer_cut_off", glm::cos(glm::radians(20.0f)));
	}
	else {
		float flashlight_cut_off = glm::cos(glm::radians(0.0f));
		float flashlight_outer_cut_off = glm::cos(glm::radians(0.0f));
		ubo.SubData(sizeof(glm::vec4) * 21 + sizeof(glm::vec3) * 5 + sizeof(float) * 23, sizeof(float), &flashlight_cut_off);
		ubo.SubData(sizeof(glm::vec4) * 21 + sizeof(glm::vec3) * 5 + sizeof(float) * 24, sizeof(float), &flashlight_outer_cut_off);
		//shader.setFloat("spot_light.cut_off", glm::cos(glm::radians(0.0f)));
		//shader.setFloat("spot_light.outer_cut_off", glm::cos(glm::radians(0.0f)));
	}
}

void Window::draw_ojbect(int objects_amount, glm::vec3* model_positions) {
	glm::mat4 model = glm::mat4(1.0f);
	for (unsigned int i = 0; i < objects_amount; i++) {
		model = glm::translate(model, model_positions[i]);
		if (i % 3 == 0)
			model = glm::rotate(model, (float)(glm::radians(90.0f)), glm::vec3(1.0f, 1.0f, 1.0f));
		else
			model = glm::rotate(model, -(float)(i * 1.5), glm::vec3(0.2f, 1.0f, 0.5f));
		object_shader.Uniform_matrix("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}



void Window::draw_model(Shader& shader, glm::vec3 translate, glm::vec3 scale, float rotate_angle, glm::vec3 rotate, Model& loaded_model) {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, translate);
	model = glm::scale(model, scale);
	model = glm::rotate(model, glm::radians(rotate_angle), rotate);
	shader.setMat4("model", model);
	loaded_model.Draw(shader);
}


void Window::draw_model(Shader& shader, Model& loaded_model, unsigned int models_amount) {
	loaded_model.Draw(shader, models_amount);
}

void Window::draw_light_blocks(int point_light_size, glm::vec3* point_spec_power, glm::vec3* point_light_pos) {
	vao_light.Bind();
	for (unsigned int i = 0; i < point_light_size; i++) {
		lighting_shader.setVec3("color", point_spec_power[i]);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, point_light_pos[i]);
		std::cout << point_light_pos->x << '\n';
		model = glm::scale(model, glm::vec3(0.3f));
		lighting_shader.Uniform_matrix("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}

void Window::set_coord_systems(Shader& shader, glm::mat4 proj, glm::mat4 view) {
	shader.Uniform_matrix("projection", proj);
	shader.Uniform_matrix("view", view);
}

glm::mat4 Window::get_projection_perspective(float fov, float aspect, float near_plane, float far_plane) {
	return glm::perspective(glm::radians(fov), aspect, near_plane, far_plane);
}

glm::mat4 Window::get_projection_ortho(float near_plane, float far_plane) {
	return glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
}

void Window::bind_uniform_block(unsigned int shader_id[], const unsigned int arr_size, const char* uniform_block, unsigned int binding_point) {
	for (unsigned int i = 0; i < arr_size; i++) {
		unsigned int uniform_block_index = glGetUniformBlockIndex(shader_id[i], uniform_block);
		glUniformBlockBinding(shader_id[i], uniform_block_index, binding_point);
	}
}

Window::~Window() {

}