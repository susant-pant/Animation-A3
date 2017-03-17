#include "mass.h"

void Mass::addSpringForce(vec3 springForce) {
	netForce = springForce;
}

void Mass::updatePos() {
	if(!fixed){
		vec3 accel = (netForce/mass);
		velocity += (accel + vec3(0.f, -9.81f, 0.f))/60.f;
		pos += velocity/60.f;
	}
	if (pos.y < -3.95f) pos.y = -3.95f;
}
	/*
	– For each moving spring node -> Compute the internal force neighbor nodes are exerting on this node and add the force to this node’s force vector
	– For each moving spring node -> Compute the nodes acceleration (a=f/m) and add it to the spring’s velocity, and add the velocity to the spring’s position in space
	– For each moving spring node -> Compute the energy loss (friction)
	*/