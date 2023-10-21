#include "Movement.h"
#include "Message.h"
#include "Enginecore.h"
#include "DebugDiagnostic.h"
#include "ECS.h"
#include "Global.h"
#include "model.h"
#include "physics.h"
#include "Camera.h"

vmath::Vector2 currMousePos{ RESET_VEC2 };
vmath::Vector2 direction{ RESET_VEC2 };
vmath::Vector2 finalPos{ RESET_VEC2 };

constexpr float speed = 200.f;

void UpdateMovement(Transform & transform, Model & model) {	
	//Mail::mail().CreatePostcard(TYPE::KEY_CHECK, ADDRESS::MOVEMENT, INFO::NONE);
	transform.velocity = { RESET_VEC2 };
	for (Postcard const & msg : Mail::mail().mailbox[ADDRESS::MOVEMENT]) {
		switch (msg.type) {

		case TYPE::KEY_DOWN:
			
			switch (msg.info) {
			case INFO::KEY_W:   case INFO::KEY_UP:      direction = { RESET_VEC2 }; transform.velocity.y = speed * g_dt;    break;
			case INFO::KEY_S:   case INFO::KEY_DOWN:    direction = { RESET_VEC2 }; transform.velocity.y = -speed * g_dt;   break;
			case INFO::KEY_A:   case INFO::KEY_LEFT:    direction = { RESET_VEC2 }; transform.velocity.x = -speed * g_dt;   break;
			case INFO::KEY_D:   case INFO::KEY_RIGHT:   direction = { RESET_VEC2 }; transform.velocity.x = speed * g_dt;    break;
			case INFO::KEY_O:   transform.scale += 1.f * g_dt; break;
			case INFO::KEY_P:   transform.scale -= 1.f * g_dt; break;
			case INFO::KEY_Q:   transform.rotation -= 1.f * g_dt;   break;
			case INFO::KEY_E:   transform.rotation += 1.f * g_dt;   break;
			case INFO::KEY_1:   model.AddAlpha(0.2f * g_dt);        break;
			case INFO::KEY_2:   model.AddAlpha(-0.2f * g_dt);       break;
			case INFO::KEY_3:   camera.AddZoom(0.1f * g_dt);        break;
			case INFO::KEY_4:   camera.AddZoom(-0.1f * g_dt);       break;
			case INFO::KEY_I:   camera.AddPos(0.f, 200.f * g_dt);   break;
			case INFO::KEY_J:   camera.AddPos(-200.f * g_dt, 0.f);  break;
			case INFO::KEY_K:   camera.AddPos(0, -200.f * g_dt);    break;
			case INFO::KEY_L:   camera.AddPos(200.f * g_dt, 0.f);   break;
			default: break;
			}
			break;

		case TYPE::MOUSE_MOVE:
			currMousePos = { msg.posX, msg.posY };
			//transform.position = { msg.posX, msg.posY };
			break;

		case TYPE::MOUSE_CLICK:
			
			if (msg.info == INFO::MOUSE_LEFT && !hoveringPanel) {
				finalPos = currMousePos;
				transform.velocity = { RESET_VEC2 };
				direction = currMousePos - transform.position;
				direction = direction.normalize();
			}
			break;
			
		default:
			break;

		}
	}
	if (transform.scale < 0.f) {
		transform.scale = 0.f;
	}
	//transform.position += transform.velocity;
	//transform.velocity = {0,0};
	

	if (transform.position.distance(finalPos) > 2.f) {
		transform.velocity += direction * speed * g_dt;
	}

}
