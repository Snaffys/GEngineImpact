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
	shadow_shader = { "Shaders/vertex/framebuffer_shadow.vert", "Shaders/fragment/framebuffer_shadow.frag" };
	shadow_objects = { "Shaders/vertex/shadow_objects.vert", "Shaders/fragment/framebuffer_shadow.frag" };
	discard_shadow = { "Shaders/vertex/shadow_blending.vert", "Shaders/fragment/shadow_discard.frag" };





	pos_matrix_ubo = { 2 * sizeof(glm::mat4) };
	unsigned int shader_id[] = { object_shader.GetID(), instancing_shader.GetID(), lighting_shader.GetID(), stencil_shader.GetID(), discard_shader.GetID(), no_material_shader.GetID(), cubemap_reflection_shader.GetID() };
	bind_uniform_block(shader_id, sizeof(shader_id)/ sizeof(shader_id[0]), "matrices", pos_matrix_ubo.GetId());
	
	light_matrix_ubo = { sizeof(glm::mat4) };
	unsigned int shader_id_light[] = { object_shader.GetID(), discard_shadow.GetID(), shadow_objects.GetID(), shadow_shader.GetID()};
	bind_uniform_block(shader_id_light, sizeof(shader_id_light) / sizeof(shader_id_light[0]),"light_matrix", light_matrix_ubo.GetId());
	


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











	//tes1 = {
	//	// Coords
	//	{-0.5f,  0.5f, 0.2f},
	//	{ 0.5f,	 0.5f, 0.3f},
	//	{ 0.5f, -0.5f, 0.4f},
	//	{-0.5f, -0.5f, 0.9f}
	//};






	geom_vbo = {
		// Coords
		{-0.5f,  0.5f, 0.2f},
		{ 0.5f,	 0.5f, 0.3f},
		{ 0.5f, -0.5f, 0.4f},
		{-0.5f, -0.5f, 0.9f}
	};
	
	geom_vao = FramebufferVAO(geom_vbo);




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



	// Creating and binding framebuffer
	anti_aliasing_fbo = { width, height, samples, anti_aliasing_fbo_tex };
	post_proc_fbo = { width, height, post_proc_fbo_tex };

	shadow_fbo = { shadow_size, shadow_size, shadow_fbo_tex };

	cubemap_shadow_fbo = {cubemap_shadow_size, cubemap_shadow_size, cubemap_shadow_tex};
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


	window_state_map[window]->anti_aliasing_fbo_tex.BindMultisample();
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, window_state_map[window]->samples, GL_RGB, width, height, GL_TRUE);
	window_state_map[window]->anti_aliasing_fbo.BindRenderbuffer();
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, window_state_map[window]->samples, GL_DEPTH24_STENCIL8, width, height);


	window_state_map[window]->post_proc_fbo_tex.Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	window_state_map[window]->shadow_fbo_tex.Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, window_state_map[window]->shadow_size, window_state_map[window]->shadow_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

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
		glm::vec3(19.0f, -27.0f, -8.0f),
		glm::vec3(19.0f, -25.0f, 1.0f),
		glm::vec3(19.0f, -23.0f, 0.5f),
		glm::vec3(19.0f, -29.0f, -2.0f)
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
		glm::vec3(-5.5f, -6.2f, -15.7f),
		glm::vec3(0.2f,  0.7f, -4.8f),
		glm::vec3(-2.9f, -0.4f, -3.9f),
		glm::vec3(2.8f, -3.0f, -3.2f),
		glm::vec3(2.0f,  3.3f, -3.3f),
		glm::vec3(-0.8f,  3.9f, -6.6f),
		glm::vec3(0.0f,	0.0f, 3.0f),
		glm::vec3(-4.4f,  -5.8f, -6.4f),
		glm::vec3(-5.6f, -13.4f, -13.4f),
		glm::vec3(-2.0f,  -16.75f,  -1.0f)
	};

	for (unsigned int i = 0; i < objects_amount; i++) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubes_positions[i]);
		if (i == objects_amount - 1)
			model = glm::scale(model, glm::vec3(30.0f, 20.0f, 60.0f));

		grass_pos_mat[i] = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));

		if (i % 3 == 0)
			model = glm::rotate(model, (float)(glm::radians(0.0f)), glm::vec3(1.0f, 1.0f, 1.0f));
		else
			model = glm::rotate(model, -(float)(i * 1.5), glm::vec3(0.2f, 1.0f, 0.5f));

		cubes_pos_mat[i] = model;
	}

	cubes_pos_storage = { cubes_pos_mat, objects_amount };

	vao_model.MatInstance(3, cubes_pos_storage);

	grass_pos_storage = { grass_pos_mat, objects_amount };

	grass_transparent_vao.MatInstance(3, grass_pos_storage);

	int objects_pos_size = sizeof(cubes_positions) / sizeof(cubes_positions[0]);


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

	glm::vec3 dir_light_color = glm::vec3(0.1, 0.1, 0.15);

	glm::vec3 spot_light_color = glm::vec3(1.0, 1.0, 1.0);

	while (!glfwWindowShouldClose(window))
	{
		float time = glfwGetTime();


		// For proper movement for every PC
		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		processInput(window);

		glClearColor(0.9f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// sorts map by distance between camera and object positions
		std::map<float, glm::vec3> sorted;
		for (unsigned int i = 0; i < objects_pos_size; i++) {
			float distance = glm::length(camera.GetCameraPosition() - cubes_positions[i]);	// distance between camera and object position
			sorted[distance] = cubes_positions[i];	// map initialising
		}

		// SHADOW FRAMEBUFFER__________________________________________________________________________________________________________________
		glm::mat4 light_projection = get_projection_ortho(near_plane_shadow, far_plane_shadow);
		glm::mat4 light_view = glm::lookAt(glm::vec3(3.1f, 15.0f, 15.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 light_space_matrix = light_projection * light_view;
		light_matrix_ubo.SubData(0, sizeof(glm::mat4), &light_space_matrix);
		
		shadow_shader.Activate();
		
		glViewport(0, 0, shadow_size, shadow_size);
		shadow_fbo.BindFramebuffer();
		// anti peter panning
		glEnable(GL_CULL_FACE);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		// anti peter panning
		glCullFace(GL_FRONT);
		
		// CUBES______________________________________________________________________________________________________________________________________
		shadow_objects.Activate();
		vao_model.Bind();
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, objects_amount);
		
		// LIGHTS______________________________________________________________________________________________________________________________________
		//vao_light.Bind();
		//glDrawArraysInstanced(GL_TRIANGLES, 0, 36, light_amount);
		//vao_light.Unbind();
		
		// REFLECTED MODELS____________________________________________________________________________________________________________________________
		draw_model(shadow_objects, reflected_model, reflected_models_amount);
		
		// MODELS______________________________________________________________________________________________________________________________________
		draw_model(shadow_objects, loaded_model, models_amount);
		
		// for 2D objects
		glCullFace(GL_BACK);
		glDisable(GL_CULL_FACE);
		
		// GRASS______________________________________________________________________________________________________________________________________
		discard_shadow.Activate();
		discard_shadow.setInt("texture1", grass_tex.GetId());
		grass_transparent_vao.Bind();
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, objects_amount);
		
		// WINDOWS____________________________________________________________________________________________________________________________________
		shadow_objects.Activate();
		glm::mat4 windows_pos_mat[objects_amount];
		unsigned int windows_indexy = 0;
		for (std::map<float, glm::vec3>::reverse_iterator start = sorted.rbegin(); start != sorted.rend(); ++start, windows_indexy++) {	// from pointer to the end of the map to pointer to the start of the map
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, start->second);	// pointer to the model position in the map
			model = glm::translate(model, glm::vec3(0.0f, 3.0f, 2.0f));
			windows_pos_mat[windows_indexy] = model;
		}
		windows_pos_storage = { windows_pos_mat, objects_amount };
		windows_transparent_vao.MatInstance(3, windows_pos_storage);
		windows_transparent_vao.Bind();
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, objects_amount);
		windows_pos_storage.Delete();
		
		shadow_fbo.UnbindFramebuffer();



		//CUBEMAP SHADOW FRAMEBUFFER___________________________________________________________________________________________________________________
		//glViewport(0, 0, cubemap_shadow_size, cubemap_shadow_size);
		//cubemap_shadow_fbo.BindFramebuffer();
		//// anti peter panning
		//glEnable(GL_CULL_FACE);
		//glClear(GL_DEPTH_BUFFER_BIT);
		//
		//glm::mat4 shadow_proj = glm::perspective(90.0f, (float)cubemap_shadow_size / (float)cubemap_shadow_size, near_plane_cubamap_shadow, far_plane_cubemap_shadow);
		//
		//std::vector<glm::mat4> shadow_transfomrs;
		//// RIGHT
		//shadow_transfomrs.push_back(shadow_proj *
		//	glm::lookAt(point_light_pos[0], point_light_pos[0] + glm::vec3(1.0f,0.0f,0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		//// LEFT
		//shadow_transfomrs.push_back(shadow_proj *
		//	glm::lookAt(point_light_pos[0], point_light_pos[0] + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		//// TOP
		//shadow_transfomrs.push_back(shadow_proj *
		//	glm::lookAt(point_light_pos[0], point_light_pos[0] + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		//// BOTTOM
		//shadow_transfomrs.push_back(shadow_proj *
		//	glm::lookAt(point_light_pos[0], point_light_pos[0] + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		//// NEAR
		//shadow_transfomrs.push_back(shadow_proj *
		//	glm::lookAt(point_light_pos[0], point_light_pos[0] + glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		//// FAR
		//shadow_transfomrs.push_back(shadow_proj *
		//	glm::lookAt(point_light_pos[0], point_light_pos[0] + glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		//
		//
		//
		//
		//
		//
		//
		//glm::mat4 view = camera.GetViewMatrix();
		//glm::mat4 projection = get_projection_perspective(camera_fov, (float)(width) / (float)(height), near_plane_camera, far_plane_camera);
		//object_shader.Activate();
		//glm::vec3 camera_position = camera.GetCameraPosition();
		//glm::vec3 camera_front = camera.GetCameraFront();
		//object_shader.setVec3("view_pos", camera_position);
		//object_shader.setMat4("light_space_matrix", light_space_matrix);
		//object_shader.setInt("shadowMap", shadow_fbo_tex.GetId());
		//
		//// anti peter panning
		//glCullFace(GL_FRONT);
		//
		//// CUBES______________________________________________________________________________________________________________________________________
		//shadow_objects.Activate();
		//shadow_objects.setMat4("light_space_matrix", light_space_matrix);
		//vao_model.Bind();
		//glDrawArraysInstanced(GL_TRIANGLES, 0, 36, objects_amount);
		//
		//// LIGHTS______________________________________________________________________________________________________________________________________
		////vao_light.Bind();
		////glDrawArraysInstanced(GL_TRIANGLES, 0, 36, light_amount);
		////vao_light.Unbind();
		//
		//// REFLECTED MODELS____________________________________________________________________________________________________________________________
		//draw_model(shadow_objects, reflected_model, reflected_models_amount);
		//
		//// MODELS______________________________________________________________________________________________________________________________________
		//draw_model(shadow_objects, loaded_model, models_amount);
		//
		//// for 2D objects
		//glCullFace(GL_BACK);
		//glDisable(GL_CULL_FACE);
		//
		//// GRASS______________________________________________________________________________________________________________________________________
		//discard_shadow.Activate();
		//discard_shadow.setMat4("light_space_matrix", light_space_matrix);
		//discard_shadow.setInt("texture1", grass_tex.GetId());
		//grass_transparent_vao.Bind();
		//glDrawArraysInstanced(GL_TRIANGLES, 0, 6, objects_amount);
		//
		//// WINDOWS____________________________________________________________________________________________________________________________________
		//shadow_objects.Activate();
		//glm::mat4 windows_pos_mat[objects_amount];
		//unsigned int windows_indexy = 0;
		//for (std::map<float, glm::vec3>::reverse_iterator start = sorted.rbegin(); start != sorted.rend(); ++start, windows_indexy++) {	// from pointer to the end of the map to pointer to the start of the map
		//	glm::mat4 model = glm::mat4(1.0f);
		//	model = glm::translate(model, start->second);	// pointer to the model position in the map
		//	model = glm::translate(model, glm::vec3(0.0f, 3.0f, 2.0f));
		//	windows_pos_mat[windows_indexy] = model;
		//}
		//windows_pos_storage = { windows_pos_mat, objects_amount };
		//windows_transparent_vao.MatInstance(3, windows_pos_storage);
		//windows_transparent_vao.Bind();
		//glDrawArraysInstanced(GL_TRIANGLES, 0, 6, objects_amount);
		//windows_pos_storage.Delete();
		//
		//shadow_fbo.UnbindFramebuffer();






		// POST PROCESSING FRAMEBUFFER_________________________________________________________________________________________________________________
		glViewport(0, 0, width, height);
		anti_aliasing_fbo.BindFramebuffer();
		
		glEnable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		//glStencilMask(0x00);
		glEnable(GL_DEPTH_TEST);





		glm::mat4 projection = get_projection_perspective(camera_fov, (float)(width) / (float)(height), near_plane_camera, far_plane_camera);
		pos_matrix_ubo.SubData(0, sizeof(glm::mat4), &projection);

		glm::mat4 view = camera.GetViewMatrix();
		pos_matrix_ubo.SubData(sizeof(glm::mat4), sizeof(glm::mat4), &view);


		object_shader.Activate();
		glm::vec3 camera_position = camera.GetCameraPosition();
		glm::vec3 camera_front = camera.GetCameraFront();
		object_shader.setVec3("view_pos", camera_position);
		object_shader.setFloat("time", time);
		object_shader.setInt("shadowMap", shadow_fbo_tex.GetId());

		// LIGHTING__________________________________________________________________________________________________________________
		// point
		glm::vec3 point_ambient_power[point_light_size];
		glm::vec3 point_diffuse_power[point_light_size];
		glm::vec3 point_specular_power[point_light_size];
		
		object_shader.setFloat("material.shininess", 255.0f);
		
		dir_light_settings(dir_light_color, object_shader);
		
		// Point light
		for (int i = 0; i < point_light_size; i++) {
			// lighting maps power
			point_ambient_power[i] = point_light_colors[i] * glm::vec3(0.05f);
			point_diffuse_power[i] = point_light_colors[i] * glm::vec3(0.5f);
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
		//point_light_settings(point_light_size, point_light_pos, point_ambient_power, point_diffuse_power, point_specular_power, point_light_colors);
		
		spot_light_settings(spot_light_color, camera_position, camera_front, object_shader);
		
		// CUBES____________________________________________________________________________________________________________________________
		object_shader.setInt("material.texture_diffuse1", diffuse_map.GetId());
		object_shader.setInt("material.texture_specular1", specular_map.GetId());
		vao_model.Bind();
		glDrawArraysInstanced(GL_TRIANGLES, 0, 36, objects_amount);
		
		
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
		object_shader.Activate();
		object_shader.setFloat("time", time);
		object_shader.setInt("shadow_map", shadow_fbo_tex.GetId());
		
		glStencilFunc(GL_ALWAYS, 1, 0xFF);	// stencil buffer will always fill with 1s
		glStencilMask(0xFF);				// writes data in the stencil buffer
		draw_model(object_shader, loaded_model, models_amount);
		
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
		grass_transparent_vao.Bind();
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, objects_amount);
		
		// WINDOWS____________________________________________________________________________________________________________________________
		no_material_shader.Activate();
		no_material_shader.setInt("texture1", window_tex.GetId());
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
		framebuffer_shader.Activate();
		framebuffer_shader.setInt("screen_texture", post_proc_fbo_tex.GetId());
		framebuffer_shader.setFloat("near_plane", 0.1f);
		framebuffer_shader.setFloat("far_plane", 100.0f);
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
	glm::vec3 dir_diff_power = dir_light_color * glm::vec3(0.2f);
	glm::vec3 dir_spec_power = dir_light_color * glm::vec3(0.3f);
	// directional light settings
	shader.setVec3("dir_light.direction", 0.0f, -0.1f, 0.0f);
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
		object_shader.setVec3(buff, point_light_pos[i]);
		//std::cout << point_light_pos->x << '\n';
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