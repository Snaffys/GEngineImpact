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
	fov = 90.0f;

	// Enables z-buffer to output objects properly (initial value of glDepthFunc is GL_LESS, that means that ojbect which further from you replaces with object which nearer)
	glEnable(GL_DEPTH_TEST);

	// Enables stencil buffer
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);	// Control how to pass/fail a test. Whenever the stencil value of a fragment is not equal to 1 the fragment passes the test and is drawn, otherwise discarded
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);	// Control result of passing/failing a test. If both stencil and depth tests pass, values update to ref in glStencilFunc(1 in this case)

	// Enables blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enables face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Variables for proper movement
	delta_time = 0.0f;
	last_frame = 0.0f;

	// SHADERS_______________________________________________________________________________________________
	object_shader = { "Shaders/vertex/cubes_instancing.vert", "Shaders/fragment/default.frag" };
	model_shader = { "Shaders/vertex/default.vert", "Shaders/fragment/default.frag" };
	lighting_shader = { "Shaders/vertex/default.vert", "Shaders/fragment/lighting.frag" };
	stencil_shader = { "Shaders/vertex/stencil_buffer.vert", "Shaders/fragment/stencil_buffer.frag" };
	no_material_shader = { "Shaders/vertex/cubes_instancing.vert", "Shaders/fragment/no_material.frag" };
	discard_shader = { "Shaders/vertex/cubes_instancing.vert", "Shaders/fragment/discard.frag" };
	cubemap_shader = { "Shaders/vertex/cubemap_shader.vert", "Shaders/fragment/cubemap_shader.frag" };
	framebuffer_shader = { "Shaders/vertex/framebuffer_shader.vert", "Shaders/fragment/framebuffer_shader.frag" };
	cubemap_reflection_shader = { "Shaders/vertex/cubemap_reflection.vert", "Shaders/fragment/cubemap_reflection.frag" };
	instancing_shader = { "Shaders/vertex/Test.vert", "Shaders/fragment/Test.frag" };

	// TEXTURES______________________________________________________________________________________________
	// model's texture settings
	diffuse_map = { "Textures/steeled_wooden_container.png", GL_CLAMP_TO_EDGE, GL_RGBA };
	specular_map = { "Textures/steeled_border_2.png", GL_CLAMP_TO_EDGE, GL_RGBA };
	grass_tex = { "Textures/grass.png", GL_CLAMP_TO_EDGE, GL_RGBA, GL_SRGB_ALPHA};
	window_tex = { "Textures/window_tex.png", GL_CLAMP_TO_EDGE, GL_RGBA, GL_SRGB_ALPHA};
	char faces[][100] =
	{ "Textures/Cubemaps/right.jpg",
	"Textures/Cubemaps/left.jpg",
	"Textures/Cubemaps/top.jpg",
	"Textures/Cubemaps/bottom.jpg",
	"Textures/Cubemaps/front.jpg",
	"Textures/Cubemaps/back.jpg" };
	cubemap_tex = { (int)sizeof(faces) / sizeof(faces)[0], (int)sizeof(faces[0]) / sizeof(faces[0])[0], (char*)faces };


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
		{  0.5f, -0.5f,  0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f},	// top-right
		{ -0.5f, -0.5f,  0.5f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f},	// top-left
		{ -0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f},	// bottom-left
		{ -0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f},	// bottom-left
		{  0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f},	// bottom-right
		{  0.5f, -0.5f,  0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f}		// top-right
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

	unsigned const int samples = 6;

	// Creating and binding framebuffer
	anti_aliasing_fbo = { width, height, samples, anti_aliasing_fbo_tex };
	post_proc_fbo = { width, height, post_proc_fbo_tex };


	// model's path
	loaded_model = { "objects/mushroom/model.fbx" };


	// UNBINDING______________________________________________________________________________________________
	// Unbinds buffers to avoid accidental modification
	vao_model.Unbind();
	vao_light.Unbind();
	grass_transparent_vao.Unbind();
	windows_transparent_vao.Unbind();
	framebuffer_vao.Unbind();
	cubemap_vao.Unbind();
	diffuse_map.Unbind();
	specular_map.Unbind();
	grass_tex.Unbind();
	window_tex.Unbind();
	//anti_aliasing_fbo_tex.Unbind();
	//ebo.Unbind();	
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
	// Changes the fov
	window_state_map[window]->fov -= (float)y_scroll;
	// Restrictions for the fov
	if (window_state_map[window]->fov < 1.0f)
		window_state_map[window]->fov = 1.0f;
	if (window_state_map[window]->fov > 90.0f)
		window_state_map[window]->fov = 90.0f;
}

