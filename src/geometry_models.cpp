struct GeometryModel {
	F32	*vertices;
	U32	n_vertices;
};
#define GEOMETRY_MODEL(vertices) {vertices, sizeof(vertices)/sizeof(F32)}

global_variable F32 default_cube_vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
global_variable GeometryModel DEFAULT_CUBE_MODEL = GEOMETRY_MODEL(default_cube_vertices);
global_variable GeometryModel *DEFAULT_CUBE = &DEFAULT_CUBE_MODEL;


struct ModelInstance {
	GeometryModel	*model;
	vec3		position;
	vec3		rotation;
	vec3		scale;
};

internal void
model_instance_initialize(ModelInstance *instance, GeometryModel *model) {
	instance->model = model;
	vec3 pos = {}, rot = {}, scale = {1.0f, 1.0f, 1.0f};
	vec3_dup(instance->position, pos);
	vec3_dup(instance->rotation, rot);
	vec3_dup(instance->scale, scale);
}


