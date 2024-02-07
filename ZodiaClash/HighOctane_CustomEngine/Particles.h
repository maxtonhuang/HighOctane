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
struct Particle {
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
	void AddParticle( bool fixed, Vec2 position, Vec2 size, Vec2 velocity, Color color, void (*update)(Particle&), float rot = 0.f, float rotSpeed = 0.f, float timer = 0.f );
	void Update(float dt);
	void Draw(float dt);
	void ResetParticles();
	size_t index;
	std::array<Particle, 10000> particleList;
	ParticleManager();
};

//TBD, right now used as a flag to create environmental particles in scene
class Emitter {

};

namespace particlePresets 
{
	void ParticleFade(Particle& p);
	void ParticleShrink(Particle& p);
}
extern ParticleManager particles;