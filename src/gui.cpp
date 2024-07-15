
internal inline void
imgui_log_cube(Cube *cube)
{
	ImGui::Begin("Transform");
	ImGui::End();
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

internal inline void
gui(F64 frame_duration, Cube *cube)
{
	imgui_start_frame();
	imgui_log_frame_duration(frame_duration);
	imgui_log_cube(cube);
	imgui_end_frame();
}

