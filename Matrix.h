#pragma once
#include <cmath>
#include <algorithm>
#include <GL/glew.h>

class Matrix
{
	GLfloat matrix[16];

public:
	Matrix() {}

	Matrix(const GLfloat* a)
	{
		std::copy(a, a + 16, matrix);
	}

	const GLfloat& operator[](std::size_t i) const
	{
		return matrix[i];
	}

	GLfloat& operator[](std::size_t i)
	{
		return matrix[i];
	}

	const GLfloat* data() const
	{
		return matrix;
	}

	void loadIdentity()
	{
		std::fill(matrix, matrix + 16, 0.f);
		matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.f;
	}

	static Matrix identity()
	{
		Matrix t;
		t.loadIdentity();

		return t;
	}

	static Matrix translate(GLfloat x, GLfloat y, GLfloat z)
	{
		Matrix t;
		t.loadIdentity();
		t[12] = x;
		t[13] = y;
		t[14] = z;

		return t;
	}

	static Matrix scale(GLfloat x, GLfloat y, GLfloat z)
	{
		Matrix t;
		t.loadIdentity();
		t[0] = x;
		t[5] = y;
		t[10] = z;

		return t;
	}

	// :D TODO // ™’’f‚ÌŠÖ”

	// :D TODO // À•WŽ²’†S‰ñ“]‚ÌŠÖ”

	static Matrix rotate(GLfloat a, GLfloat x, GLfloat y, GLfloat z)
	{
		Matrix t;
		const GLfloat d(sqrt(x * x + y * y + z * z));

		if (d > 0.f)
		{
			const GLfloat l(x / d), m(y / d), n(z / d);
			const GLfloat l2(l * l), m2(m * m), n2(n * n);
			const GLfloat lm(l * m), mn(m * n), nl(n * l);
			const GLfloat c(cos(a)), c1(1.f - c), s(sin(a));

			t.loadIdentity();
			t[0] = (1.f - l2) * c + l2;
			t[1] = lm * c1 + n * s;
			t[2] = nl * c1 - m * s;
			t[4] = lm * c1 - n * s;
			t[5] = (1.f - m2) * c + m2;
			t[6] = mn * c1 + l * s;
			t[8] = nl * c1 + m * s;
			t[9] = mn * c1 - l * s;
			t[10] = (1.f - n2) * c + n2;
		}

		return t;
	}

	//Matrix operator*(const Matrix& m) const
	//{
	//	Matrix t;
	//	for (int j = 0; j < 4; ++j)
	//	{
	//		for (int i = 0; i < 4; ++i)
	//		{
	//			const int ji(j * 4 + i);
	//			t[ji] = 0.f;
	//			for (int k = 0; k < 4; ++k)
	//				t[ji] += matrix[k * 4 + i] * m[j * 4 + k];
	//		}
	//	}

	//	return t;
	//}

	Matrix operator*(const Matrix& m) const
	{
		Matrix t;

		for (int i = 0; i < 16; ++i)
		{
			const int j(i & 3), k(i & ~3);

			t[i] = 
				matrix[0 + j] * m[k + 0] +
				matrix[4 + j] * m[k + 1] +
				matrix[8 + j] * m[k + 2] +
				matrix[12 + j] * m[k + 3];
		}

		return t;
	}
};
