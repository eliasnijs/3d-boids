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
#include "cube.cpp"
#include "gui.cpp"


internal void
handle_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
	    || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
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

	Cube cube;
	cube_initialize(&cube);

	U32 VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube.vertices), cube.vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube.indices), cube.indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(F32), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(F32), (void*)(3 * sizeof(F32)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(F32), (void*)(6 * sizeof(F32)));
	glEnableVertexAttribArray(2);


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

	F64  frame_duration	= 1.0;
	while (!glfwWindowShouldClose(window)) {
		F64 start_time = glfwGetTime();

		handle_input(window);


		glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader_program);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex1);


		mat4x4 model_transform, view_transform, projection_transform;

		mat4x4_identity(model_transform);
		mat4x4_rotate(model_transform, model_transform, 1.0f, 0.0f, 0.0f, deg_to_rad(-55.0f));
		shader_set_mat4x4(shader_program, "model_transform", model_transform);

		mat4x4_identity(view_transform);
		mat4x4_translate(view_transform, 0.0f, 0.0f, -3.0f);
		shader_set_mat4x4(shader_program, "view_transform", view_transform);

		mat4x4_identity(projection_transform);
		mat4x4_perspective(projection_transform, deg_to_rad(45.0f),
				   (F32)window_width / (F32)window_height, 0.1f, 100.0f);
		shader_set_mat4x4(shader_program, "projection_transform", projection_transform);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		gui(frame_duration, &cube);

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

