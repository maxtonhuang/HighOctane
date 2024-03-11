/******************************************************************************
*
*	\copyright
*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
*		All rights reserved. Reproduction or disclosure of this file or its
*		contents without the prior written consent of DigiPen Institute of
*		Technology is prohibited.
*
* *****************************************************************************
*
*	@file		Particles.h
*
*	@author		Liu Wan Ting
*
*	@email		wanting.liu@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		3 February 2024
*
* *****************************************************************************
*
*	@brief		Particles manager 
*
*	This file contains functions used in the particles manager
*
******************************************************************************/

#pragma once
#include "math.h"
#include "ECS.h"
#include "vector"

/**************************************************************************/
/*!
	@struct Particle
	@brief  Represents a single particle in a particle system. This structure
			holds all necessary information for representing and updating
			a particle's state over time. It includes properties for activity
			state, position, size, velocity, color, and lifetimes. It also
			supports customizable update behavior via a function pointer.
*/
/**************************************************************************/
struct Particle 
{
	bool                    active{};
	bool                    fixed{};
	Vec2                    position;
	Vec2                    size;
	Vec2                    velocity;
	Color                   particleColor;
	float					rotation{ 0.f };
	float					rotationSpeed{ 0.f };
	float                   timer{ 0.f }; //how long the particle has lived
	float                   fadeDecay{ 2.f };
	float                   shrinkDecay{ 2.f };
	void					(*Update)(Particle&) {};
	Texture*				texture;
	float					textureID;

	Particle() : active{ false }, fixed{ false }, position{}, size{}, velocity{}, particleColor{} {};
	Particle(bool isActive, bool isFixed, Vec2 pos, Vec2 size, Vec2 vel, Color clr, void (*update)(Particle&), float rot = 0.f, float rotSpeed = 0.f, float timer = 0.f)
		: active{ isActive }, fixed{ isFixed }, position{ pos }, size{ size }, velocity{ vel }, particleColor{ clr }, Update{ update }, timer{ timer }, rotation{ rot }, rotationSpeed{rotSpeed} {}
};

/**************************************************************************/
/*!
	@class ParticleManager
	@brief Manages a collection of particles, handling their creation, update,
		   and rendering. It maintains a fixed-size array of Particle instances
		   and provides methods for adding new particles, updating existing ones,
		   drawing them to the screen, and resetting the particle system to its
		   initial state.
*/
/**************************************************************************/
class ParticleManager 
{
public:
	Particle& AddParticle( bool fixed, Vec2 position, Vec2 size, Vec2 velocity, Color color, void (*update)(Particle&), float rot = 0.f, float rotSpeed = 0.f, float timer = 0.f );
	void Update(float dt);
	void Draw(float dt);
	void ResetParticles();
	size_t index;
	std::array<Particle, 10000> particleList;
	ParticleManager();
};

namespace particlePresets 
{
	void ParticleFade(Particle& p);
	void ParticleShrink(Particle& p);
}
extern ParticleManager particles;

struct Emitter
{
	Vec2                    position;
	Vec2                    size;
	Vec2                    velocity;
	bool					singleSided{ false }; //does particle go one way or two ways
	Color                   particleColor;
	int						particlesRate = 0;
	float					rotation{ 0.f };
	float					rotationSpeed{ 0.f };
	float                   fadeDecay{ 2.f };
	float                   shrinkDecay{ 2.f };
	float					particleLifetime{ 0.f };
	float					emitterLifetime{ 0.f };
	float					frequency{ 0.f }; //how many seconds has passed before you spawn a particle
	void					(*Update)(Emitter&) {};
	std::vector<std::string> textures;
	
	Emitter(Vec2 pos = Vec2{}, Vec2 sz = Vec2{}, Vec2 vel = Vec2{}, Color clr = Color{}, 
			int particlesToSpawn = 0, float rot = 0.f, float rotSpeed = 0.f, float ptcllifetime = 0.f, float lifeTime = 0.f, float freq = 0.f, void (*update)(Emitter&) = nullptr)
			: 
				position(pos), size(sz), velocity(vel), particleColor(clr), particlesRate(particlesToSpawn),
				rotation(rot), rotationSpeed(rotSpeed), particleLifetime(ptcllifetime), emitterLifetime(lifeTime), frequency(freq), Update(update) {}
};

class EmitterManager {
public:
	std::array<Particle, 10000> particleList;
	size_t index = 0; // Keep track of the next free spot in the array
	EmitterManager() : index(0) {}

	void AddParticle(bool fixed, Vec2 position, Vec2 size, Vec2 velocity, Color color, void (*update)(Particle&), float rotation = 0.f, float rotationSpeed = 0.f, float timer = 0.f) 
	{
		if (index >= particleList.size()) return;

		// Construct a new Particle and place it in the particleList
		particleList[index] = Particle(true, fixed, position, size, velocity, color, update, rotation, rotationSpeed, timer);
		++index; // Increment the index to the next free spot
	}

	// Optional: Implement a method to reset or clear particles, adjust the index, etc.
	void ResetParticles() 
	{
		index = 0; // Reset the index, effectively clearing the particles
	}
};
