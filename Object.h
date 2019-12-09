#pragma once
#include <GL/glew.h>

class Object
{
private:
	GLuint vao;
	GLuint vbo;

public:
	struct Vertex
	{
		GLfloat position[3];
	};

	Object(GLint size, GLsizei vertexcount, const Vertex *vertex)
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertexcount * sizeof(Vertex), vertex, GL_STATIC_DRAW);

		glVertexAttribPointer(0, size, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
	}

	virtual ~Object()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}

private:
	Object(const Object &o);
	Object &operator=(const Object &o);

public:
	void bind() const
	{
		glBindVertexArray(vao);
	}
};

