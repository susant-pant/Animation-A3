#include "mass.h"

void Mass::addSpringForce(vec3 springForce) {
	netForce += springForce * mass;
}

void Mass::updatePos() {
	if(!fixed){
		vec3 accel = (netForce/mass);
		velocity += accel/100000.f;
		pos += velocity/100000.f;
	}
}
	/*
	– For each moving spring node -> Compute the internal force neighbor nodes are exerting on this node and add the force to this node’s force vector
	– For each moving spring node -> Compute the nodes acceleration (a=f/m) and add it to the spring’s velocity, and add the velocity to the spring’s position in space
	– For each moving spring node -> Compute the energy loss (friction)
	*/