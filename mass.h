#include "glm/glm.hpp"

using namespace glm;

class Mass{
public:
	float mass;
	vec3 pos;
	vec3 velocity;
	vec3 Fspring;
	bool fixed;
	bool isCloth;

	Mass(float _mass, vec3 _pos, bool _fixed): mass(_mass), pos(_pos), velocity(vec3(0.f, 0.f, 0.f)), Fspring(vec3(0.f, 0.f, 0.f)), fixed(_fixed), isCloth(false){};

	Mass(float _mass, vec3 _pos, bool _fixed, bool _isCloth): mass(_mass), pos(_pos), velocity(vec3(0.f, 0.f, 0.f)), Fspring(vec3(0.f, 0.f, 0.f)), fixed(_fixed), isCloth(_isCloth){};

	float findDistTo(Mass otherMass);
	void updatePos();
};