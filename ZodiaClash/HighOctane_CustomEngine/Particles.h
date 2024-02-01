#pragma once
#include "ECS.h"

class ParticleManager{
public:

private:
    void updateEmitter(Emitter& emitter, float deltaTime);
    void updateParticle(Particle& particle, float deltaTime);
    void emitParticle(const Emitter& emitter);

    std::list<Particle> particles;
    std::list<Emitter> emitters;
};
extern ParticleManager particle;