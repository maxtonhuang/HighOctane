#include "Particles.h"

void ParticleManager::updateEmitter(Emitter& emitter, float deltaTime) {
	emitter.timeSinceLastEmission += deltaTime;
	while (emitter.timeSinceLastEmission >= 1.0f / emitter.emissionRate) {
		emitParticle(emitter);
		emitter.timeSinceLastEmission -= 1.0f / emitter.emissionRate;
	}
}

void ParticleManager::updateParticle(Particle& particle, float deltaTime) {
	particle.properties.velocity += particle.properties.acceleration * deltaTime;
	particle.properties.position += particle.properties.velocity * deltaTime;
	particle.lifetime -= deltaTime;
	particle.age += deltaTime;
}

void ParticleManager::emitParticle(const Emitter& emitter) {
	/*Particle newParticle = emitter.particlePrototype;
	newParticle.properties.position = emitter.position;
	particles.push_back(newParticle);*/
}