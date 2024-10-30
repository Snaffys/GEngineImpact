#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Application.h>
#include <Window_class.h>
#include <Shader_class.h>
#include <VBO_class.h>
#include <VAO_class.h>

int main()
{
    Application application;
    
    Window window;
    
    window.Go();
    
    return 0;
}