#include "glm/glm.hpp"

using namespace glm;

class Mass{
public:
	float mass;
	vec3 pos;
	vec3 velocity;
	vec3 netForce;
	bool fixed;

	Mass(float _mass, vec3 _pos, bool _fixed): mass(_mass), pos(_pos), velocity(vec3(0.f, 0.f, 0.f)), netForce(vec3(0.f, _mass * -9.81f, 0.f)), fixed(_fixed){};

	void addSpringForce(vec3 springForce);
	void updatePos();
};