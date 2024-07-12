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

internal void
handle_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
	    || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}


internal inline void
imgui_log_frame_duration(F64 frame_duration)
{
	ImGui::SetNextWindowSize(ImVec2(500, 50));
	ImGui::Begin("Process Information", NULL, ImGuiWindowFlags_NoResize);
	ImGui::Text("Frame duration: %.3f ms | FPS: %.1f",
		    frame_duration * 1000.0f, 1.0 / frame_duration);
	ImGui::End();
}


I32
main()
{
	////////////////////////////////////////////////////////////////////////
	//// Initialize platform
	GLFWwindow *window = window_init(960, 540, "Hello, World!");
	if (!window) {
		print_error("failed to initialize window");
		return 1;
	}

	imgui_initialize(window);
	stbi_set_flip_vertically_on_load(true);


	////////////////////////////////////////////////////////////////////////
	//// Load models on the gpu

	F32 vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   0.4f, 0.4f, 0.4f,   1.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,   0.4f, 0.4f, 0.4f,   1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,   0.4f, 0.4f, 0.4f,   0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,   0.4f, 0.4f, 0.4f,   0.0f, 1.0f
	};

	U32 indices[] = {
		0, 1, 3,    // first triangle
		1, 2, 3     // second triangle
	};


	U32 VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	I32 tex1_w, tex1_h, tex1_c;
	U8 *tex1_data = stbi_load("./resources/textures/container.jpg", &tex1_w,
				  &tex1_h, &tex1_c, 0);
	if (tex1_data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex1_w, tex1_h, 0,
			     GL_RGB, GL_UNSIGNED_BYTE, tex1_data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		print_error("failed to load texture");
	}
	stbi_image_free(tex1_data);

	U32 tex2;
	glGenTextures(1, &tex2);
	glBindTexture(GL_TEXTURE_2D, tex2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	I32 tex2_w, tex2_h, tex2_c;
	U8 *tex2_data = stbi_load("./resources/textures/satelite.png", &tex2_w,
				  &tex2_h, &tex2_c, 0);
	if (tex2_data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex2_w, tex2_h, 0,
			     GL_RGB, GL_UNSIGNED_BYTE, tex2_data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		print_error("failed to load texture");
	}
	stbi_image_free(tex2_data);

	U32 shader_program;
	if (!load_glprogram("./src/example.vs", "./src/example.fs", &shader_program)) {
		print_error("failed to load triangle program");
		return 1;
	}

	glUseProgram(shader_program);
	shader_set_I32(shader_program, "tex1", 0);
	shader_set_I32(shader_program, "tex2", 1);


	////////////////////////////////////////////////////////////////////////
	//// Main loop

	F64 frame_duration = 1.0;
	while (!glfwWindowShouldClose(window)) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		F64 start_time = glfwGetTime();

		handle_input(window);


		glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex2);

		glUseProgram(shader_program);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		imgui_start_frame();
		imgui_log_frame_duration(frame_duration);
		imgui_end_frame();


		glfwSwapBuffers(window);
		glfwPollEvents();
		frame_duration = glfwGetTime() - start_time;
	}

	////////////////////////////////////////////////////////////////////////
	//// Cleanup
	glDeleteTextures(1, &tex1);
	glDeleteTextures(1, &tex2);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glprogram_die(shader_program);

	imgui_die();
	window_die(window);

	return 0;
}

