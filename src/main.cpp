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

#include "engine/window.cpp"
#include "engine/imgui.cpp"
#include "engine/shaders.cpp"
#include "engine/camera.cpp"
#include "geometry_models.cpp"

struct GameState {
	Camera	camera;
};

#include "gui.cpp"


internal void
handle_input(GLFWwindow* window, GameState *game_state)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
	    || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	F32 camera_speed = 0.05f;
	Camera *camera = &game_state->camera;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		vec3 scaled_front;
		vec3_scale(scaled_front, camera->front, camera_speed);
		vec3_add(camera->pos, scaled_front, camera->pos);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		vec3 scaled_front;
		vec3_scale(scaled_front, camera->front, camera_speed);
		vec3_sub(camera->pos, camera->pos, scaled_front);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		vec3 scaled_right;
		vec3_mul_cross(scaled_right, camera->front, camera->up);
		vec3_norm(scaled_right, scaled_right);
		vec3_scale(scaled_right, scaled_right, camera_speed);
		vec3_sub(camera->pos, camera->pos, scaled_right);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		vec3 scaled_right;
		vec3_mul_cross(scaled_right, camera->front, camera->up);
		vec3_norm(scaled_right, scaled_right);
		vec3_scale(scaled_right, scaled_right, camera_speed);
		vec3_add(camera->pos, scaled_right, camera->pos);
	}
}

internal F32
deg_to_rad(F32 deg)
{
  return deg*(PI32/180);
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


	GameState game_state = {};
	vec3 camera_pos      = {0.0f, 0.0f, 3.0f};
	vec3 camera_front    = {0.0f, 0.0f, -1.0f};
	vec3 camera_world_up = {0.0f, 1.0f, 0.0f};
	memcpy(game_state.camera.pos, camera_pos, sizeof(vec3));
	memcpy(game_state.camera.front, camera_front, sizeof(vec3));
	memcpy(game_state.camera.up, camera_world_up, sizeof(vec3));


	U32 VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, DEFAULT_CUBE->n_vertices * sizeof(F32), DEFAULT_CUBE->vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(F32), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(F32), (void*)(3 * sizeof(F32)));
	glEnableVertexAttribArray(1);

	ModelInstance cubes[1];
	for (U32 i = 0; i < ArrayCount(cubes); ++i) {
		model_instance_initialize(&cubes[i], DEFAULT_CUBE);
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
	if (!load_glprogram("./src/example.vs", "./src/example.fs", &shader_program)) {
		print_error("failed to load triangle program");
		return 1;
	}

	glUseProgram(shader_program);
	shader_set_I32(shader_program, "tex1", 0);
	mat4x4 projection_transform = {};
	mat4x4_perspective(projection_transform, deg_to_rad(45.0f), (F32)window_width / (F32)window_height, 0.1f, 100.0f);
	shader_set_mat4x4(shader_program, "projection_transform", projection_transform);

	F64  frame_duration	= 1.0;
	while (!glfwWindowShouldClose(window)) {
		F64 start_time = glfwGetTime();

		handle_input(window, &game_state);


		glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_program);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex1);

		mat4x4 view_transform;
		camera_get_view_transform(&game_state.camera, view_transform);
		shader_set_mat4x4(shader_program, "view_transform", view_transform);

		glBindVertexArray(VAO);
		for (U32 i = 0; i < ArrayCount(cubes); ++i) {
			mat4x4 model_transform;
			mat4x4_identity(model_transform);
			mat4x4_translate_in_place(model_transform, cubes[i].position[0], cubes[i].position[1], cubes[i].position[2]);
			mat4x4_rotate_X(model_transform, model_transform, deg_to_rad(cubes[i].rotation[0]));
			mat4x4_rotate_Y(model_transform, model_transform, deg_to_rad(cubes[i].rotation[1]));
			mat4x4_rotate_Z(model_transform, model_transform, deg_to_rad(cubes[i].rotation[2]));
			mat4x4_scale_aniso(model_transform, model_transform, cubes[i].scale[0], cubes[i].scale[1], cubes[i].scale[2]);
			shader_set_mat4x4(shader_program, "model_transform", model_transform);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		gui(frame_duration, &game_state, cubes, ArrayCount(cubes));

		glfwSwapBuffers(window);
		glfwPollEvents();
		frame_duration = glfwGetTime() - start_time;
	}

	glDeleteTextures(1, &tex1);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glprogram_die(shader_program);

	imgui_die();
	window_die(window);

	return 0;
}

