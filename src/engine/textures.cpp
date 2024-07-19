
struct Texture {
	U32 id;
	U32 w, h, c;
};

internal B32
load_texture_param(char *path, Texture *texture, I32 wrap_s, I32 wrap_t,
		     I32 min_filter, I32 mag_filter) {
	U32 t;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

	I32 w, h, c;
	U8 *data = stbi_load("./resources/texture.png", &w, &h, &c, 0);
	if (!data) {
		print_error("failed to load t");
		return false;
	}

	switch (c) {
	case 3: {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	} break;
	case 4: {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	} break;
	case 1: {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);
	} break;
	default: {
		print_error("unsupported number of channels");
		return false;
	} break;
	}

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	texture->id = t;
	texture->w = w;
	texture->h = h;
	texture->c = c;
	return true;
}

#define TEXTURE_DEFAULT_WRAP_S		GL_REPEAT
#define TEXTURE_DEFAULT_WRAP_T		GL_REPEAT
#define TEXTURE_DEFAULT_MIN_FILTER	GL_NEAREST
#define TEXTURE_DEFAULT_MAG_FILTER	GL_NEAREST

internal B32
load_texture(char *path, Texture *texture)
{
	return load_texture_param(path, texture,
				  TEXTURE_DEFAULT_WRAP_S,
				  TEXTURE_DEFAULT_WRAP_T,
				  TEXTURE_DEFAULT_MIN_FILTER,
				  TEXTURE_DEFAULT_MAG_FILTER);
}


internal void
use_texture(Texture *texture, U32 slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texture->id);
}

internal void
texture_die(Texture *texture)
{
	glDeleteTextures(1, &texture->id);
}

