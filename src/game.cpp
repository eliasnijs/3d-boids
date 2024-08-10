
internal inline void
game_start(GameState *game_state, I32 window_width, I32 window_height)
{
	game_state->type = GAME_STATE_PLAY;
	game_state->dt = 0.0f;
	game_state->camera = {
		{(F32)window_width, (F32)window_height},
		{0.0f, 0.0f, 3.0f},
		{0.0f, 0.0f},
		1.0f, 45.0f, 0.1f
	};
	camera_start(&game_state->camera);

	init_boids_app(&game_state->boidsapp);
}

internal inline void
game_update(GameState *game_state, Controller *controller)
{
	/* handle input */
	if (game_state->type == GAME_STATE_PLAY) {
		Camera *camera = &game_state->camera;
		if (controller->move_forward.is_down) {
			camera_move(camera, AXIS_Z, AXIS_DIR_INC, game_state->dt);
		}
		if (controller->move_backward.is_down) {
			camera_move(camera, AXIS_Z, AXIS_DIR_DEC, game_state->dt);
		}
		if (controller->move_right.is_down) {
			camera_move(camera, AXIS_X, AXIS_DIR_INC, game_state->dt);
		}
		if (controller->move_left.is_down) {
			camera_move(camera, AXIS_X, AXIS_DIR_DEC, game_state->dt);
		}
		if (controller->move_up.is_down) {
			camera_move(camera, AXIS_Y, AXIS_DIR_INC, game_state->dt);
		}
		if (controller->move_down.is_down) {
			camera_move(camera, AXIS_Y, AXIS_DIR_DEC, game_state->dt);
		}

		MouseState *mouse = &controller->mouse;
		F64 ds = mouse->scroll - mouse->scroll_prev;
		camera->fov = Clamp(CAMERA_FOV_CLAMP_LB,
				    camera->fov - ds,
				    CAMERA_FOV_CLAMP_UB);
		camera_aim(camera, mouse->xpos - mouse->xpos_prev,
			   mouse->ypos_prev - mouse->ypos);
	}

	if (button_released(&controller->pause)) {
		if (game_state->type == GAME_STATE_PLAY) {
			game_state->type = GAME_STATE_MENU;
			glfwSetInputMode(game_state->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		} else {
			game_state->type = GAME_STATE_PLAY;
			glfwSetInputMode(game_state->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	update_boids(&game_state->boidsapp);
}

internal inline void
game_die(GameState *game_state)
{
	boids_app_die(&game_state->boidsapp);
}



