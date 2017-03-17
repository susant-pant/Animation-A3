#include "mass.h"
#include <iostream>

void Mass::addSpringForce(vec3 springForce) {
	if(!fixed)
		Fspring += springForce;
}

void Mass::updatePos() {
	if(!fixed){
		vec3 Fgrav = vec3(-0.f, -9.81f, 0.f) * mass;
		vec3 Fdamp = -velocity * 0.25f;
		vec3 netForce = Fspring + Fgrav + Fdamp;

		std::cout << Fspring.y << std::endl;

		vec3 accel = (netForce/mass);
		velocity += accel/60.f;
		pos +=	 velocity/60.f;
	}
	if (pos.y <= -3.95f) velocity.y = -velocity.y;
}