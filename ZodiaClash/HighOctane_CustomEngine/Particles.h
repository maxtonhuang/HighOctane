#pragma once
#include "math.h"
#include "ECS.h"
#include "vector"

struct Particle {
	bool                    active{};
	bool                    fixed{};
	Vec2                    position;
	Vec2                    size;
	Vec2                    velocity;
	Color                   particleColor;
	float                   timer{ 0.f }; //how long the particle has lived
	float                   fadeDecay{ 2.f };
	float                   shrinkDecay{ 2.f };
	void					(*Update)(Particle&) {};

	Particle() : active{ false }, fixed{ false }, position{}, size{}, velocity{}, particleColor{} {};
	Particle(bool isActive, bool isFixed, Vec2 pos, Vec2 size, Vec2 vel, Color clr, void (*update)(Particle&), float timer = 0.f)
		: active{ isActive }, fixed{ isFixed }, position{ pos }, size{ size }, velocity{ vel }, particleColor{ clr }, Update{ update }, timer{ timer } {}
};

class ParticleManager 
{
public:
	void AddParticle( bool fixed, Vec2 position, Vec2 size, Vec2 velocity, Color color, void (*update)(Particle&), float timer = 0.f );
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