void Window::Go() {
	const unsigned int light_amount = 4;
	glm::mat4 light_pos_mat[light_amount];
	glm::vec3 light_col_vec[light_amount];

	glm::vec3 point_light_pos[light_amount] = {
		glm::vec3(5.0f, 16.0f, -8.0f),
		glm::vec3(-4.0f, -6.0f, 1.0f),
		glm::vec3(3.0f, 1.0f, 0.5f),
		glm::vec3(4.0f, -2.0f, -2.0f)
	};

	glm::vec3 point_light_colors[light_amount] = {
		glm::vec3(1.0, 0.0, 1.0),
		glm::vec3(1.0, 1.0, 0.0),
		glm::vec3(0.0, 1.0, 1.0),
		glm::vec3(1.0, 0.0, 0.0)
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

	// cube's positions
	glm::vec3 cubes_positions[objects_amount] = {
		glm::vec3(-5.5f, -4.0f, -15.7f),
		glm::vec3(0.2f,  0.7f, -4.8f),
		glm::vec3(-2.9f, -0.4f, -3.9f),
		glm::vec3(3.0f, -3.0f, -3.2f),
		glm::vec3(2.0f,  3.3f, -3.3f),
		glm::vec3(-0.8f,  3.9f, -6.6f),
		glm::vec3(0.0f, -0.9f, -2.0f),
		glm::vec3(-4.4f,  5.6f, -6.4f),
		glm::vec3(-5.6f, -9.4f, -13.4f),
		glm::vec3(9.0f,  8.0f,  -17.0f)
	};

	for (unsigned int i = 0; i < objects_amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubes_positions[i]);
		if (i == objects_amount - 1)
			model = glm::scale(model, glm::vec3(20.0f, 20.0f, 20.0f));

		grass_pos_mat[i] = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));

		if (i % 3 == 0)
			model = glm::rotate(model, (float)(glm::radians(90.0f)), glm::vec3(1.0f, 1.0f, 1.0f));
		else
			model = glm::rotate(model, -(float)(i * 1.5), glm::vec3(0.2f, 1.0f, 0.5f));

		cubes_pos_mat[i] = model;
	}

	cubes_pos_storage = { cubes_pos_mat, objects_amount};

	vao_model.MatInstance(3, cubes_pos_storage);

	grass_pos_storage = {grass_pos_mat, objects_amount};

	grass_transparent_vao.MatInstance(3, grass_pos_storage);

	int objects_pos_size = sizeof(cubes_positions) / sizeof(cubes_positions[0]);


	const unsigned int models_amount = 2;
	glm::mat4 models_pos_mat[models_amount];
	glm::vec3 models_positions[models_amount] = {
		glm::vec3(0.0f, 3.0f, -2.0f),
		glm::vec3(0.0f, 0.0f, -1.0f)
	};
	for (unsigned int i = 0; i < models_amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, models_positions[i]);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = glm::rotate(model, glm::radians(30.0f), glm::vec3(2.0f,2.0f,2.0f));
		models_pos_mat[i] = model;
	}

	//models_pos_storage = {models_pos_mat, models_amount};


	reflected_model = { "objects/mushroom/model.fbx" , models_pos_mat, models_amount };





	const int point_light_size = sizeof(point_light_pos) / sizeof(point_light_pos[0]);

	glm::vec3 dir_light_color = glm::vec3(0.1, 0.1, 0.15);

	glm::vec3 spot_light_color = glm::vec3(1.0, 1.0, 1.0);





	while (!glfwWindowShouldClose(window))
	{

		// For proper movement for every PC
		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		processInput(window);

		// sorts map by distance between camera and object positions
		std::map<float, glm::vec3> sorted;
		for (unsigned int i = 0; i < objects_pos_size; i++) {
			float distance = glm::length(camera.GetCameraPosition() - cubes_positions[i]);	// distance between camera and object position
			sorted[distance] = cubes_positions[i];	// map initialising
		}









		// Window's color values
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Clears window color, depth buffer and stencil buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		anti_aliasing_fbo.BindFramebuffer();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_CULL_FACE);
		// Clears window color, depth buffer and stencil buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Disables writing in stencil buffer
		glStencilMask(0x00);
		glEnable(GL_DEPTH_TEST);

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = get_projection();

		// Shader activation
		model_shader.Activate();

		glm::vec3 camera_position = camera.GetCameraPosition();
		glm::vec3 camera_front = camera.GetCameraFront();
		model_shader.setVec3("view_pos", camera_position);

		// lighting maps power
		// point
		glm::vec3 point_ambient_power[point_light_size];
		glm::vec3 point_diffuse_power[point_light_size];
		glm::vec3 point_specular_power[point_light_size];

		// Material
		// color
		//model_shader.setVec3("material.diffuse",  1.0f, 1.0f, 1.0f);
		//model_shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
		// texture
		model_shader.setInt("material.texture_diffuse1", 0);
		model_shader.setInt("material.texture_specular1", 1);
		// shininess
		model_shader.setFloat("material.shininess", 32.0f);

		// Directional light
		dir_light_settings(dir_light_color, model_shader);

		// Point light
		for (int i = 0; i < point_light_size; i++) {
			// lighting maps power
			point_ambient_power[i] = point_light_colors[i] * glm::vec3(0.02f);
			point_diffuse_power[i] = point_light_colors[i] * glm::vec3(0.5f);
			point_specular_power[i] = point_light_colors[i] * glm::vec3(1.0f);

			// sets point ligh
			char buff[100]{};
			sprintf(buff, "point_lights[%d].%s", i, "position");
			model_shader.setVec3(buff, point_light_pos[i]);
			sprintf(buff, "point_lights[%d].%s", i, "constant");
			model_shader.setFloat(buff, 1.0f);
			sprintf(buff, "point_lights[%d].%s", i, "linear");
			model_shader.setFloat(buff, 0.09f);
			sprintf(buff, "point_lights[%d].%s", i, "quadratic");
			model_shader.setFloat(buff, 0.032f);
			sprintf(buff, "point_lights[%d].%s", i, "ambient");
			model_shader.setVec3(buff, point_ambient_power[i]);
			sprintf(buff, "point_lights[%d].%s", i, "diffuse");
			model_shader.setVec3(buff, point_diffuse_power[i]);
			sprintf(buff, "point_lights[%d].%s", i, "specular");
			model_shader.setVec3(buff, point_specular_power[i]);
		}
		//point_light_settings(point_light_size, point_light_pos, point_ambient_power, point_diffuse_power, point_specular_power, point_light_colors);

		// Spot light
		spot_light_settings(spot_light_color, camera_position, camera_front, model_shader);

		// Texture uniforms
		//model_shader.setInt("texture1", 0);
		//model_shader.setInt("texture2", 1);

		// CUBES____________________________________________________________________________________________________________________________
		// Shader activation
		object_shader.Activate();

		camera_position = camera.GetCameraPosition();
		object_shader.setVec3("view_pos", camera_position);

		// Material
		// texture
		object_shader.setInt("material.texture_diffuse1", diffuse_map.GetId());
		object_shader.setInt("material.texture_specular1", specular_map.GetId());
		// shininess
		object_shader.setFloat("material.shininess", 255.0f);

		// Directional light
		dir_light_settings(dir_light_color, object_shader);

		// Point light
		for (int i = 0; i < point_light_size; i++) {
			// lighting maps power
			point_ambient_power[i] = point_light_colors[i] * glm::vec3(0.05f);
			point_diffuse_power[i] = point_light_colors[i] * glm::vec3(0.8f);
			point_specular_power[i] = point_light_colors[i] * glm::vec3(1.0f);

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
			object_shader.setVec3(buff, point_diffuse_power[i]);
			sprintf(buff, "point_lights[%d].%s", i, "specular");
			object_shader.setVec3(buff, point_specular_power[i]);
		}

		// Spot light
		spot_light_settings(spot_light_color, camera_position, camera_front, object_shader);

		set_coord_systems(object_shader, projection, view);
		vao_model.Bind();
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, objects_amount);


		// LIGHTING BLOCKS____________________________________________________________________________________________________________________
		instancing_shader.Activate();
		set_coord_systems(instancing_shader, projection, view);
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
		//reflected_model.Draw(cubemap_shader);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glDepthFunc(GL_LESS);


		// MODEL REFLECTION__________________________________________________________________________________________________________________
		cubemap_reflection_shader.Activate();
		cubemap_reflection_shader.setVec3("camera_pos", camera_position);
		set_coord_systems(cubemap_reflection_shader, projection, view);
		cubemap_reflection_shader.setInt("cubemap", cubemap_tex.GetId());
		// draws model
		draw_model(cubemap_reflection_shader, reflected_model, models_amount);


		// MODEL______________________________________________________________________________________________________________________________
		glStencilFunc(GL_ALWAYS, 1, 0xFF);	// stencil buffer will always fill with 1s
		glStencilMask(0xFF);				// writes data in the stencil buffer
		
		model_shader.Activate();
		set_coord_systems(model_shader, projection, view);
		// draws model
		draw_model(model_shader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.1f, 1.1f, 1.1f), 90.0f, glm::vec3(4.0f, 1.0f, 0.0f), loaded_model);
		
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);	// stencil buffer will fill with values which are not equal to 1 (the outlining)
		glStencilMask(0x00);					// doesn't write in the stencil buffer
		glDisable(GL_DEPTH_TEST);				// show object through other pbjects
		
		stencil_shader.Activate();
		stencil_shader.setFloat("scale", 0.01f);
		set_coord_systems(stencil_shader, projection, view);
		draw_model(stencil_shader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.1f, 1.1f, 1.1f), 90.0f, glm::vec3(4.0f, 1.0f, 0.0f), loaded_model);
		
		glStencilFunc(GL_ALWAYS, 0, 0xFF);	// stencil buffer will always fill with 0s
		glStencilMask(0xFF);				// writes data in the stencil buffer(stencil buffer cannot be cleared while start's in read only mode)
		glEnable(GL_DEPTH_TEST);			// enables depth test to proper display


		// TRANSPARENT OBJECTS________________________________________________________________________________________________________________
		glDisable(GL_CULL_FACE);
		
		// grass_tex
		discard_shader.Activate();
		discard_shader.setInt("texture1", grass_tex.GetId());
		set_coord_systems(discard_shader, projection, view);
		grass_transparent_vao.Bind();
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, objects_amount);


		// windows
		no_material_shader.Activate();
		no_material_shader.setInt("texture1", window_tex.GetId());
		set_coord_systems(no_material_shader, projection, view);
		glm::mat4 windows_pos_mat[objects_amount];
		unsigned int windows_index = 0;
		for (std::map<float, glm::vec3>::reverse_iterator start = sorted.rbegin(); start != sorted.rend(); ++start, windows_index++) {	// from pointer to the end of the map to pointer to the start of the map
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, start->second);	// pointer to the model position in the map
			model = glm::translate(model, glm::vec3(0.0f, 3.0f, 2.0f));
			windows_pos_mat[windows_index] = model;
		}
		windows_pos_storage = { windows_pos_mat, objects_amount };
		windows_transparent_vao.MatInstance(3, windows_pos_storage);
		windows_transparent_vao.Bind();
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, objects_amount);
		windows_pos_storage.Delete();



		// FRAMEBUFFER______________________________________________________________________________________________________________________
		anti_aliasing_fbo.BindFramebufferR();
		post_proc_fbo.BindFramebufferD();
		anti_aliasing_fbo.BlitFramebuffer(width, height);

		anti_aliasing_fbo.UnbindFramebuffer();
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);
		framebuffer_shader.Activate();
		framebuffer_shader.setInt("screen_texture", post_proc_fbo_tex.GetId());
		framebuffer_vao.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glEnable(GL_DEPTH_TEST);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}



