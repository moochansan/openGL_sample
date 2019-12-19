#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.h"
#include "Matrix.h"
#include "Shape.h"
#include "ShapeIndex.h"
#include "SolidShapeIndex.h"
#include "SolidShape.h"


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
	glBindAttribLocation(program, 1, "color");
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

constexpr Object::Vertex octahedronVertex[] =
{
	{  0.0f,  1.0f,  0.0f },
	{ -1.0f,  0.0f,  0.0f },
	{  0.0f, -1.0f,  0.0f },
	{  1.0f,  0.0f,  0.0f },
	{  0.0f,  1.0f,  0.0f },
	{  0.0f,  0.0f,  1.0f },
	{  0.0f, -1.0f,  0.0f },
	{  0.0f,  0.0f, -1.0f },
	{ -1.0f,  0.0f,  0.0f },
	{  0.0f,  0.0f,  1.0f },
	{  1.0f,  0.0f,  0.0f },
	{  0.0f,  0.0f, -1.0f }
};

constexpr Object::Vertex cubeVertex[] =
{
 { -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  0.0f },
 { -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  0.8f },
 { -1.0f,  1.0f,  1.0f,  0.0f,  0.8f,  0.0f },
 { -1.0f,  1.0f, -1.0f,  0.0f,  0.8f,  0.8f },
 {  1.0f,  1.0f, -1.0f,  0.8f,  0.0f,  0.0f },
 {  1.0f, -1.0f, -1.0f,  0.8f,  0.0f,  0.8f },
 {  1.0f, -1.0f,  1.0f,  0.8f,  0.8f,  0.0f },
 {  1.0f,  1.0f,  1.0f,  0.8f,  0.8f,  0.8f }
};

constexpr GLuint wireCubeIndex[] =
{
 1, 0,
 2, 7,
 3, 0,
 4, 7,
 5, 0,
 6, 7,
 1, 2,
 2, 3,
 3, 4,
 4, 5,
 5, 6,
 6, 1
};

constexpr GLuint solidCubeIndex[] =
{
	0, 1, 2, 0, 2, 3, 
	0, 3, 4, 0, 4, 5,
	0, 5, 6, 0, 6, 1,
	7, 6, 5, 7, 5, 4,
	7, 4, 3, 7, 3, 2,
	7, 2, 1, 7, 1, 6
};

