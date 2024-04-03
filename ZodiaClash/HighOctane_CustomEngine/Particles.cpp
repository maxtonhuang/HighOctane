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
*	@file		Particles.cpp
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

#include "Particles.h"
#include "graphics.h"
#include "AssetManager.h"

ParticleManager particles;

ParticleManager::ParticleManager() : index{}, particleList{} {}


/**************************************************************************/
/*!
	@brief AddParticle Adds a new particle to the particle system with specified
            properties. If the system is full, it may overwrite existing particles
            or ignore the request, depending on implementation.
    @param fixed Determines whether the particle's position is fixed.
    @param position The starting position of the particle.
    @param size The size of the particle.
    @param velocity The velocity of the particle upon creation.
    @param color The color of the particle.
    @param update A function pointer to a custom update function for the particle.
    @param timer The initial timer value for the particle.
*/
/**************************************************************************/
Particle& ParticleManager::AddParticle( bool fixed, Vec2 position, Vec2 size, Vec2 velocity, Color color, void (*update)(Particle&),float rot,float rotSpeed, float timer) 
{
	Particle& out{ particleList[particles.index++] };
	out = Particle{ true, fixed, position, size, velocity, color, update, rot, rotSpeed,timer };
	index %= particleList.size();
	return out;
}

/**************************************************************************/
/*!
	@brief Update Updates the state of all active particles in the system over
            the given time step.
    @param dt The time step over which to update the particles.

*/
/**************************************************************************/
void ParticleManager::Update(float dt) 
{
	for (Particle& p : particleList) 
	{
		if (!p.active) continue;
		if (!p.fixed) p.velocity.y += dt;
		p.position += p.velocity * dt;
		p.rotation += p.rotationSpeed * dt;
		//if (p.position.x < 0.f - (float)(graphics.GetWindowWidth()) || p.position.x >(float)(2 * graphics.GetWindowWidth()) || 
		//	p.position.y < 0.f - (float)(graphics.GetWindowHeight()) || p.position.y >(float)(2 * graphics.GetWindowHeight())) 
		//{
		//	p.active = false;
		//	continue;
		//}
		if (p.Update)
			p.Update(p);
		else {
			p.Update = particlePresets::ParticleFade;
			p.timer -= dt;
			if (p.timer <= 0)
				p.active = false;
		}
	}
}

/**************************************************************************/
/*!
	@brief Draw Renders all active particles in the system.
    @param dt The time step over which to render the particles, can be used for
            frame rate independent effects like motion blur.

*/
/**************************************************************************/
void ParticleManager::Draw(int layer) 
{
	static Renderer* particleRenderer = &graphics.renderer["particle"];
	
	if (previousRenderer) {
		previousRenderer->Draw();
	}
	
	for (Particle& p : particleList) 
	{
		if (!p.active) continue;
		if (p.layer != layer) continue;
		Texture* particleTexture = p.texture;
		if (!particleTexture) continue;
		float textureID = p.textureID;
		//graphics.DrawPoint(p.position.x, p.position.y,
			//p.particleColor.color.r, p.particleColor.color.g, p.particleColor.color.b, p.particleColor.color.a);
		glm::vec2 convertedPosition{ p.position.x / GRAPHICS::w, p.position.y / GRAPHICS::h };
		glm::vec2 convertedSize{ p.size.x / GRAPHICS::w, p.size.y / GRAPHICS::h };
		glm::mat3 matrix = glm::mat3{ cos(p.rotation) * convertedSize.x ,-sin(p.rotation) * convertedSize.x,0,
		sin(p.rotation) * convertedSize.y , cos(p.rotation) * convertedSize.y,0,
		convertedPosition.x,convertedPosition.y,1 };
		
		//glm::vec2 convertedWidth{ p.size.x / GRAPHICS::w, 0.f };
		//glm::vec2 convertedHeight{ 0.f, p.size.y / GRAPHICS::h };

		glm::vec3 bottomleft3 = matrix * glm::vec3{ -1,-1,1 };
		glm::vec3 bottomright3 = matrix * glm::vec3{ 1,-1,1 };
		glm::vec3 topleft3 = matrix * glm::vec3{ -1,1,1 };
		glm::vec3 topright3 = matrix * glm::vec3{ 1,1,1 };
		glm::vec2 botleft = glm::vec2{ bottomleft3.x,bottomleft3.y };
		glm::vec2 botright = glm::vec2{ bottomright3.x,bottomright3.y };
		glm::vec2 topleft = glm::vec2{ topleft3.x,topleft3.y };
		glm::vec2 topright = glm::vec2{ topright3.x,topright3.y };

		particleRenderer->AddVertex(Vertex{ botleft, p.particleColor.color, particleTexture->GetTexCoords(0,0), textureID }); //bottom left
		particleRenderer->AddVertex(Vertex{ botright, p.particleColor.color, particleTexture->GetTexCoords(0,1), textureID }); //bottom right
		particleRenderer->AddVertex(Vertex{ topleft, p.particleColor.color, particleTexture->GetTexCoords(0,2), textureID }); //top left
		particleRenderer->AddVertex(Vertex{ topright, p.particleColor.color, particleTexture->GetTexCoords(0,3), textureID }); //top right
		particleRenderer->AddVertex(Vertex{ botright, p.particleColor.color, particleTexture->GetTexCoords(0,1), textureID }); //bottom right
		particleRenderer->AddVertex(Vertex{ topleft, p.particleColor.color, particleTexture->GetTexCoords(0,2), textureID }); //top left
		//graphics.DrawRect(p.position.x, p.position.y, p.position.x +( p.size.x / 2), p.position.y + (p.size.y / 2));
	}
	particleRenderer->Draw();
}

/**************************************************************************/
/*!
	@brief ResetParticles Resets all particles in the system to their initial,
            inactive state. This is typically used to clear the particle system
            or restart it.
*/
/**************************************************************************/
void ParticleManager::ResetParticles() 
{
	for (Particle& p : particleList)
		p.active = false;
}

/**************************************************************************/
/*!
    @brief Modifies the alpha component of a particle's color to implement
           fading over time, based on a predefined decay rate. This function
           decreases the particle's opacity, eventually deactivating it when
           its opacity falls below a minimal threshold.
    @param p A reference to the Particle instance to be modified. The function
             directly modifies the particle's color and active status.
*/
/**************************************************************************/
void particlePresets::ParticleFade(Particle& p) 
{
	p.particleColor.color.a *= (1.f / (1.f + (2 * FIXED_DT * p.fadeDecay) / 4));
	if (p.particleColor.color.a < FLT_EPSILON)
		p.active = false;
}

/**************************************************************************/
/*!
	@brief Scales down a particle's size over time, according to a predefined
           shrink decay rate. The function gradually decreases the particle's
           size and deactivates it if either dimension becomes negligibly small.

    @param p A reference to the Particle instance to be modified. The function
             affects the particle's size and active status.
*/
/**************************************************************************/
void particlePresets::ParticleShrink(Particle& p) 
{
	p.size = p.size * (1.f / (1.f + (FIXED_DT * p.shrinkDecay) / 4));
	if (p.size.x < FLT_EPSILON || p.size.y < FLT_EPSILON)
		p.active = false;
}
