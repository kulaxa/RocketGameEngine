#pragma once
#include <vector>
#include <memory>
#include "particle.hpp"

namespace rocket {

	class PhysicsSystem {
	public:
		PhysicsSystem(glm::vec2 gravity);
		~PhysicsSystem();

		PhysicsSystem(const PhysicsSystem&) = delete;
		void operator=(const PhysicsSystem&) = delete;

		void updatePhysics(float deltaTime, std::vector<RocketGameObject>& gameObjects);

		void resolveCollisionWithOuterWalls(RocketGameObject& gameObject, float deltaTime);

		void resolveCollisionWithOtherParticles(RocketGameObject& gameObject, std::vector<RocketGameObject>& gameObjects, float deltaTime);

		glm::vec2 getGravity() { return gravity; }
	private:
		glm::vec2 gravity;
	};
}