#include "ECS.h"
#include "Movement.h"


namespace Architecture {

	extern ECS ecs;

	void PhysicsSystem::Update(float g_dt) {
		for (Entity const & entity : m_Entities) {
			Vel movement = ecs.GetComponent<Vel>(entity);
			Transform transform = ecs.GetComponent<Transform>(entity);
		
			UpdateMovement(movement);
			UpdateModel(transform, movement);
		
		}
	}
	
	
}

