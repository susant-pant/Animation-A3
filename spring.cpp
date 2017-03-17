#include <iostream>
#include <glm/glm.hpp>
#include "spring.h"

vec3 Spring::findSpringForce(){
	float springDisp = length(vec3(mass2->pos - mass1->pos));
	vec3 springForce = -1 * stiffness * (springDisp - restLength) * normalize(vec3(mass2->pos - mass1->pos));

	if(!mass1->fixed && !mass2->fixed) springForce /= 2.f;
	
	return springForce;
}