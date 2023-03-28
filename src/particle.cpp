#include "particle.hpp"
#include <memory>
#include <vector>
#include <iostream>
#define GLM_FORCE_RADIANS


namespace rocket {


	std::vector<RocketModel::Vertex> Particle::createParticleVerticies(float radius, glm::vec2 position)
	{
		std::vector<RocketModel::Vertex> vertices;
		// https://stackoverflow.com/questions/71398406/generate-the-vertices-of-a-circle-in-sdl2-c
		constexpr float segRotationAngle = (360.0 / VERTEX_COUNT) * (glm::pi<float>() / 180);

		float startX = position.x - radius;
		float startY = position.y;

		glm::vec3 color = { 1.0f, 1.0f, 1.0f };
		for (int i = 0; i < VERTEX_COUNT; i++) {
			float finalSegRotationAngle = segRotationAngle * i;
			float firstX = cos(finalSegRotationAngle) * startX - sin(finalSegRotationAngle) * startY;
			float firstY = cos(finalSegRotationAngle) * startY + sin(finalSegRotationAngle) * startX;

			finalSegRotationAngle = segRotationAngle * (i + 1);
			float secondX = cos(finalSegRotationAngle) * startX - sin(finalSegRotationAngle) * startY;
			float secondY = cos(finalSegRotationAngle) * startY + sin(finalSegRotationAngle) * startX;
			vertices.push_back({ { 0, 0}, color });
			vertices.push_back({ { firstX, firstY}, color });
			vertices.push_back({ { secondX, secondY}, color });
		}
		return vertices;
	}

	Particle::Particle()
	{
	}

	Particle::~Particle()
	{
	}

}
