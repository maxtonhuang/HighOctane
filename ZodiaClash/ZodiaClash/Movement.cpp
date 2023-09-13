#include "Movement.h"
#include "Message.h"
#include "Enginecore.h"
#include "DebugDiagnostic.h"

extern Mail mail;
extern float dt;

//int keyInput;

int Movement()
{
	int keyInput = 11;
	
	for (Postcard msg : mail.mailbox[ADDRESS::MOVEMENT]) {
		if (msg.type == TYPE::KEY_DOWN) {
			
			if (msg.info == INFO::KEY_W || msg.info == INFO::KEY_UP) { keyInput += 10; }
			if (msg.info == INFO::KEY_S || msg.info == INFO::KEY_DOWN) { keyInput += -10; }
			if (msg.info == INFO::KEY_A || msg.info == INFO::KEY_LEFT) { keyInput += -1; }
			if (msg.info == INFO::KEY_D || msg.info == INFO::KEY_RIGHT) { keyInput += 1; }
		}
	}
	mail.mailbox[ADDRESS::MOVEMENT].clear();

	return keyInput;
}

void UpdateModel() {
	mail.CreatePostcard(TYPE::KEY_CHECK, ADDRESS::MOVEMENT, INFO::NONE);
	int direction = Movement();
	switch (direction) {
	case UP_DIRECTION:
		test_model.AddPos(0, 200 * dt);
		break;
	case UPRIGHT_DIRECTION:
		test_model.AddPos(200 * dt, 200 * dt);
		break;
	case RIGHT_DIRECTION:
		test_model.AddPos(200 * dt, 0);
		break;
	case DOWNRIGHT_DIRECTION:
		test_model.AddPos(200 * dt, -200 * dt);
		break;
	case DOWN_DIRECTION:
		test_model.AddPos(0, -200 * dt);
		break;
	case DOWNLEFT_DIRECTION:
		test_model.AddPos(-200 * dt, -200 * dt);
		break;
	case LEFT_DIRECTION:
		test_model.AddPos(-200 * dt, 0);
		break;
	case UPLEFT_DIRECTION:
		test_model.AddPos(-200 * dt, 200 * dt);
		break;
	case NEUTRAL_DIRECTION:
	default:
		break;
	}
}