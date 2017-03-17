#include <iostream>
#include <glm/glm.hpp>
#include "spring.h"

vec3 Spring::findSpringForce(){
	float springDisp = length(vec3(mass2->pos - mass1->pos));
	vec3 springForce = -1 * stiffness * (restLength - springDisp) * normalize(vec3(mass1->pos - mass2->pos));
	return springForce;
}