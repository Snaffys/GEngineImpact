#include"Window_class.h"

std::unordered_map<GLFWwindow*, Window*> window_state_map;
std::unordered_map<int, int> window_size_map;

Window::Window(int width, int height) {
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

	// Map for callbacks
	window_state_map[window] = this;
	window_size_map[width] = this->width;
	window_size_map[height] = this->height;

	// CALLBACKS
	// Sets objects in window stretching callback
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// Sets mouse callback
	glfwSetCursorPosCallback(window, mouse_callback);
	// Sets croll callback
	glfwSetScrollCallback(window, scroll_callback);

	// CAMERA
	// Makes cursor invisible
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Sets first_mouse to true to set cursor's position properly
	first_mouse = true;
	// Sets cursor on the screen center
	last_x = width / 2;
	last_y = height / 2;
	// Creates camera
	camera = Camera(glm::vec3(0.0f,0.0f,3.0f));
	// Initializes the field of view
	fov = 90.0f;

	// Enables z-buffer to output objects properly (initial value of glDepthFunc is GL_LESS, that means that ojbect which further from you replaces with object which nearer)
	glEnable(GL_DEPTH_TEST);

	// Enables stencil buffer
	//glEnable(GL_STENCIL_TEST);
	//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);	// Control how to pass/fail a test. Whenever the stencil value of a fragment is not equal to 1 the fragment passes the test and is drawn, otherwise discarded
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);	// Control result of passing/failing a test. If both stencil and depth tests pass, values update to ref in glStencilFunc(1 in this case)

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

	// SHADERS
	model_shader = { "Shaders\\vertex\\default.vert", "Shaders\\fragment\\default.frag" };
	lighting_shader = { "Shaders\\vertex\\default.vert", "Shaders\\fragment\\lighting.frag" };
	stencil_shader = { "Shaders\\vertex\\stencil_buffer.vert", "Shaders\\fragment\\stencil_buffer.frag" };
	no_material_shader = { "Shaders\\vertex\\default.vert", "Shaders\\fragment\\no_material.frag" };
	discard_shader = { "Shaders\\vertex\\default.vert", "Shaders\\fragment\\discard.frag" };
	framebuffer_shader = { "Shaders\\vertex\\simple_shader.vert", "Shaders\\fragment\\simple_shader.frag" };



	// TEXTURES
	// model's texture correct display
	stbi_set_flip_vertically_on_load(true);
	// model's texture settings
	diffuse_map = { "Textures\\steeled_wooden_container.png", GL_TEXTURE0, GL_CLAMP_TO_EDGE, GL_RGBA };
	specular_map = { "Textures\\steeled_border_2.png", GL_TEXTURE1, GL_CLAMP_TO_EDGE, GL_RGBA };
	grass = { "Textures\\grass.png", GL_TEXTURE2, GL_CLAMP_TO_EDGE, GL_RGBA };
	window_tex = { "Textures\\window_tex.png", GL_TEXTURE3, GL_CLAMP_TO_EDGE, GL_RGBA };
	

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

	transparent_vbo = {
		{ 0.0f, 0.5f, 0.0f,		0.0f, 0.0f, 0.0f,		0.0f, 1.0f},
		{ 0.0f, -0.5f, 0.0f,	0.0f, 0.0f, 0.0f,		0.0f, 0.0f},
		{ 1.0f, -0.5f, 0.0f,	0.0f, 0.0f, 0.0f,		1.0f, 0.0f},
		{ 0.0f, 0.5f, 0.0f,		0.0f, 0.0f, 0.0f,		0.0f, 1.0f},
		{ 1.0f, -0.5f, 0.0f,	0.0f, 0.0f, 0.0f,		1.0f, 0.0f},
		{ 1.0f, 0.5f, 0.0f,		0.0f, 0.0f, 0.0f,		1.0f, 1.0f}
	};

	vao_model = VAO(vbo);
	vao_light = VAO(vbo);
	transparent_vao = VAO(transparent_vbo);

	framebuffer_vbo = {
		// Coords		// texCoords
		{-1.0f,  1.0f,  0.0f, 1.0f},
		{ 1.0f, -1.0f,	1.0f, 0.0f},
		{-1.0f, -1.0f,  0.0f, 0.0f},
		{-1.0f,  1.0f,  0.0f, 1.0f},
		{ 1.0f,  1.0f,  1.0f, 1.0f},
		{ 1.0f, -1.0f,  1.0f, 0.0f}
	};

	framebuffer_vao.FramebufferVAO(framebuffer_vbo);



	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	
	
	fbo_tex = { GL_RGB, width, height, GL_TEXTURE4 };


	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	framebuffer_shader.Activate();
	framebuffer_shader.setInt("screen_texture", 4);



	// model's path
	loaded_model = { "objects\\mushroom\\model.fbx" };

	// flashlight usage
	flashlight = false;

	// unbinds buffers to avoid accidental modification
	vao_model.Unbind();
	vao_light.Unbind();
	transparent_vao.Unbind();
	framebuffer_vao.Unbind();
	//ebo.Unbind();	
	diffuse_map.Unbind();
	specular_map.Unbind();
	grass.Unbind();
	window_tex.Unbind();
	fbo_tex.Unbind();
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

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// Changes the viewport
	//std::cout << width << '\n' << height << std::endl;
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
	// Cube's positions
	glm::vec3 object_positions[] = {
		glm::vec3( 9.0f,  8.0f,  -17.0f),
		glm::vec3(-5.5f, -4.0f, -15.7f),
		glm::vec3( 0.2f,  0.7f, -4.8f),
		glm::vec3(-2.9f, -0.4f, -3.9f),
		glm::vec3( 3.0f, -3.0f, -3.2f),
		glm::vec3( 2.0f,  3.3f, -3.3f),
		glm::vec3(-0.8f,  3.9f, -6.6f),
		glm::vec3( 0.0f, -0.9f, -2.0f),
		glm::vec3(-4.4f,  5.6f, -6.4f),
		glm::vec3(-5.6f, -9.4f, -13.4f)
	};

	int model_pos_size = sizeof(object_positions) / sizeof(object_positions[0]);

	glm::vec3 point_light_pos[] = {	
		glm::vec3(1.0f, 2.0f, -1.0f),
		glm::vec3(-4.0f, -6.0f, 1.0f),
		glm::vec3(3.0f, 1.0f, 0.5f),
		glm::vec3(4.0f, -2.0f, -2.0f)
	};
	
	glm::vec3 point_light_colors[] = {
		glm::vec3(1.0, 0.0, 1.0),
		glm::vec3(1.0, 1.0, 0.0),
		glm::vec3(0.0, 1.0, 1.0),
		glm::vec3(1.0, 0.0, 0.0)
	};

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
		for (unsigned int i = 0; i < model_pos_size; i++) {
			float distance = glm::length(camera.GetCameraPosition() - object_positions[i]);	// distance between camera and object position
			sorted[distance] = object_positions[i];	// map initialising
		}


		//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

		//framebuffer_size_callback(window, width, height);

		//glViewport(0, 0, width, height);
		

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		// Window's color values
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_CULL_FACE);
		// Clears window color, depth buffer and stencil buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//glStencilMask(0x00); // disabling write in stencil buffer

		glEnable(GL_DEPTH_TEST);

		// Texture binding (all bindings should be before model drawing)
		diffuse_map.Bind();
		specular_map.Bind();
		grass.Bind();
		window_tex.Bind();

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
		model_shader.setFloat("material.shininess",	32.0f);

		// Directional light
		dir_light_settings(dir_light_color);

		// Point light
		for (int i = 0; i < point_light_size; i++) {
			// lighting maps power
			point_ambient_power[i] = point_light_colors[i] * glm::vec3(0.05f);
			point_diffuse_power[i] = point_light_colors[i] * glm::vec3(0.8f);
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
		spot_light_settings(spot_light_color, camera_position, camera_front);

		// Texture uniforms
		//model_shader.setInt("texture1", 0);
		//model_shader.setInt("texture2", 1);

		set_coord_systems(model_shader, projection, view);

		vao_model.Bind();

		// Draws 10 cubes
		for (unsigned int i = 0; i < model_pos_size; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, object_positions[i]);
			if (i % 3 == 0)
				model = glm::rotate(model, (float)(glm::radians(90.0f)), glm::vec3(1.0f, 1.0f, 1.0f));
			else
				model = glm::rotate(model, -(float)(i * 1.5), glm::vec3(0.2f, 1.0f, 0.5f));
			model_shader.Uniform_matrix("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		//draw_ojbect(model_pos_size, object_positions);



		//glStencilFunc(GL_ALWAYS, 1, 0xFF);	// stencil buffer will always fill with 1s
		//glStencilMask(0xFF);				// writes data in the stencil buffer



		// draws model
		draw_model(model_shader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.1f, 1.1f, 1.1f), -90.0f, glm::vec3(4.0f, 1.0f, 0.0f), loaded_model);



		//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);	// stencil buffer will fill with values which are not equal to 1 (the outlining)
		//glStencilMask(0x00);					// doesn't write in the stencil buffer
		//glDisable(GL_DEPTH_TEST);				// show object through other pbjects

		/*stencil_shader.Activate();
		stencil_shader.setFloat("scale", 0.01f);
		set_coord_systems(stencil_shader, projection, view);

		draw_model(stencil_shader, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.1f, 1.1f, 1.1f), -90.0f, glm::vec3(4.0f, 1.0f, 0.0f), loaded_model);*/
	



		//glStencilFunc(GL_ALWAYS, 0, 0xFF);	// stencil buffer will always fill with 0s
		//glStencilMask(0xFF);				// writes data in the stencil buffer(stencil buffer cannot be cleared while it's in read only mode)
		//glEnable(GL_DEPTH_TEST);			// enables depth test to proper display

		// lighting block
		lighting_shader.Activate();
		set_coord_systems(lighting_shader, projection, view);
		// Draws points of light
		vao_light.Bind();
		for (unsigned int i = 0; i < point_light_size; i++) {
			lighting_shader.setVec3("color", point_specular_power[i]);
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, point_light_pos[i]);
			model = glm::scale(model, glm::vec3(0.3f));
			lighting_shader.Uniform_matrix("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glDisable(GL_CULL_FACE);

		// grass
		discard_shader.Activate();
		discard_shader.setInt("texture1", 2);
		set_coord_systems(discard_shader, projection, view);
		transparent_vao.Bind();
		for (unsigned int i = 0; i < model_pos_size; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, object_positions[i]);
			model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
			discard_shader.Uniform_matrix("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		// windows
		no_material_shader.Activate();
		no_material_shader.setInt("texture1", 3);
		set_coord_systems(no_material_shader, projection, view);
		transparent_vao.Bind();
		for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {	// from pointer to the end of the map to pointer to the start of the map
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, it->second);	// pointer to the model position in the map
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));
			no_material_shader.Uniform_matrix("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		//draw_light_blocks(point_light_size, point_specular_power, point_light_pos);


		//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		//glViewport(0, 0, width, height);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		framebuffer_shader.Activate();
		
		framebuffer_vao.Bind();
		glDisable(GL_DEPTH_TEST);
		fbo_tex.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		
		//glEnable(GL_DEPTH_TEST);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}



void Window::dir_light_settings(glm::vec3 dir_light_color) {
	// directional light power
	glm::vec3 dir_ambient_power = dir_light_color * glm::vec3(0.05f);
	glm::vec3 dir_diff_power = dir_light_color * glm::vec3(0.1f);
	glm::vec3 dir_spec_power = dir_light_color * glm::vec3(0.2f);
	// directional light settings
	model_shader.setVec3("dir_light.direction", -0.35f, -1.0f, 0.0f);
	model_shader.setVec3("dir_light.ambient", dir_ambient_power);
	model_shader.setVec3("dir_light.diffuse", dir_diff_power);
	model_shader.setVec3("dir_light.specular", dir_spec_power);
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

void Window::spot_light_settings(glm::vec3 spot_light_color, glm::vec3 cam_pos, glm::vec3 cam_front) {
	// spot light power
	glm::vec3 spot_ambient_power = spot_light_color * glm::vec3(0.0f);
	glm::vec3 spot_diff_power = spot_light_color * glm::vec3(1.0f);
	glm::vec3 spot_spec_power = spot_light_color * glm::vec3(1.0f);
	// spot light settings
	model_shader.setVec3("spot_light.position", cam_pos);
	model_shader.setVec3("spot_light.direction", cam_front);
	model_shader.setVec3("spot_light.ambient", spot_ambient_power);
	model_shader.setVec3("spot_light.diffuse", spot_diff_power);
	model_shader.setVec3("spot_light.specular", spot_spec_power);
	model_shader.setFloat("spot_light.constant", 1.0f);
	model_shader.setFloat("spot_light.linear", 0.35f);
	model_shader.setFloat("spot_light.quadratic", 0.55f);
	if (flashlight) {
		model_shader.setFloat("spot_light.cut_off", glm::cos(glm::radians(12.5f)));
		model_shader.setFloat("spot_light.outer_cut_off", glm::cos(glm::radians(20.0f)));
	}
	else {
		model_shader.setFloat("spot_light.cut_off", glm::cos(glm::radians(0.0f)));
		model_shader.setFloat("spot_light.outer_cut_off", glm::cos(glm::radians(0.0f)));
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
