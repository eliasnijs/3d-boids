struct Model {
	F32	*vertices;
	U32	n_vertices;
	U32	VAO;
	U32	VBO;
};
#define MODEL(vertices) {vertices, sizeof(vertices)/sizeof(F32)}

struct ModelInstance {
	Model		*model;
	vec3		pos;
	vec3		rot;
	vec3		scale;
};

internal void
load_model(Model *model) {
	U32 VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, model->n_vertices * sizeof(F32),
		     model->vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(F32), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(F32), (void*)(3 * sizeof(F32)));
	glEnableVertexAttribArray(1);

	model->VAO = VAO;
	model->VBO = VBO;
}

internal void
model_instance_initialize(ModelInstance *instance, Model *model) {
	instance->model = model;
	vec3 pos = {}, rot = {}, scale = {1.0f, 1.0f, 1.0f};
	vec3_dup(instance->pos,   pos);
	vec3_dup(instance->rot,   rot);
	vec3_dup(instance->scale, scale);
}

internal void
model_get_transform(ModelInstance *m, mat4x4 t) {
	mat4x4_identity(t);
	mat4x4_translate_in_place(t, m->pos[0], m->pos[1], m->pos[2]);
	mat4x4_rotate_X(t, t, rad(m->rot[0]));
	mat4x4_rotate_Y(t, t, rad(m->rot[1]));
	mat4x4_rotate_Z(t, t, rad(m->rot[2]));
	mat4x4_scale_aniso(t, t, m->scale[0], m->scale[1], m->scale[2]);
}

internal void
model_die(Model *model) {
	glDeleteVertexArrays(1, &model->VAO);
	glDeleteBuffers(1, &model->VBO);
}

internal void
draw_model_instance(ModelInstance *m, U32 shader_program) {
	mat4x4 model_transform;
	model_get_transform(m, model_transform);
	shader_set_mat4x4(shader_program, "model_transform", model_transform);
	glBindVertexArray(m->model->VAO);
	glDrawArrays(GL_TRIANGLES, 0, m->model->n_vertices);
}


