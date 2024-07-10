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

#define ENABLE_PRINT  1
#define ENABLE_ASSERT 1
#include "utils/utils.h"

#include "window.cpp"
#include "imgui.cpp"


internal void
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
	GLFWwindow *window = window_init(960, 540, "Hello, World!");
	if (!window) {
		print_error("failed to initialize window");
		return 1;
	}

	imgui_initialize(window);

	F64 frame_duration = 1.0;
	while (!glfwWindowShouldClose(window)) {
		F64 start_time = glfwGetTime();


		handle_input(window);


		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		imgui_start_frame();
		imgui_log_frame_duration(frame_duration);
		imgui_end_frame();


		glfwSwapBuffers(window);
		glfwPollEvents();


		frame_duration = glfwGetTime() - start_time;
	}

	imgui_die();
	window_die(window);

	return 0;
}

