
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION
#include "Mesh.h"
#define CERR_OUTPUT
#include "TextureManager.h"
#include "core.h"
#define SCR_WIDTH 1920
#define SCR_HEIGHT 1080
#define SCR_RES SCR_WIDTH, SCR_HEIGHT
#include "Actor.h"
#include "Player.h"
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_RES, "engine", NULL, NULL);
		
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
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	ShaderWorker s(ShaderWorker::LoadSourceCode("triangle.vert"), ShaderWorker::LoadSourceCode("triangle.frag"));
	std::shared_ptr<ShaderProgram> shader(s.assemble());
	Input input(window);
	Player player(&input, glm::vec3(0,1,0));
	TextureManager* manager = new TextureManager();
	Actor<1> actor("WineBarrel\\objBarrel.obj", manager);
	float deltaTime = 0;
	float lastFrame = 0;
	unsigned int frameTick = 0;
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
        input.Update(frameTick, deltaTime);
        player.Update(frameTick, deltaTime);
        actor.Update(frameTick, deltaTime);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
		shader->use();
		shader->setMat4("view", player.camera->GetViewMatrix());
		glm::mat4 projection = glm::perspective(
			glm::radians(player.camera->getFOV()),
			(float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		shader->setMat4("projection", projection);
		actor.Draw(shader);
		frameTick++;
        glfwSwapBuffers(window);
        glfwPollEvents();
	}
}