void Window::dir_light_settings(glm::vec3 dir_light_color, Shader& shader) {
	// directional light power
	glm::vec3 dir_ambient_power = dir_light_color * glm::vec3(0.05f);
	glm::vec3 dir_diff_power = dir_light_color * glm::vec3(0.1f);
	glm::vec3 dir_spec_power = dir_light_color * glm::vec3(0.2f);
	// directional light settings
	shader.setVec3("dir_light.direction", -0.35f, -1.0f, 0.0f);
	shader.setVec3("dir_light.ambient", dir_ambient_power);
	shader.setVec3("dir_light.diffuse", dir_diff_power);
	shader.setVec3("dir_light.specular", dir_spec_power);
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
		model_shader.setVec3(buff, point_light_pos[i]);
		//std::cout << point_light_pos->x << '\n';
		sprintf(buff, "point_lights[%d].%s", i, "constant");
		model_shader.setFloat(buff, 1.0f);
		sprintf(buff, "point_lights[%d].%s", i, "linear");
		model_shader.setFloat(buff, 0.09f);
		sprintf(buff, "point_lights[%d].%s", i, "quadratic");
		model_shader.setFloat(buff, 0.032f);
		sprintf(buff, "point_lights[%d].%s", i, "ambient");
		model_shader.setVec3(buff, point_ambient_power[i]);
		sprintf(buff, "point_lights[%d].%s", i, "diffuse");
		model_shader.setVec3(buff, point_diff_power[i]);
		sprintf(buff, "point_lights[%d].%s", i, "specular");
		model_shader.setVec3(buff, point_spec_power[i]);
	}
}

