
internal inline void
imgui_log_boids_app(BoidsApplication *boids_app)
{
	ImGui::Begin("Boid controls", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::Checkbox("Pause", (bool *)&boids_app->paused);
	ImGui::SliderInt("Number of Boids", &boids_app->n, 0, MAX_BOIDS);
	ImGui::SliderFloat("Cohesion", &boids_app->p.c, 0.0f, 1.0f);
	ImGui::SliderFloat("Separation", &boids_app->p.s, 0.0f, 1.0f);
	ImGui::SliderFloat("Alignment", &boids_app->p.a, 0.0f, 1.0f);
	ImGui::SliderFloat("Separation Radius", &boids_app->p.s_r, 0.0f, 100.0f);
	ImGui::SliderFloat("Radius", &boids_app->p.r, 0.0f, 500.0f);
	ImGui::SliderFloat("View angle", &boids_app->p.theta_max, 0.0f, 3.14f/2.0f);
	ImGui::SliderFloat("Max Velocity", &boids_app->p.max_vel, 1.0f, 20.0f);
	ImGui::SliderFloat("Point Size", &boids_app->p.size, 1.0f, 10.0f);
	ImGui::InputInt3("Simulation Space", boids_app->p.simulation_space);
	boids_app->p.simulation_space[0] = Clamp(100, boids_app->p.simulation_space[0], 10000);
	boids_app->p.simulation_space[1] = Clamp(100, boids_app->p.simulation_space[1], 10000);
	boids_app->p.simulation_space[2] = Clamp(100, boids_app->p.simulation_space[2], 10000);
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
	ImGui::InputFloat2("Orientation", camera->orientation);
	ImGui::SliderFloat("Speed", &camera->speed, 0.1f, 100.0f);
	ImGui::SliderFloat("FOV", &camera->fov, CAMERA_FOV_CLAMP_LB, CAMERA_FOV_CLAMP_UB);
	ImGui::SliderFloat("Sensitivity", &camera->sensitivity, 0.01f, 1.0f);
	ImGui::End();
}

internal inline void
gui(GameState *game_state)
{
	imgui_start_frame();
	imgui_log_frame_duration(game_state->dt);
	imgui_log_camera(&game_state->camera);
	imgui_log_boids_app(&game_state->boidsapp);
	imgui_end_frame();
}

