#pragma once
\
#include "rocket_model.hpp"
#include <memory>	

namespace rocket {
	struct Transform2dComponent {
		glm::vec2 translation{};
		glm::vec2 scale{ 1.f, 1.f };
		float rotation;
		glm::mat2 mat2() {
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 scaleMat{
				{scale.x, 0.0f},
				{ 0.0f, scale.y}
			};
			glm::mat2 rotationMat{
				{c, s},
				{-s, c}
			};
			return rotationMat * scaleMat;
		};

	};

	class RocketGameObject {
	public:
		using id_t = unsigned int;

		static RocketGameObject createGameObject() {
			static id_t currentId = 0;
			return RocketGameObject{ currentId++ };

		}

		RocketGameObject(const RocketGameObject&) = delete;
		RocketGameObject& operator=(const RocketGameObject&) = delete;
		RocketGameObject(RocketGameObject&&) = default;
		RocketGameObject& operator=(RocketGameObject&&) = default;

		id_t getId() const { return id; }

		std::shared_ptr<RocketModel> model{};
		glm::vec3 color{};
		Transform2dComponent transform2d{};

	private:
		RocketGameObject(id_t objId) : id{ objId } {}

		id_t id;
	
	};
}