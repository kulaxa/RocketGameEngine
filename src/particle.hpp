#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "rocket_model.hpp"
#include "rocket_game_object.hpp"

namespace rocket {
	class Particle {
	static const int MAX_PARTICLES = 1000;
	static const int VERTEX_COUNT = 100;
	public:
		static std::vector<RocketModel::Vertex> createParticleVerticies(float radius, glm::vec2 position);
		
		Particle();
		~Particle();


		Particle(const Particle&) = delete;
		void operator=(const Particle&) = delete;
		
	private:
		float radius;
	};
}