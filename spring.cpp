#include <iostream>
#include "spring.h"

vec3 Spring::findSpringForce(){
	/*float springLength = sqrt(	(mass1->pos.x - mass2->pos.x)*(mass1->pos.x - mass2->pos.x) +
								(mass1->pos.y - mass2->pos.y)*(mass1->pos.y - mass2->pos.y) +
								(mass1->pos.z - mass2->pos.z)*(mass1->pos.z - mass2->pos.z));
	*/
	float springLength = mass1->pos.y - mass2->pos.y;
	std::cout << springLength << std::endl;
	vec3 springForce = -1 * (restLength - springLength) * vec3(0.f, 1.f, 0.f);
	std::cout << springForce.x << "," << springForce.y << "," << springForce.z << std::endl;

	//if (!mass1->fixed && !mass2->fixed) springForce /= 2.f;

	return springForce;
}