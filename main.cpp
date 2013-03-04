#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <iostream>
#include <unistd.h>

template <GLuint>
void PrintError(GLuint) { }

template <>
void PrintError<GL_COMPILE_STATUS>(GLuint shaderId)
{
    GLint result = GL_FALSE;
    int infoLength;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLength);
    std::string errorMessage(std::max(1, infoLength), char());
    glGetShaderInfoLog(shaderId, infoLength, NULL, &errorMessage[0]);
    fprintf(stderr, "Compile: %s", errorMessage.c_str());
}

template <>
void PrintError<GL_LINK_STATUS>(GLuint programId)
{
    GLint result = GL_FALSE;
    int infoLength;
    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLength);
    std::string errorMessage(std::max(1, infoLength), char());
    glGetProgramInfoLog(programId, infoLength, NULL, &errorMessage[0]);
    fprintf(stderr, "Link message: %s", errorMessage.c_str());
}

GLuint CompileShader(GLuint shaderType, const char* shaderCode)
{
    GLuint shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shaderCode, NULL);
    glCompileShader(shaderId);
    PrintError<GL_COMPILE_STATUS>(shaderId);
    return shaderId;
}

GLuint LinkProgram(const char* vertexShader, const char* fragmentShader)
{
    GLuint vertexShaderId = CompileShader(GL_VERTEX_SHADER, vertexShader);
    GLuint fragmentShaderId = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);

    PrintError<GL_LINK_STATUS>(programId);

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    return programId;
}

int main()
{
    bool running = true;

    if (!glfwInit())
	exit(-1);

    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (!glfwOpenWindow(320, 240, 0, 0, 0, 0, 32, 0, GLFW_WINDOW))
    {
	glfwTerminate();
	exit(-1);
    }

    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
	glfwTerminate();
	exit(-1);
    }

    glfwSetWindowTitle("Hello World!");

    GLuint vertexArrayId;
    glGenVertexArrays(1, &vertexArrayId);
    glBindVertexArray(vertexArrayId);

    static const GLfloat vertexBufferData[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
    };

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

    const char shaderSource[] = ""
	"#version 330 core\n"
	"layout(location = 0) in vec3 vertexPosition_modelspace;\n"
	"void main() {\n"
	"  gl_Position.xyz = vertexPosition_modelspace;\n"
	"  gl_Position.w = 1.0;\n"
	"}";

    const char fragmentSource[] = ""
	"#version 330 core\n"
	"out vec3 color;\n"
	"void main() {\n"
	"  color = vec3(1,0,0);\n"
	"}";
    
    GLuint programId = LinkProgram(shaderSource, fragmentSource);

    while (running)
    {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programId);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	
	glDisableVertexAttribArray(0);
	
	glfwSwapBuffers();
	
	running = !glfwGetKey(GLFW_KEY_ESC)
	    && glfwGetWindowParam(GLFW_OPENED);
    }

    glfwTerminate();

    return 0;
}

