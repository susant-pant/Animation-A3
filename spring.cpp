#include "spring.h"

vec3 Spring::findSpringForce(){
	springLength = sqrt((mass1.pos.x - mass2.pos.x)*(mass1.pos.x - mass2.pos.x) +
						(mass1.pos.y - mass2.pos.y)*(mass1.pos.y - mass2.pos.y) +
						(mass1.pos.z - mass2.pos.z)*(mass1.pos.z - mass2.pos.z));
	vec3 springForce = -stiffness * (springLength - restLength);

	if (!mass1.fixed && !mass2.fixed) springForce /= 2.f;

	return springForce;
}