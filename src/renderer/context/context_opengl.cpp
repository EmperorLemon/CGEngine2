//#include <glad/gl.h>
//#include <GLFW/glfw3.h>
//#include <cstdio>
//
//#include "context.h"
//#include "types.h"
//#include "platform/window.h"
//
//// context_opengl.cpp
//namespace cg::renderer::ContextOps
//{
//	static void SwapBuffers(void* window);
//	static void QueryPhysicalDevice(CGPhysicalDeviceProperties& deviceProperties);
//
//	void QueryPhysicalDevice(CGPhysicalDeviceProperties& deviceProperties)
//	{
//
//		//glGetIntegerv(GL_MAJOR_VERSION, &deviceProperties.versionMajor);
//		//glGetIntegerv(GL_MINOR_VERSION, &deviceProperties.versionMinor);
//
//		GLint numExtensions;
//		glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
//

//
//		for (GLint i = 0; i < numExtensions; ++i)
//		{
//			const char* extensionName = reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, i));
//		}
//	}
//
//	static void SwapBuffers(void* window)
//	{
//		glfwSwapBuffers(static_cast<GLFWwindow*>(window));
//	}
//
//	void SetupOpenGLContextFunctions(CGRenderContext& renderContext)
//	{
//		renderContext.apiFunctions.present = SwapBuffers;
//	}
//
//	void DestroyOpenGLContext(CGRenderContext& renderContext)
//	{
//		//DestroyOpenGLBuffers(renderContext.bufferPool);
//		//DestroyOpenGLShaderPrograms();
//	}
//}
