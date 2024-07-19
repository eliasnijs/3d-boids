
internal bool
load_glshader(char *path, U32 type, U32 *shader_id) {
	FILE *f = fopen(path, "r");
	if (!f) {
		print_error("failed to open shader path");
		return false;
	}

	U32 flength = 0;
	fseek(f, 0, SEEK_END);
	flength = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *fbuffer = (char *)calloc(flength + 1, sizeof(char));
	if (!fbuffer) {
		print_error("failed to allocate memory for shader");
		return false;
	}
	U32 bytesread = fread(fbuffer, sizeof(char), flength, f);
	if (bytesread < flength) {
		print_error("reading file failed");
		free(fbuffer);
		return false;
	}

	U32 id = glCreateShader(type);
	glShaderSource(id, 1, (const char * const *)&fbuffer, 0);
	glCompileShader(id);

	I32 success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success) {
		char info[1024] = {0};
		glGetShaderInfoLog(id, 1024, 0, info);
		print_error("failed to compile shader component (%s):\n%s",
		       path, info);
		free(fbuffer);
		return false;
	}
	*(shader_id) = id;

	free(fbuffer);
	return true;
}

internal bool
load_glprogram(char *name, U32 *program_id) {
	char vertexPath[1024] = {0};
	char fragmentPath[1024] = {0};
	snprintf(vertexPath, 1024, "./src/shaders/%s.vs", name);
	snprintf(fragmentPath, 1024, "./src/shaders/%s.fs", name);

	U32 vertex_shader, fragment_shader;
	I32 success;

	success = load_glshader(vertexPath, GL_VERTEX_SHADER, &vertex_shader);
	if (!success) {
		print_error("failed to load vertex shader");
		return false;
	}
	success = load_glshader(fragmentPath, GL_FRAGMENT_SHADER, &fragment_shader);
	if (!success) {
		print_error("failed to load fragment shader");
		glDeleteShader(vertex_shader);
		return false;
	}

	U32 id = glCreateProgram();
	glAttachShader(id, vertex_shader);
	glAttachShader(id, fragment_shader);
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		char info[1024] = {0};
		glGetProgramInfoLog(id, 1024, 0, info);
		print_error("failed to create opengl program, %s", info);
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		glDeleteProgram(id);
		return false;
	}

	*(program_id) = id;

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return true;
}

internal void
glprogram_die(U32 program_id) {
	glDeleteProgram(program_id);
}

internal void
shader_set_B32(I32 shader_id, char *name, B32 value) {
  glUniform1i(glGetUniformLocation(shader_id, name), (I32)value);
}

internal void
shader_set_I32(I32 shader_id, char *name, I32 value) {
  glUniform1i(glGetUniformLocation(shader_id, name), value);
}

internal void
shader_set_F32(I32 shader_id, char *name, F32 value) {
  glUniform1f(glGetUniformLocation(shader_id, name), value);
}

internal void
shader_set_vec3(I32 shader_id, char *name, vec3 v) {
  glUniform3fv(glGetUniformLocation(shader_id, name), 1, v);
}

internal void
shader_set_mat4x4(I32 shader_id, char *name, mat4x4 m) {
  glUniformMatrix4fv(glGetUniformLocation(shader_id, name), 1, GL_FALSE, &m[0][0]);
}

internal void
shader_set_camera_transforms(I32 shader_id, Camera *camera) {
  mat4x4 Tproj, Tview;
  camera_get_view_transform(camera, Tview);
  camera_get_perpective_transform(camera, Tproj,
				  camera->window_w, camera->window_h);
  shader_set_mat4x4(shader_id, "projection_transform", Tproj);
  shader_set_mat4x4(shader_id, "view_transform", Tview);
}

