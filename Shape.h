#pragma once
#include <memory>
#include "Object.h"

class Shape
{
	std::shared_ptr<const Object> object;

protected:
	const GLsizei vertexCount;

public:
	Shape(GLint size, GLsizei vertexcount, const Object::Vertex* vertex)
	 : object(new Object(size, vertexcount, vertex)),
	   vertexCount(vertexcount)
	{}

	void draw() const
	{
		object->bind();
		execute();
	}

	virtual void execute() const
	{
		glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
	}

};

