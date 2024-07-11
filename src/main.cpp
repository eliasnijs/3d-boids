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


	////////////////////////////////////////////////////////////////////////
	//// Build triangle on the gpu
	F32 vertices[] = {
		// positions		// colors
		 0.5f,  0.5f, 0.0f,	0.0f, 1.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,	1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,	0.0f, 0.0f, 1.0f, 1.0f,

		 0.5f, -0.5f, 0.0f,	1.0f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f,	0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f,  0.5f, 0.0f,	0.0f, 0.0f, 1.0f, 1.0f,
	};


	U32 triangle_program;
	if (!load_glprogram("./src/triangle.vs", "./src/triangle.fs", &triangle_program)) {
		print_error("failed to load triangle program");
		return 1;
	}


	U32 VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(F32),
			      (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(F32),
			      (void*)(3 * sizeof(F32)));
	glEnableVertexAttribArray(1);


	////////////////////////////////////////////////////////////////////////
	//// Main loop

	F64 frame_duration = 1.0;
	while (!glfwWindowShouldClose(window)) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		F64 start_time = glfwGetTime();

		handle_input(window);


		glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		glUseProgram(triangle_program);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		imgui_start_frame();
		imgui_log_frame_duration(frame_duration);
		imgui_end_frame();


		glfwSwapBuffers(window);
		glfwPollEvents();
		frame_duration = glfwGetTime() - start_time;
	}

	////////////////////////////////////////////////////////////////////////
	//// Cleanup
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glprogram_die(triangle_program);

	imgui_die();
	window_die(window);

	return 0;
}

