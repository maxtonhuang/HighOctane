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
*	@file		Message.cpp
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
*	@brief		Messaging System for all systems to communicate with each other
*
*	This file contains the messaging system which sends a Postcard out to
*   respective mailboxes for every event, such as Key Presses to Movement
*   System.
*
******************************************************************************/

#include "Message.h"
#include "Input.h"
#include "DebugDiagnostic.h"

/******************************************************************************
*
*	@brief Creates Mailboxes for each system
*
*	-
*
******************************************************************************/
void Mail::RegisterMailbox(ADDRESS system) {
    mailbox[system].reserve(MAILBOX_RESERVE_CAP);
}

/******************************************************************************
*
*	@brief Sends out all queued mails to the respective systems
*
*	-
*
******************************************************************************/
void Mail::SendMails() {
    // Send out all mail
    for (Postcard msg : mailQueue) {
        switch (msg.type) {
        case TYPE::KEY_TRIGGERED:
            mailbox[ADDRESS::INPUT].emplace_back(msg);
            mailbox[ADDRESS::MODEL].emplace_back(msg);
            mailbox[ADDRESS::MOVEMENT].emplace_back(msg);
            mailbox[ADDRESS::PHYSICS].emplace_back(msg);
        break;
        case TYPE::KEY_DOWN:
            mailbox[ADDRESS::MOVEMENT].emplace_back(msg);
            //mailbox[ADDRESS::MODEL].emplace_back(msg);
            break;
        case TYPE::KEY_UP:
            //mailbox[ADDRESS::MODEL].emplace_back(msg);
            break;
        case TYPE::MOUSE_MOVE:
            mailbox[ADDRESS::MOVEMENT].emplace_back(msg);
            break;
        default:
            break;
        }        
    }
    mailQueue.clear();
}

/******************************************************************************
*
*	@brief Creates a Postcard (message)
*
*	This function creates a 'Postcard' that contains information about an
*   event and sends it to the mail outbox queue to be sent out later.
*
******************************************************************************/
void Mail::CreatePostcard(TYPE messageType, ADDRESS from, INFO info, float posX, float posY) {
    // Sending a message
    Postcard tmp_Msg{};
    tmp_Msg.type = messageType;
    tmp_Msg.from = from;
    tmp_Msg.info = info;
    tmp_Msg.posX = posX;
    tmp_Msg.posY = posY;
    mailQueue.emplace_back(tmp_Msg);
}

