#include "Movement.h"
#include "Message.h"
#include "Enginecore.h"
#include "DebugDiagnostic.h"
#include "ECS.h"
#include "Global.h"

void UpdateMovement(Transform & transform) {	
	//Mail::mail().CreatePostcard(TYPE::KEY_CHECK, ADDRESS::MOVEMENT, INFO::NONE);
		
	for (Postcard msg : Mail::mail().mailbox[ADDRESS::MOVEMENT]) {
		switch (msg.type) {
		case TYPE::KEY_DOWN:
			//Entity 
			//Transform tr = ecs.GetComponent<Transform>(entity);
			if (msg.info == INFO::KEY_W || msg.info == INFO::KEY_UP) { transform.velocity.y += 200.f * g_dt; }
			if (msg.info == INFO::KEY_S || msg.info == INFO::KEY_DOWN) { transform.velocity.y += -200.f * g_dt; }
			if (msg.info == INFO::KEY_A || msg.info == INFO::KEY_LEFT) { transform.velocity.x += -200.f * g_dt; }
			if (msg.info == INFO::KEY_D || msg.info == INFO::KEY_RIGHT) { transform.velocity.x += 200.f * g_dt; }

			//THE FOLLOWING FUNCTIONS NEED SOME KIND OF LIMIT CHECK
			if (msg.info == INFO::KEY_O) { transform.scale.x += 10.f * g_dt; transform.scale.y += 10.f * g_dt; }
			if (msg.info == INFO::KEY_P) { transform.scale.x -= 10.f * g_dt; transform.scale.y -= 10.f * g_dt; }
			if (msg.info == INFO::KEY_Q) { transform.rotation -= 1.f * g_dt; }
			if (msg.info == INFO::KEY_E) { transform.rotation += 1.f * g_dt; }
			break;
		case TYPE::MOUSE_MOVE:
			transform.position = { msg.posX, msg.posY };
		}

	}
	if (transform.scale.x < 0.f) {
		transform.scale.x = 0.f;
	}
	if (transform.scale.y < 0.f) {
		transform.scale.y = 0.f;
	}
	transform.position += transform.velocity;
	transform.velocity = {0,0};
	


}
