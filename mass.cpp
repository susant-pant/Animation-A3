#include "mass.h"
#include <iostream>
#include <cstdlib>

float Mass::findDistTo(Mass otherMass){
	return length(vec3(pos - otherMass.pos));
}

void Mass::updatePos(float time) {
	vec3 Fgrav = vec3(0.f, -9.81f, 0.f) * mass;
	vec3 Fdamp = -velocity * 0.25f;
	vec3 Fwind = vec3(0.f);

	if(isCloth){
		float windIntensity = sin(time) + 0.25f * sin(rand());
		float wind = (windIntensity <= 0.f) ? 20.f * (windIntensity - 1.f) : 0.f;
		float windPosX = 2.f * tan(time + 1.4f) + 0.25f*tan(rand());
		if (pos.x >= (windPosX - 0.5f) && pos.x <= windPosX + 0.5f) {
			Fwind = vec3(0.f, 0.f, -wind) * mass;
		}
	}

	vec3 netForce = Fspring + Fgrav + Fdamp + Fwind;

	vec3 accel = (netForce/mass);
	velocity += accel/120.f;
	pos +=	 velocity/120.f;
	
	if (pos.y < -3.95f) {
		pos.y = -3.95f;
		if (!isCloth) velocity.y = -velocity.y * 0.85f;
		else velocity = vec3(0.f);
	}
}