#include "EnginePCH.h"
#include "Render.h"

using namespace Engine;

// TODO: on android put GLES...
#include "../vendor/glad/include/glad/glad.h"
#include "../vendor/glad/include/KHR/khrplatform.h"
#include <GLFW/glfw3.h>

void RenderAPI::InitForContext(){
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void RenderAPI::ClearFB(float4 rgba) {
	glClearColor(rgba.x, rgba.y, rgba.z, rgba.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}