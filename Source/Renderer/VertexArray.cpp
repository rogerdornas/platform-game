#include "VertexArray.h"
#include <GL/glew.h>

VertexArray::VertexArray(const float* verts, unsigned int numVerts, const unsigned int* indices,
						 unsigned int numIndices)
: mNumVerts(numVerts)
, mNumIndices(numIndices)
, mVertexBuffer(0)
, mIndexBuffer(0)
, mVertexArray(0)
{
	// Create vertex array
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);

	// Create vertex buffer
	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, numVerts * 4 * sizeof(verts), verts, GL_STATIC_DRAW);

    // Create index buffer
	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint), indices, GL_STATIC_DRAW);

	// Specify the vertex attributes (For now, assume one vertex format) Position is 2 floats
	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // position

    // Texture coordinates is 2 floats
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // texCoord
}

VertexArray::~VertexArray()
{
	glDeleteBuffers(1, &mVertexBuffer);
	glDeleteBuffers(1, &mIndexBuffer);
	glDeleteVertexArrays(1, &mVertexArray);
}

void VertexArray::SetActive() const
{
	glBindVertexArray(mVertexArray);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
}
