
global_variable U32 selected_instance = 0;
internal inline void
imgui_log_model_instance(ModelInstance *instances, U32 n_instances)
{
	ImGui::Begin("Instances");
	ImGui::Text("Instances: %d", n_instances);
	if (ImGui::BeginListBox("Instances", ImVec2(-FLT_MIN, -FLT_MIN))) {
		for (U32 i = 0; i < n_instances; ++i) {
			char label[64];
			snprintf(label, 64, "Instance %d", i);
			if (ImGui::Selectable(label, i == selected_instance)) {
				selected_instance = i;
			}
		}
		ImGui::EndListBox();
	}
	ImGui::End();
	ImGui::Begin("Instance");
	ImGui::Text("Instance %d", selected_instance);
	ImGui::InputFloat3("Position", instances[selected_instance].pos);
	ImGui::InputFloat3("Rotation", instances[selected_instance].rot);
	ImGui::InputFloat3("Scale", instances[selected_instance].scale);
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
imgui_log_camera(Camera *camera)
{
	ImGui::Begin("Camera");
	ImGui::InputFloat3("Position", camera->pos);
	ImGui::InputFloat3("Orientation", camera->orientation);
	ImGui::SliderFloat("Speed", &camera->speed, 0.1f, 10.0f);
	ImGui::SliderFloat("FOV", &camera->fov, 1.0f, 179.0f);
	ImGui::SliderFloat("Sensitivity", &camera->sensitivity, 0.01f, 1.0f);
	ImGui::End();
}

internal inline void
gui(GameState *game_state, ModelInstance *instances, U32 n_instances)
{
	imgui_start_frame();
	imgui_log_frame_duration(game_state->dt);
	imgui_log_model_instance(instances, n_instances);
	imgui_log_camera(&game_state->camera);
	imgui_end_frame();
}

