#include "Movement.h"
#include "Message.h"
#include "Enginecore.h"
#include "DebugDiagnostic.h"

extern Mail mail;
extern float dt;

//int keyInput;

void Movement()
{
	for (Postcard msg : mail.mailbox[ADDRESS::MOVEMENT]) {
		if (msg.type == TYPE::KEY_DOWN) {
			if (msg.info == INFO::KEY_W || msg.info == INFO::KEY_UP) { 
				test_model.AddPos(0, 200 * dt);
			}
			if (msg.info == INFO::KEY_S || msg.info == INFO::KEY_DOWN) { 
				test_model.AddPos(0, -200 * dt);
			}
			if (msg.info == INFO::KEY_A || msg.info == INFO::KEY_LEFT) { 
				test_model.AddPos(-200 * dt, 0);
			}
			if (msg.info == INFO::KEY_D || msg.info == INFO::KEY_RIGHT) { 
				test_model.AddPos(200 * dt, 0);
			}
			if (msg.info == INFO::KEY_E) {
				test_model.AddRot(30 * dt);
			}
			if (msg.info == INFO::KEY_Q) {
				test_model.AddRot(-30 * dt);
			}
		}
	}
	mail.mailbox[ADDRESS::MOVEMENT].clear();
}

void UpdateModel() {
	mail.CreatePostcard(TYPE::KEY_CHECK, ADDRESS::MOVEMENT, INFO::NONE);
	Movement();
	return;
}