constexpr Object::Vertex solidCubeVertex[] =
{
	// 左
	{ -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.1f },
	{ -1.0f, -1.0f,  1.0f, 0.1f, 0.8f, 0.1f },
	{ -1.0f,  1.0f,  1.0f, 0.1f, 0.8f, 0.1f },
	{ -1.0f,  1.0f, -1.0f, 0.1f, 0.8f, 0.1f },
	// 裏
	{  1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	{ -1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	{ -1.0f,  1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	{  1.0f,  1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	// 下
	{ -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
	{  1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
	{  1.0f, -1.0f,  1.0f, 0.1f, 0.8f, 0.8f },
	{ -1.0f, -1.0f,  1.0f, 0.1f, 0.8f, 0.8f },
	// 右
	{  1.0f, -1.0f,  1.0f, 0.1f, 0.1f, 0.8f },
	{  1.0f, -1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
	{  1.0f,  1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
	{  1.0f,  1.0f,  1.0f, 0.1f, 0.1f, 0.8f },
	// 上
	{ -1.0f,  1.0f, -1.0f, 0.8f, 0.1f, 0.1f },
	{ -1.0f,  1.0f,  1.0f, 0.8f, 0.1f, 0.1f },
	{  1.0f,  1.0f,  1.0f, 0.8f, 0.1f, 0.1f },
	{  1.0f,  1.0f, -1.0f, 0.8f, 0.1f, 0.1f },
	// 前
	{ -1.0f, -1.0f,  1.0f, 0.8f, 0.8f, 0.1f },
	{  1.0f, -1.0f,  1.0f, 0.8f, 0.8f, 0.1f },
	{  1.0f,  1.0f,  1.0f, 0.8f, 0.8f, 0.1f },
	{ -1.0f,  1.0f,  1.0f, 0.8f, 0.8f, 0.1f }
};

constexpr GLuint solidCubeFaceColorIndex[] =
{
	 0,  1,  2,  0,  2,  3,
	 4,  5,  6,  4,  6,  7,
	 8,  9, 10,  8, 10, 11,
	12, 13, 14, 12, 14, 15,
	16, 17, 18, 16, 18, 19,
	20, 21, 22, 20, 22, 23
};

constexpr Object::Vertex solidCubeVertex36[] =
{
	// 左
	{ -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.1f },
	{ -1.0f, -1.0f,  1.0f, 0.1f, 0.8f, 0.1f },
	{ -1.0f,  1.0f,  1.0f, 0.1f, 0.8f, 0.1f },
	{ -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.1f },
	{ -1.0f,  1.0f,  1.0f, 0.1f, 0.8f, 0.1f },
	{ -1.0f,  1.0f, -1.0f, 0.1f, 0.8f, 0.1f },
	// 裏
	{  1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	{ -1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	{ -1.0f,  1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	{  1.0f, -1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	{ -1.0f,  1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	{  1.0f,  1.0f, -1.0f, 0.8f, 0.1f, 0.8f },
	// 下
	{ -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
	{  1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
	{  1.0f, -1.0f,  1.0f, 0.1f, 0.8f, 0.8f },
	{ -1.0f, -1.0f, -1.0f, 0.1f, 0.8f, 0.8f },
	{  1.0f, -1.0f,  1.0f, 0.1f, 0.8f, 0.8f },
	{ -1.0f, -1.0f,  1.0f, 0.1f, 0.8f, 0.8f },
	// 右
	{  1.0f, -1.0f,  1.0f, 0.1f, 0.1f, 0.8f },
	{  1.0f, -1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
	{  1.0f,  1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
	{  1.0f, -1.0f,  1.0f, 0.1f, 0.1f, 0.8f },
	{  1.0f,  1.0f, -1.0f, 0.1f, 0.1f, 0.8f },
	{  1.0f,  1.0f,  1.0f, 0.1f, 0.1f, 0.8f },
	// 上
	{ -1.0f,  1.0f, -1.0f, 0.8f, 0.1f, 0.1f },
	{ -1.0f,  1.0f,  1.0f, 0.8f, 0.1f, 0.1f },
	{  1.0f,  1.0f,  1.0f, 0.8f, 0.1f, 0.1f },
	{ -1.0f,  1.0f, -1.0f, 0.8f, 0.1f, 0.1f },
	{  1.0f,  1.0f,  1.0f, 0.8f, 0.1f, 0.1f },
	{  1.0f,  1.0f, -1.0f, 0.8f, 0.1f, 0.1f },
	// 前
	{ -1.0f, -1.0f,  1.0f, 0.8f, 0.8f, 0.1f },
	{  1.0f, -1.0f,  1.0f, 0.8f, 0.8f, 0.1f },
	{  1.0f,  1.0f,  1.0f, 0.8f, 0.8f, 0.1f },
	{ -1.0f, -1.0f,  1.0f, 0.8f, 0.8f, 0.1f },
	{  1.0f,  1.0f,  1.0f, 0.8f, 0.8f, 0.1f },
	{ -1.0f,  1.0f,  1.0f, 0.8f, 0.8f, 0.1f }
};

constexpr GLuint solidCubeFaceColorIndex36[] =
{
	 0,  1,  2,  3,  4,  5,
	 6,  7,  8,  9, 10, 11,
	12, 13, 14, 15, 16, 17,
	18, 19, 20, 21, 22, 23,
	24, 25, 26, 27, 28, 29,
	30, 31, 32, 33, 34, 35
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

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

#ifdef _WIN64
	const char* vertFile = "..\\sample\\point.vert";
	const char* fragFile = "..\\sample\\point.frag";
#else
	const char* vertFile = "point.vert";
	const char* fragFile = "point.frag";
#endif


	const GLuint program(loadProgram(vertFile, fragFile));

	const GLint modelviewLoc(glGetUniformLocation(program, "modelview"));
	const GLint projectionLoc(glGetUniformLocation(program, "projection"));

	std::unique_ptr<const Shape> shape(new Shape(3, 12, octahedronVertex));
	std::unique_ptr<const Shape> shapeCube(new ShapeIndex(3, 8, cubeVertex, 24, wireCubeIndex));
	std::unique_ptr<const Shape> shapeCubeTriangles(new SolidShapeIndex(3, 24, solidCubeVertex, 36, solidCubeFaceColorIndex));	
	std::unique_ptr<const Shape> shapeCubeTriangles36(new SolidShapeIndex(3, 36, solidCubeVertex36, 36, solidCubeFaceColorIndex36));	

	glfwSetTime(0.0);

	while (window)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(program);
		
		const GLfloat* const size(window.getSize());

		const GLfloat fovy(window.getScale() * 0.01f);
		const GLfloat aspect(size[0] / size[1]);
		const Matrix projection(Matrix::perspective(fovy, aspect, 1.f, 10.f));

		const GLfloat* const position(window.getLocation());
		const Matrix r(Matrix::rotate(static_cast<GLfloat>(glfwGetTime()), 0.f, 1.f, 0.f));
		const Matrix model(Matrix::translate(position[0], position[1], 0.f) * r);

		const Matrix view(Matrix::lookat(3.f, 4.f, 5.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f));

		const Matrix modelview(view * model);

		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection.data());
		glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, modelview.data());

//		shape->draw();
//		shapeCube->draw();
		shapeCubeTriangles36->draw();

		const Matrix modelView1(modelview * Matrix::translate(0.f, 0.f, 3.f));
		glUniformMatrix4fv(modelviewLoc, 1, GL_FALSE, modelView1.data());
		shapeCubeTriangles->draw();

		window.swapBuffers();
	}
}
