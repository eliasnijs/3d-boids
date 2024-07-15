
struct Cube {
	F32	vertices[8 * 8];
	U32     indices[6 * 6];
};

internal void
cube_initialize(Cube *c) {
	F32 vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f,  0.0f,   0.4f, 0.4f, 0.4f,   1.0f, 1.0f, // 0: front right top
		 0.5f, -0.5f,  0.0f,   0.4f, 0.4f, 0.4f,   1.0f, 0.0f, // 1: front right bottom
		-0.5f, -0.5f,  0.0f,   0.4f, 0.4f, 0.4f,   0.0f, 0.0f, // 2: front left bottom
		-0.5f,  0.5f,  0.0f,   0.4f, 0.4f, 0.4f,   0.0f, 1.0f, // 3: front left top
	};
	memcpy(c->vertices, vertices, sizeof(vertices));
	U32 indices[] = {
		/* front:  */	0, 1, 3,   1, 2, 3,
	};
	memcpy(c->indices, indices, sizeof(indices));
}

