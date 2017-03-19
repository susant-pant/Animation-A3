#include "glm/glm.hpp"

using namespace glm;

class Mass{
public:
	float mass;
	vec3 pos;
	vec3 velocity;
	vec3 Fspring;
	bool fixed;
	bool bounces;

	Mass(float _mass, vec3 _pos, bool _fixed): mass(_mass), pos(_pos), velocity(vec3(0.f, 0.f, 0.f)), Fspring(vec3(0.f, 0.f, 0.f)), fixed(_fixed), bounces(true){};

	Mass(float _mass, vec3 _pos, bool _fixed, bool _bounces): mass(_mass), pos(_pos), velocity(vec3(0.f, 0.f, 0.f)), Fspring(vec3(0.f, 0.f, 0.f)), fixed(_fixed), bounces(_bounces){};

	float findDistTo(Mass otherMass);
	void updatePos();
};