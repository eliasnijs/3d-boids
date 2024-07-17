
struct Camera {
	vec3 pos;
	vec3 front;
	vec3 up;
};

internal void
camera_get_view_transform(Camera *camera, mat4x4 view)
{
	vec3 target;
	vec3_add(target, camera->front, camera->pos);
	mat4x4_look_at(view, camera->pos, target, camera->up);
}

