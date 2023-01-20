#include "renderable.hpp"
int Renderable::rCount;

Renderable::Renderable() {

}

Renderable::Renderable(std::vector<float> vertices) {
	vCount = vertices.size() / 8;
	iCount = 0;

	/*
	unsigned ModelVao;
	glGenVertexArrays(1, &ModelVao);
	glBindVertexArray(ModelVao);
	unsigned ModelVbo;
	glGenBuffers(1, &ModelVbo);
	glBindBuffer(GL_ARRAY_BUFFER, ModelVbo);
	glBufferData(GL_ARRAY_BUFFER, LudiListVert.size() * sizeof(float), LudiListVert.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	*/

	
	glGenVertexArrays(1, &VAO);
	std::cout << "-Made an array-" << std::endl;
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	std::cout << "-Made a buffer-" << std::endl;

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Renderable:rCount++;
}
Renderable::~Renderable() {

	glDeleteBuffers(1, &VBO);
	std::cout << "-Deleted a buffer-" << std::endl;
	if (iCount > 0){
		glDeleteBuffers(1, &EBO);
		std::cout << "-Deleted a buffer for indexing-" << std::endl;
	}
	std::cout << "-Deleted an array-" << std::endl;
	glDeleteVertexArrays(1, &VAO);

	Renderable::rCount--;
}
void Renderable::Render(unsigned const &textura, unsigned const &specularMap) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textura);

	if (specularMap != NULL) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
	}

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vCount);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}