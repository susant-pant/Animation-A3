#include <iostream>
#include <glm/glm.hpp>
#include "spring.h"

vec3 Spring::findSpringForce(){
	float springDist = length(vec3(mass2->pos - mass1->pos));
	vec3 springForce = -1 * stiffness * (springDist - restLength) * normalize(vec3(mass2->pos - mass1->pos));

	if (!mass1->fixed && !mass2->fixed)	springForce /= 2.f;

	if (springDist <= 0.05f){
		if (!mass1->isCloth) mass1->velocity = -1.f * mass1->velocity * 0.45f;
		if (!mass2->isCloth) mass2->velocity = -1.f * mass2->velocity * 0.45f;
	}
	
	return springForce;
}