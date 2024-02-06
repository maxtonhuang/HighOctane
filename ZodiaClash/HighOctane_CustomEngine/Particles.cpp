#include "Particles.h"
#include "graphics.h"

ParticleManager particles;

ParticleManager::ParticleManager() : index{}, particleList{} {}

void ParticleManager::AddParticle( bool fixed, Vec2 position, Vec2 size, Vec2 velocity, Color color, void (*update)(Particle&), float timer) 
{
	particleList[particles.index++] = Particle{ true, fixed, position, size, velocity, color, update, timer };
	index %= particleList.size();
}

void ParticleManager::Update(float dt) 
{
	for (Particle& p : particleList) 
	{
		if (!p.active) continue;
		if (!p.fixed) p.velocity.y += dt;
		p.position += p.velocity * dt;
		if (p.position.x < 0.f - (float)(graphics.GetWindowWidth()) || p.position.x >(float)(2 * graphics.GetWindowWidth()) || 
			p.position.y < 0.f - (float)(graphics.GetWindowHeight()) || p.position.y >(float)(2 * graphics.GetWindowHeight())) 
		{
			p.active = false;
			continue;
		}
		p.Update(p);
	}
}

void ParticleManager::Draw(float dt) 
{
	for (Particle& p : particleList) 
	{
		if (!p.active) continue;
		graphics.DrawPoint(p.position.x, p.position.y);
	}
}

void ParticleManager::ResetParticles() 
{
	for (Particle& p : particleList)
		p.active = false;
}

void particlePresets::ParticleFade(Particle& p) 
{
	p.particleColor.color.a *= (1.f / (1.f + (FIXED_DT * p.fadeDecay)));
	if (p.particleColor.color.a < FLT_EPSILON)
		p.active = false;
}

void particlePresets::ParticleShrink(Particle& p) 
{
	p.size = p.size * (1.f / (1.f + (FIXED_DT * p.shrinkDecay)));
	if (p.size.x < FLT_EPSILON || p.size.y < FLT_EPSILON)
		p.active = false;
}