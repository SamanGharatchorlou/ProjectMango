#pragma once

struct CameraShake
{
	float x = 0;
	float speed = 2.0f;
	float magnitude = 0.5f;
	
	VectorF direction;
	
	VectorF maxTrauma;
	VectorF trauma;

	void Update(float dt);
};

// see different types here
// https://www.davetech.co.uk/gamedevscreenshake