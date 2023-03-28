#include "physics_system.hpp"
#include <iostream>

namespace rocket {
	PhysicsSystem::PhysicsSystem(glm::vec2 gravity): gravity(gravity)
	{

	}

	PhysicsSystem::~PhysicsSystem()
	{
	}

	void PhysicsSystem::updatePhysics(float deltaTime, std::vector<RocketGameObject>& gameObjects)
	{
		for (auto& gameObject : gameObjects) {
			if (gameObject.gravityApplied && gameObject.total_force.y/ gameObject.mass < gravity.y) {
				gameObject.total_force += gameObject.mass * gravity * deltaTime;
			}
			if (gameObject.collisionApplied && gameObject.type == RocketGameObjectType::PARTICLE) 
			{
				resolveCollisionWithOuterWalls(gameObject, deltaTime);
				resolveCollisionWithOtherParticles(gameObject, gameObjects, deltaTime);
			}
			//std::cout << "total force: " << gameObject.total_force.x << " " << gameObject.total_force.y << std::endl;
			gameObject.velocity +=  gameObject.total_force / gameObject.mass * deltaTime;
			//std::cout << "velocity: " << gameObject.velocity.x << " " << gameObject.velocity.y << std::endl;
			gameObject.transform2d.translation += gameObject.velocity * deltaTime;
			//gameObject.gravityApplied = true;
	
		}
	}

	void PhysicsSystem::resolveCollisionWithOuterWalls(RocketGameObject& gameObject, float deltaTime)
	{
		float stillForceTreshold = gameObject.mass;
		float radius =  gameObject.radius;
		glm::vec2 center = gameObject.transform2d.translation;
		const float COLLISION_ENERGY_LOSS = 0.70f;
		const float FRICTION = 0.7f;

		 if (glm::abs(center.x) + radius >= 1.0f) {
			//gameObject.velocity.x *= -COLLISION_ENERGY_LOSS;
			float distance = glm::abs(gameObject.transform2d.translation.x) + radius - 1.0f;
			if (center.x < 0.0f) {
				gameObject.transform2d.translation.x += distance;
			}
			else {
				gameObject.transform2d.translation.x -= distance;
			}
		}

		 if (glm::abs(center.y) + radius >= 1.0f) {
			 gameObject.velocity.y *= -COLLISION_ENERGY_LOSS;
			 float distance = glm::abs(gameObject.transform2d.translation.y) + radius - 1.0f;
			 if (center.y < 0.0f) {
				 gameObject.transform2d.translation.y +=  distance;
			 }
			 else {
				gameObject.transform2d.translation.y -=  distance;
			 }
			 if (glm::abs(gameObject.velocity.y) < 0.01f) {
				 gameObject.gravityApplied = false;
				 gameObject.velocity = { 0.0f , 0.0f };
				 gameObject.total_force.y = 0.0f;
			 }
			 else {
				 gameObject.gravityApplied  = true;
			 }
		}
		
	}

	void PhysicsSystem::resolveCollisionWithOtherParticles(RocketGameObject& gameObject, std::vector<RocketGameObject>& gameObjects, float deltaTime)
	{
		float radius = gameObject.radius;
		glm::vec2 center = gameObject.transform2d.translation;
		float COLLISION_ENERGY_LOSS = 0.9f;
		for (auto& particle : gameObjects) {
			if (particle.getId() != gameObject.getId()) {
				{
					float otherRadius = particle.radius;
					glm::vec2 otherCenter = particle.transform2d.translation;
					if (glm::distance(center, otherCenter) <= radius + otherRadius) {

						float distance = glm::distance(center, otherCenter);
						float overlap = (distance - (radius + otherRadius));
						glm::vec2 v1 = gameObject.velocity;
						glm::vec2 v2 = particle.velocity;
						float m1 = gameObject.mass;
						float m2 = particle.mass;
						if (!gameObject.gravityApplied && particle.gravityApplied) {
							particle.transform2d.translation += overlap * (center - otherCenter) / distance;
							particle.velocity.y *= -COLLISION_ENERGY_LOSS;
						}
						else if (!particle.gravityApplied && gameObject.gravityApplied) {
							gameObject.transform2d.translation -= overlap * (center - otherCenter) / distance;
							gameObject.velocity.y *= -COLLISION_ENERGY_LOSS;
						}
						else if (gameObject.gravityApplied && particle.gravityApplied) {
							gameObject.transform2d.translation -= 0.5f * overlap * (center - otherCenter) / distance;
							gameObject.velocity = (v1 - (2.0f * m2 / (m1 + m2)) * glm::dot(v1 - v2, center - otherCenter) / glm::dot(center - otherCenter, center - otherCenter) * (center - otherCenter));
							gameObject.velocity *= COLLISION_ENERGY_LOSS;

							particle.transform2d.translation += 0.5f * overlap * (center - otherCenter) / distance;
							particle.velocity = (v2 - (2.0f * m1 / (m1 + m2)) * glm::dot(v2 - v1, otherCenter - center) / glm::dot(otherCenter - center, otherCenter - center) * (otherCenter - center));
							particle.velocity *= COLLISION_ENERGY_LOSS;
						}
						else {
							gameObject.transform2d.translation -= 0.5f * overlap * (center - otherCenter) / distance;
							gameObject.velocity = (v1 - (2.0f * m2 / (m1 + m2)) * glm::dot(v1 - v2, center - otherCenter) / glm::dot(center - otherCenter, center - otherCenter) * (center - otherCenter));
							gameObject.velocity *= COLLISION_ENERGY_LOSS;

							particle.transform2d.translation += 0.5f * overlap * (center - otherCenter) / distance;
							particle.velocity = (v2 - (2.0f * m1 / (m1 + m2)) * glm::dot(v2 - v1, otherCenter - center) / glm::dot(otherCenter - center, otherCenter - center) * (otherCenter - center));
							particle.velocity *= COLLISION_ENERGY_LOSS;
						}
						if (!gameObject.gravityApplied || !particle.gravityApplied) {
							if (glm::abs(gameObject.velocity.y) < 0.001f) {
								gameObject.gravityApplied = false;
								gameObject.velocity = { 0.0f , 0.0f };
								gameObject.total_force.y = 0.0f;

							}
							else {
								gameObject.gravityApplied = true;
							}
							if (glm::abs(particle.velocity.y) < 0.001f) {
								particle.gravityApplied = false;
								particle.velocity = { 0.0f , 0.0f };
								particle.total_force.y = 0.0f;
							}
							else {
								particle.gravityApplied = true;
							}
						}

						return;
					}
				}
			}
		}
		
	}





}
