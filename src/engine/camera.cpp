#define CAMERA_PITCH_CLAMP 89.0f
#define CAMERA_FOV_CLAMP_LB 1.0f
#define CAMERA_FOV_CLAMP_UB 45.0f

struct Camera {
	union {
		vec2	dim[3];
		struct	{ F32 window_w, window_h; };
	};
	union {
		vec3	pos;
		struct	{ F32 x, y, z; };
	};
	union {
		vec2	orientation;
		struct	{ F32 pitch, yaw; };
	};

	F32	speed;
	F32	fov;
	F32	sensitivity;

	/* internals: */
	union {
		vec3	dir[3];
		struct	{ vec3 x_axis, y_axis, z_axis; };
	};
	vec3 world_y;
};


internal void
camera_get_view_transform(Camera *camera, mat4x4 view)
{
	vec3 target;
	vec3_add(target, camera->z_axis, camera->pos);
	mat4x4_look_at(view, camera->pos, target, camera->y_axis);
}

internal void
camera_get_perpective_transform(Camera *camera, mat4x4 projection,
				U32 window_width, U32 window_height)
{
	mat4x4_perspective(projection, rad(camera->fov),
			   (F32)window_width / (F32)window_height,
			   0.1f, 100.0f);
}

internal void
camera_move(Camera *camera, Axis axis, AxisDirection dir, F32 dt)
{
	vec3 dir_vec;
	vec3_dup(dir_vec, camera->dir[axis]);
	vec3_scale(dir_vec, dir_vec, dir * dt * camera->speed);
	vec3_add(camera->pos, camera->pos, dir_vec);
}

internal void
camera_aim(Camera *camera, F32 dx, F32 dy)
{
	camera->yaw   = camera->yaw + camera->sensitivity * dx,
	camera->pitch = Clamp(-CAMERA_PITCH_CLAMP,
			      camera->pitch + camera->sensitivity * dy,
			      CAMERA_PITCH_CLAMP);

	vec3 dir;
	dir[0] = cos(rad(camera->yaw)) * cos(rad(camera->pitch));
	dir[1] = sin(rad(camera->pitch));
	dir[2] = sin(rad(camera->yaw)) * cos(rad(camera->pitch));
	vec3_norm(camera->z_axis, dir);

	vec3_mul_cross(camera->x_axis, camera->z_axis, camera->world_y);
	vec3_norm(camera->x_axis, camera->x_axis);

	vec3_mul_cross(camera->y_axis, camera->x_axis, camera->z_axis);
	vec3_norm(camera->y_axis, camera->y_axis);
}

internal void
camera_start(Camera *camera) {
	camera->world_y[0] = 0.0f;
	camera->world_y[1] = 1.0f;
	camera->world_y[2] = 0.0f;
	camera_aim(camera, 0.0f, 0.0f);
}
