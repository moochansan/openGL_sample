#pragma once
#include "Shape.h"

class SolidShape : public Shape
{
public:
	SolidShape(GLint size, GLsizei vertexcount, const Object::Vertex* vertex)
	 : Shape(size, vertexcount, vertex)
	{}

	virtual void execute() const
	{
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}

};
