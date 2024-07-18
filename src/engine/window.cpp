
internal void
simple_error_callback(I32 error, const char* description) {
	fprintf(stderr, "GLFW error: %s\n", description);
}

internal void
framebuffer_size_callback(GLFWwindow* window, I32 width, I32 height) {
	glViewport(0, 0, width, height);
}

internal GLFWwindow *
window_init(I32 width, I32 height, const char* title) {
	setenv("DISPLAY", ":0", 1);

	glfwSetErrorCallback(simple_error_callback);
	if (!glfwInit()) {
		print_error("Failed to initialize GLFW");
		return 0x0;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);

	if (!window) {
		print_error("Failed to create window");
		glfwTerminate();
		return 0x0;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		print_error("Failed to initialize GLAD");
		glfwTerminate();
		return 0x0;
	}

	return window;
}

internal void
window_die(GLFWwindow* window) {
	glfwDestroyWindow(window);
	glfwTerminate();
}

