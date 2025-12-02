#include <GL/glew.h>
#include <GLFW/glfw3.h>

int frameEvent(double deltaTime);
int frameRendering(double deltaTime);
int frameSecond(double deltaTime);
void initialisation();

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void button_callback(GLFWwindow *window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);