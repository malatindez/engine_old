
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION
#include "Mesh.h"
#define CERR_OUTPUT
#include "TextureManager.h"
#include "core.h"
#define SCR_WIDTH 800
#define SCR_HEIGHT 600
#define SCR_RES SCR_WIDTH, SCR_HEIGHT
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_RES, "LearnOpenGL", NULL, NULL);
		
	glfwMakeContextCurrent(window);

	if (window == NULL) {
#ifdef CERR_OUTPUT
		std::cerr << "Failed to create GLFW window" << std::endl;
#endif
		glfwTerminate();
		return -1;
	}
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
#ifdef CERR_OUTPUT
		std::cerr << "Failed to initialize GLAD" << std::endl;
#endif
		return -1;
	}
	ShaderWorker s(ShaderWorker::LoadSourceCode("triangle.vert"), ShaderWorker::LoadSourceCode("triangle.frag"));
	s.assemble();



}