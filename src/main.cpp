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
#include "engine/shaders.cpp"
#include "engine/camera.cpp"
#include "engine/models.cpp"

struct Controller {
	F64 mouse_xpos;
	F64 mouse_ypos;
};

enum GameStateType {
	GAME_STATE_MENU,
	GAME_STATE_PLAY,
};

struct GameState {
	GameStateType	type;
	F32		dt;
	Camera		camera;
};

#include "gui.cpp"

#include "models/default_cube.cpp"


global_variable Controller controller = {};
global_variable GameState game_state = {};

void scroll_callback(GLFWwindow* window, F64 xoffset, F64 yoffset)
{
	Camera *camera = &game_state.camera;
	camera->zoom = Clamp(CAMERA_ZOOM_CLAMP_LB, camera->zoom - yoffset,
			     CAMERA_ZOOM_CLAMP_UB);
}

internal void
handle_input(GLFWwindow* window, GameState *game_state, Controller *controller)
{

	/* keyboard input */
	Camera *camera = &game_state->camera;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera_move(camera, AXIS_Z, AXIS_DIR_INC, game_state->dt);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera_move(camera, AXIS_Z, AXIS_DIR_DEC, game_state->dt);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera_move(camera, AXIS_X, AXIS_DIR_DEC, game_state->dt);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera_move(camera, AXIS_X, AXIS_DIR_INC, game_state->dt);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera_move(camera, AXIS_Y, AXIS_DIR_INC, game_state->dt);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera_move(camera, AXIS_Y, AXIS_DIR_DEC, game_state->dt);
	}

	/* mouse input */
	F64 new_xpos, new_ypos;
	glfwGetCursorPos(window, &new_xpos, &new_ypos);

	camera_aim(camera, new_xpos - controller->mouse_xpos,
		   controller->mouse_ypos - new_ypos);

	controller->mouse_xpos = new_xpos;
	controller->mouse_ypos = new_ypos;
}

I32
main()
{
	I32 window_width = 1440;
	I32 window_height = 840;
	GLFWwindow *window = window_init(window_width, window_height,
					 "Hello World!");
	if (!window) {
		print_error("failed to initialize window");
		return 1;
	}

	imgui_initialize(window);
	stbi_set_flip_vertically_on_load(true);
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, scroll_callback);



	game_state.type	= GAME_STATE_PLAY;
	game_state.dt	= 0.0f;

	game_state.camera.speed		=   1.0f;
	game_state.camera.fov		=  45.0f;
	game_state.camera.sensitivity	=   0.1f;
	game_state.camera.yaw		=  90.0f;
	game_state.camera.pitch		= 180.0f;
	vec3 camera_pos = {0.0f, 0.0f,  3.0f};
	vec3 camera_z   = {0.0f, 0.0f, -1.0f};
	vec3 camera_y   = {0.0f, 1.0f,  0.0f};
	vec3 camera_x   = {1.0f, 0.0f,  0.0f};
	vec3 world_y    = {0.0f, 1.0f,  0.0f};
	memcpy(game_state.camera.pos, camera_pos, sizeof(vec3));
	memcpy(game_state.camera.z_axis, camera_z, sizeof(vec3));
	memcpy(game_state.camera.y_axis, camera_y, sizeof(vec3));
	memcpy(game_state.camera.x_axis, camera_x, sizeof(vec3));
	memcpy(game_state.camera.world_y, world_y, sizeof(vec3));

	Model *model = &DEFAULT_CUBE_MODEL;
	load_model(model);
	ModelInstance cubes[1];
	for (U32 i = 0; i < ArrayCount(cubes); ++i) {
		model_instance_initialize(&cubes[i], model);
	}

	U32 tex1;
	glGenTextures(1, &tex1);
	glBindTexture(GL_TEXTURE_2D, tex1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	I32 tex1_w, tex1_h, tex1_c;
	U8 *tex1_data = stbi_load("./resources/texture.png", &tex1_w,
				  &tex1_h, &tex1_c, 0);
	if (tex1_data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex1_w, tex1_h, 0,
			     GL_RGBA, GL_UNSIGNED_BYTE, tex1_data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		print_error("failed to load texture");
	}
	stbi_image_free(tex1_data);

	U32 shader_program;
	if (!load_glprogram("./src/shaders/example.vs", "./src/shaders/example.fs", &shader_program)) {
		print_error("failed to load triangle program");
		return 1;
	}

	glUseProgram(shader_program);
	shader_set_I32(shader_program, "tex1", 0);

	while (!glfwWindowShouldClose(window)) {
		F64 start_time = glfwGetTime();

		handle_input(window, &game_state, &controller);

		glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_program);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex1);

		mat4x4 projection_transform = {};
		mat4x4_perspective(projection_transform,
				   rad(game_state.camera.fov),
				   (F32)window_width / (F32)window_height,
				   0.1f, 100.0f);
		shader_set_mat4x4(shader_program, "projection_transform", projection_transform);

		mat4x4 view_transform;
		camera_get_view_transform(&game_state.camera, view_transform);
		shader_set_mat4x4(shader_program, "view_transform", view_transform);

		glBindVertexArray(model->VAO);
		for (U32 i = 0; i < ArrayCount(cubes); ++i) {
			mat4x4 model_transform;
			model_get_transform(&cubes[i], model_transform);
			shader_set_mat4x4(shader_program, "model_transform", model_transform);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		gui(&game_state, cubes, ArrayCount(cubes));

		glfwSwapBuffers(window);
		glfwPollEvents();
		game_state.dt = glfwGetTime() - start_time;
	}

	glDeleteTextures(1, &tex1);

	glDeleteVertexArrays(1, &model->VAO);
	glDeleteBuffers(1, &model->VBO);
	glprogram_die(shader_program);

	imgui_die();
	window_die(window);

	return 0;
}

