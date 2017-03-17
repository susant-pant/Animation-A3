#include "glm/glm.hpp"
#include "mass.h"

using namespace glm;

class Spring{
public:
	Mass *mass1, *mass2;
	float stiffness;
	float restLength;
	float damping;

	Spring(Mass* _mass1, Mass* _mass2, float _stiffness, float _restLength) :
		mass1(_mass1), mass2(_mass2), stiffness(_stiffness), restLength(_restLength), damping(0.f){};

	Spring(Mass* _mass1, Mass* _mass2, float _stiffness, float _restLength, float _damping) :
		mass1(_mass1), mass2(_mass2), stiffness(_stiffness), restLength(_restLength), damping(_damping){};

	vec3 findSpringForce();
};