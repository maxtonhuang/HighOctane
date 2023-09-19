#include "Movement.h"
#include "Message.h"
#include "Enginecore.h"
#include "DebugDiagnostic.h"
#include "ECS.h"

extern Mail mail;
extern float g_dt;

//int keyInput;

void UpdateMovement(Transform & transform) {	
	mail.CreatePostcard(TYPE::KEY_CHECK, ADDRESS::MOVEMENT, INFO::NONE);
	
	//int keyInput = 11;
	
	for (Postcard msg : mail.mailbox[ADDRESS::MOVEMENT]) {
		if (msg.type == TYPE::KEY_DOWN) {
			
			if (msg.info == INFO::KEY_W || msg.info == INFO::KEY_UP) { transform.velocity.y += 200.f * g_dt; }
			if (msg.info == INFO::KEY_S || msg.info == INFO::KEY_DOWN) { transform.velocity.y += -200.f * g_dt; }
			if (msg.info == INFO::KEY_A || msg.info == INFO::KEY_LEFT) { transform.velocity.x += -200.f * g_dt; }
			if (msg.info == INFO::KEY_D || msg.info == INFO::KEY_RIGHT) { transform.velocity.x += 200.f * g_dt; }
		}
	}
	mail.mailbox[ADDRESS::MOVEMENT].clear();


}

void UpdateModel(Transform & transform) {
	
	transform.position += transform.velocity;






	
	
	//int direction = Movement();
	/*
	
	
	switch (direction) {
	case UP_DIRECTION:
		test_model.AddPos(0, 200 * g_dt);
		break;
	case UPRIGHT_DIRECTION:
		test_model.AddPos(200 * g_dt, 200 * g_dt);
		break;
	case RIGHT_DIRECTION:
		test_model.AddPos(200 * g_dt, 0);
		break;
	case DOWNRIGHT_DIRECTION:
		test_model.AddPos(200 * g_dt, -200 * g_dt);
		break;
	case DOWN_DIRECTION:
		test_model.AddPos(0, -200 * g_dt);
		break;
	case DOWNLEFT_DIRECTION:
		test_model.AddPos(-200 * g_dt, -200 * g_dt);
		break;
	case LEFT_DIRECTION:
		test_model.AddPos(-200 * g_dt, 0);
		break;
	case UPLEFT_DIRECTION:
		test_model.AddPos(-200 * g_dt, 200 * g_dt);
		break;
	case NEUTRAL_DIRECTION:
	default:
		break;
	}
	*/
}