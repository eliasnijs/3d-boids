#include <stdio.h>
#include <stdlib.h>
#include <cstdint>

#include <glad.h>
#include <glad.c>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <linmath.h>
#include <stb_image.h>

#define ENABLE_PRINT  1
#define ENABLE_ASSERT 1
#include "utils/utils.h"

internal F32
rad(F32 deg)
{
  return deg*(PI32/180);
}

#include "engine/window.cpp"
#include "engine/imgui.cpp"
#include "engine/camera.cpp"
#include "engine/shaders.cpp"
#include "engine/models.cpp"
#include "engine/textures.cpp"

struct ButtonState {
	B32 is_down;
	B32 was_down;
};

struct MouseState {
	F64 xpos;
	F64 ypos;
	F64 scroll;
	F64 xpos_prev;
	F64 ypos_prev;
	F64 scroll_prev;
};

struct Controller {
	MouseState  mouse;

	ButtonState move_left;
	ButtonState move_right;
	ButtonState move_up;
	ButtonState move_down;
	ButtonState move_forward;
	ButtonState move_backward;

	ButtonState pause;
};

enum GameStateType {
	GAME_STATE_MENU,
	GAME_STATE_PLAY,
};

struct GameState {
	GLFWwindow	*window;

	GameStateType	type;
	F32		dt;

	Camera		camera;

	Model		*models
	ModelInstance	*instances;
	U32		n_instances;
	Shader		*shaders;
	U32		n_shaders;
	Texture		*textures;
	U32		n_textures;

};

internal B32
button_released(ButtonState *button)
{
	return !button->is_down && button->was_down;
}

internal B32
button_is_down(ButtonState *button) {
	return button->is_down;
}

internal B32
button_pressed(ButtonState *button)
{
	return button->is_down && !button->was_down;
}

internal B32
button_is_up(ButtonState *button)
{
	return !button->is_down;
}

#include "game.cpp"
#include "gui.cpp"

#include "models/default_cube.cpp"


global_variable Controller controller = {};
global_variable GameState  game_state = {};

internal void
update_button_state(ButtonState *button, B32 is_down)
{
	button->was_down = button->is_down;
	button->is_down  = is_down;
}

internal void
scroll_callback(GLFWwindow* window, F64 xoffset, F64 yoffset)
{
	controller.mouse.scroll_prev = controller.mouse.scroll;
	controller.mouse.scroll = yoffset;
}

internal void
get_input(GLFWwindow* window, GameState *game_state, Controller *controller)
{
	update_button_state(&controller->move_left,	glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
	update_button_state(&controller->move_right,	glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
	update_button_state(&controller->move_up,	glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
	update_button_state(&controller->move_down,	glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
	update_button_state(&controller->move_forward,	glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
	update_button_state(&controller->move_backward, glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
	update_button_state(&controller->pause,		glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS);

	F64 new_xpos, new_ypos;
	glfwGetCursorPos(window, &new_xpos, &new_ypos);
	controller->mouse.xpos_prev = controller->mouse.xpos;
	controller->mouse.ypos_prev = controller->mouse.ypos;
	controller->mouse.xpos = new_xpos;
	controller->mouse.ypos = new_ypos;
}

I32
main()
{
	/* initialize platform */
	I32 window_width = 1440;
	I32 window_height = 840;
	GLFWwindow *window = window_init(window_width, window_height, "Hello World!");
	if (!window) {
		print_error("failed to initialize window");
		return 1;
	}

	imgui_initialize(window);
	stbi_set_flip_vertically_on_load(true);
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, scroll_callback);
	game_state.window = window;

	/* initialize game */
	game_state.type	= GAME_STATE_PLAY;
	game_state.dt	= 0.0f;
	game_state.camera = {
		{(F32)window_width, (F32)window_height},
		{0.0f, 0.0f, 3.0f},
		{0.0f, 0.0f},
		1.0f, 45.0f, 0.1f
	};
	camera_start(&game_state.camera);

	Model *cube_model = &DEFAULT_CUBE_MODEL;
	load_model(cube_model);
	ModelInstance cubes[1];
	for (U32 i = 0; i < ArrayCount(cubes); ++i) {
		model_instance_initialize(&cubes[i], cube_model);
	}

	Texture tex1;
	if (!load_texture("./resources/texture.png", &tex1)) {
		print_error("failed to load texture");
		return 1;
	}

	U32 shader_program;
	if (!load_glprogram("example", &shader_program)) {
		print_error("failed to load triangle program");
		return 1;
	}
	glUseProgram(shader_program);
	shader_set_I32(shader_program, "tex1",		0);

	/* game loop */
	while (!glfwWindowShouldClose(window)) {
		F64 start_time = glfwGetTime();

		get_input(window, &game_state, &controller);

		glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		game_update(&game_state, &controller);

		glUseProgram(shader_program);
		shader_set_camera_transforms(shader_program, &game_state.camera);
		use_texture(&tex1, 0);
		for (U32 i = 0; i < ArrayCount(cubes); ++i) {
			draw_model_instance(&cubes[i], shader_program);
		}

		/* draw user interface */
		gui(&game_state, cubes, ArrayCount(cubes));

		/* update window */
		glfwSwapBuffers(window);
		glfwPollEvents();
		game_state.dt = glfwGetTime() - start_time;
	}

	/* cleanup before closing */
	texture_die(&tex1);
	model_die(cube_model);
	glprogram_die(shader_program);
	imgui_die();
	window_die(window);

	return 0;
}

