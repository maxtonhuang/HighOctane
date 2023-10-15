#include "Movement.h"
#include "Message.h"
#include "Enginecore.h"
#include "DebugDiagnostic.h"
#include "ECS.h"
#include "Global.h"
#include "model.h"
#include "physics.h"
#include "Camera.h"

vmath::Vector2 currMousePos{ 0.f, 0.f };
vmath::Vector2 direction{ 0.f, 0.f };
vmath::Vector2 finalPos{ 0.f, 0.f };

void UpdateMovement(Transform& transform, Model & model) {	
	//Mail::mail().CreatePostcard(TYPE::KEY_CHECK, ADDRESS::MOVEMENT, INFO::NONE);
		
	for (Postcard const & msg : Mail::mail().mailbox[ADDRESS::MOVEMENT]) {
		switch (msg.type) {

		case TYPE::KEY_DOWN:
			
			switch (msg.info) {
			case INFO::KEY_W:   case INFO::KEY_UP:      direction = { 0.f, 0.f }; transform.velocity.y = 200.f * g_dt;    break;
			case INFO::KEY_S:   case INFO::KEY_DOWN:    direction = { 0.f, 0.f }; transform.velocity.y = -200.f * g_dt;   break;
			case INFO::KEY_A:   case INFO::KEY_LEFT:    direction = { 0.f, 0.f }; transform.velocity.x = -200.f * g_dt;   break;
			case INFO::KEY_D:   case INFO::KEY_RIGHT:   direction = { 0.f, 0.f }; transform.velocity.x = 200.f * g_dt;    break;
			case INFO::KEY_O:   transform.scale.x += 10.f * g_dt; transform.scale.y += 10.f * g_dt; break;
			case INFO::KEY_P:   transform.scale.x -= 10.f * g_dt; transform.scale.y -= 10.f * g_dt; break;
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
			
			if (msg.info == INFO::MOUSE_LEFT) {
				finalPos = currMousePos;
				transform.velocity = { 0.f, 0.f };
				direction = currMousePos - transform.position;
				direction = direction.normalize();
				
			}

			break;
			
		default:
			break;

		}
	}
	if (transform.scale.x < 0.f) {
		transform.scale.x = 0.f;
	}
	if (transform.scale.y < 0.f) {
		transform.scale.y = 0.f;
	}
	//transform.position += transform.velocity;
	//transform.velocity = {0,0};

	if (transform.position.distance(finalPos) > 1) {
		transform.velocity += direction * 200.f * g_dt;
	}

}