void Window::spot_light_settings(glm::vec3 spot_light_color, glm::vec3 cam_pos, glm::vec3 cam_front, Shader& shader) {
	// spot light power
	glm::vec3 spot_ambient_power = spot_light_color * glm::vec3(0.0f);
	glm::vec3 spot_diff_power = spot_light_color * glm::vec3(1.0f);
	glm::vec3 spot_spec_power = spot_light_color * glm::vec3(1.0f);
	// spot light settings
	shader.setVec3("spot_light.position", cam_pos);
	shader.setVec3("spot_light.direction", cam_front);
	shader.setVec3("spot_light.ambient", spot_ambient_power);
	shader.setVec3("spot_light.diffuse", spot_diff_power);
	shader.setVec3("spot_light.specular", spot_spec_power);
	shader.setFloat("spot_light.constant", 1.0f);
	shader.setFloat("spot_light.linear", 0.35f);
	shader.setFloat("spot_light.quadratic", 0.55f);
	if (flashlight) {
		shader.setFloat("spot_light.cut_off", glm::cos(glm::radians(12.5f)));
		shader.setFloat("spot_light.outer_cut_off", glm::cos(glm::radians(20.0f)));
	}
	else {
		shader.setFloat("spot_light.cut_off", glm::cos(glm::radians(0.0f)));
		shader.setFloat("spot_light.outer_cut_off", glm::cos(glm::radians(0.0f)));
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
		model_shader.Uniform_matrix("model", model);
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


void Window::draw_model(Shader& shader,  Model& loaded_model, unsigned int models_amount) {
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

glm::mat4 Window::get_projection() {
	return glm::perspective(glm::radians(fov), (float)(width) / (float)(height), 0.1f, 100.0f);
}

Window::~Window() {

}
