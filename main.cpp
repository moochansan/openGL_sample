#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Shape.h"


/**
 * @brief シェーダオブジェクトのコンパイル結果を表示する
 * @param shader: Shaderオブジェクト名
 * @param str:    エラー発生箇所を示す文字列
 * @return        GL_TRUE or GL_FALSE
 */
GLboolean printShaderInfoLog(GLuint shader, const char* str)
{
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) 
		std::cerr << "Compile Error in " << str << std::endl;

	GLsizei bufSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1)
	{
		std::vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetShaderInfoLog(shader, bufSize, &length, &infoLog[0]);
		std::cerr << &infoLog[0] << std::endl;
	}

	return static_cast<GLboolean>(status);
}

/**
 * @brief プログラムオブジェクトのリンク結果を表示する
 * @param program: プログラムオブジェクト
 * @return        GL_TRUE or GL_FALSE
 */
GLboolean printProgramInfoLog(GLuint program)
{
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
		std::cerr << "Link Error." << std::endl;
	
	GLsizei bufSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1)
	{
		std::vector<GLchar> infoLog(bufSize);
		GLsizei length;
		glGetProgramInfoLog(program, bufSize, &length, &infoLog[0]);
	}

	return static_cast<GLboolean>(status);
}

/**
 * @brief プログラムオブジェクトを作成する 
 * @param vsrc: VertexShaderのソースプログラムの文字列
 * @param fsrc: FragmentShaderのソースプログラムの文字列
 * @return      プログラムオブジェクト
 */
GLuint createProgram(const char* vsrc, const char* fsrc)
{
	// 空のプログラムオブジェクトの作成
	const GLuint program(glCreateProgram());

	if (vsrc != NULL)
	{
		// VertexShaderオブジェクトの作成
		const GLuint vobj(glCreateShader(GL_VERTEX_SHADER));
		glShaderSource(vobj, 1, &vsrc, NULL);
		glCompileShader(vobj);

		if (printShaderInfoLog(vobj, "vertex shader"))
			glAttachShader(program, vobj);
		glDeleteShader(vobj);
	}

	if (fsrc != NULL)
	{
		// FragmentShaderオブジェクトの作成
		const GLuint fobj(glCreateShader(GL_FRAGMENT_SHADER));
		glShaderSource(fobj, 1, &fsrc, NULL);
		glCompileShader(fobj);

		if (printShaderInfoLog(fobj, "fragment shader"))
			glAttachShader(program, fobj);
		glDeleteShader(fobj);
	}

	// プログラムオブジェクトをリンクする
	glBindAttribLocation(program, 0, "position");
	glBindFragDataLocation(program, 0, "fragment");
	glLinkProgram(program);

	if (printProgramInfoLog(program))
		return program;

	glDeleteProgram(program);
	return 0;
}

/*
 * @brief シェーダのソースファイルを読み込む
 * @param name:   ファイル名
 * @param buffer: 読み込んだソースファイルのテキスト
 */
bool readShaderSource(const char* name, std::vector<GLchar> &buffer)
{
	if (name == NULL) return false;

	std::ifstream file(name, std::ios::binary);
	if (file.fail())
	{
		std::cerr << "Error: Can't open source file: " << name << std::endl;
		return false;
	}

	file.seekg(0L, std::ios::end);
	GLsizei length = static_cast<GLsizei>(file.tellg());
	buffer.resize(length + 1);

	file.seekg(0L, std::ios::beg);
	file.read(buffer.data(), length);
	buffer[length] = '\0';

	if (file.fail())
	{
		std::cerr << "Error: Could not read source file: " << name << std::endl;
		file.close();
		return false;
	}

	file.close();
	return true;
}

/*
 * @brief シェーダのソースファイルを読み込んでプログラムオブジェクトを作成する
 * @param vert: VertexShaderのソースファイル名
 * @param frah: FragmentShaderのソースファイル名
 * @return      どちらも読み込み成功すればプログラムオブジェクトを返す
 */
GLuint loadProgram(const char* vert, const char* frag)
{
	std::vector<GLchar> vsrc;
	const bool vstat(readShaderSource(vert, vsrc));
	std::vector<GLchar> fsrc;
	const bool fstat(readShaderSource(frag, fsrc));

	return vstat && fstat ? createProgram(vsrc.data(), fsrc.data()) : 0;
}


constexpr Object::Vertex rectangleVertex[] =
{
	{-0.5f, -0.5f},
	{ 0.5f, -0.5f},
	{ 0.5f,  0.5f},
	{-0.5f,  0.5f}
};

int main()
{
	// GLFWの初期化
	if (glfwInit() == GL_FALSE)
	{
		std::cerr << "Can't initialize GLFW" << std::endl;
		return 1;
	}

	// 終了時の処理を登録
	atexit(glfwTerminate);

	// OpenGL Version 3.2 Core Profile を選択する
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	Window window;
	
	glClearColor(1.f, 1.f, 1.f, 0.f);


	const GLuint program(loadProgram("point.vert", "point.frag"));

	const GLuint aspectLoc(glGetUniformLocation(program, "aspect"));

	std::unique_ptr<const Shape> shape(new Shape(2, 4, rectangleVertex));

	while (window)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program);
		
		glUniform1f(aspectLoc, window.getAspect());
		shape->draw();

		window.swapBuffers();
	}
}
