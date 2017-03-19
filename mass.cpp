#include "mass.h"
#include <iostream>

float Mass::findDistTo(Mass otherMass){
	return length(vec3(pos - otherMass.pos));
}

void Mass::updatePos() {
	vec3 Fgrav = vec3(-0.f, -9.81f, 0.f) * mass;
	vec3 Fdamp = -velocity * 0.25f;
	vec3 netForce = Fspring + Fgrav + Fdamp;

	vec3 accel = (netForce/mass);
	velocity += accel/60.f;
	pos +=	 velocity/60.f;
	
	if (pos.y <= -3.95f){
		velocity.y = -velocity.y * 0.85f;
		pos.y = -3.95f;
	}
}