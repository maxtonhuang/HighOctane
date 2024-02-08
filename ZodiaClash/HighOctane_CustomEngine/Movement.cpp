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
*	@file		Movement.cpp
*
*	@author		Maxton Huang Xinghua
*
*	@email		m.huang\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		22 September 2023
*
* *****************************************************************************
*
*	@brief		Controls Main Character Movement in Exploration Scene
*
*	This file contains the function to control and update the movement of the
*	main character in the exploration scene.
* 
******************************************************************************/

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
	UNREFERENCED_PARAMETER(model);
	transform.force = { RESET_VEC2 };
	for (Postcard const & msg : Mail::mail().mailbox[ADDRESS::MOVEMENT]) {
		switch (msg.type) {

		case TYPE::KEY_DOWN:
			
			switch (msg.info) {
			case INFO::KEY_W:   case INFO::KEY_UP:      direction = { RESET_VEC2 }; transform.force.y = 2000; break;
			case INFO::KEY_S:   case INFO::KEY_DOWN:    direction = { RESET_VEC2 }; transform.force.y = -2000; break;
			case INFO::KEY_A:   case INFO::KEY_LEFT:    direction = { RESET_VEC2 }; transform.force.x = -2000; break;
			case INFO::KEY_D:   case INFO::KEY_RIGHT:   direction = { RESET_VEC2 }; transform.force.x = 2000; break;
			//case INFO::KEY_O:   transform.scale += 1.f * FIXED_DT; break;
			//case INFO::KEY_P:   transform.scale -= 1.f * FIXED_DT; break;
			//case INFO::KEY_Q:   transform.rotation -= 1.f * FIXED_DT;   break;
			//case INFO::KEY_E:   transform.rotation += 1.f * FIXED_DT;   break;
			//case INFO::KEY_1:   model.AddAlpha(0.2f * FIXED_DT);        break;
			//case INFO::KEY_2:   model.AddAlpha(-0.2f * FIXED_DT);       break;
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
				transform.force = { RESET_VEC2 };
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
}
