// -*- c-file-style: "stroustrup" -*-
#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
    std::cerr << "Compile: " << errorMessage << std::endl;
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
    std::cerr << "Link: " << errorMessage << std::endl;
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
    {
	std::cerr << "Failed to init GLFW " << std::endl;
	exit(-1);
    }

    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (!glfwOpenWindow(320, 240, 0, 0, 0, 0, 32, 0, GLFW_WINDOW))
    {
	glfwTerminate();
	exit(-1);
    }

    glewExperimental = GL_TRUE;
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK)
    {
	std::cerr << "Failed to init GLEW " << glewGetErrorString(glewErr) << std::endl;
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

    static const GLfloat colorData[] = {
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
    };

    GLuint colorBuffer;
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);

    const char shaderSource[] = ""
	"#version 330 core\n"
	"layout(location = 0) in vec3 in_Position;\n"
	"layout(location = 1) in vec3 in_Color;\n"
	"uniform mat4 MVP;\n"
	"out vec4 ex_Color;\n"
	"void main() {\n"
	"  gl_Position = MVP * vec4(in_Position, 1.0);\n"
	"  ex_Color = vec4(in_Color, 1.0);\n"
	"}";

    const char fragmentSource[] = ""
	"#version 330 core\n"
	"in vec4 ex_Color;\n"
	"out vec4 out_Color;\n"
	"void main() {\n"
	"  out_Color = ex_Color;\n"
	"}";
    
    GLuint programId = LinkProgram(shaderSource, fragmentSource);
    GLuint matrixId = glGetUniformLocation(programId, "MVP");

    glm::mat4 projection = glm::perspective(45.0f, 4.0f/3.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(4.0f, 3.0f, 3.0f),
				 glm::vec3(0.0f, 0.0f, 0.0f),
				 glm::vec3(0.0f, 1.0f, 0.0f));
    float angle = 0.0;

    while (running)
    {
	glm::mat4 model = glm::rotate(glm::mat4(1.0f), 
				      angle, 
				      glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 MVP = projection * view * model;

	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(programId);

	glUniformMatrix4fv(matrixId, 1, GL_FALSE, &MVP[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	
	glfwSwapBuffers();

	angle += 2.0;
	
	running = !glfwGetKey(GLFW_KEY_ESC)
	    && glfwGetWindowParam(GLFW_OPENED);
    }

    glfwTerminate();

    return 0;
}

