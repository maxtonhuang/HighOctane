#include "Message.h"
#include "Input.h"
#include "DebugDiagnostic.h"

Mail mail;

void Mail::RegisterMailbox(ADDRESS system) {
    mailbox[system].reserve(MAILBOX_RESERVE_CAP);
}

void Mail::SendMails() {
    // Send out all mail.
    for (Postcard msg : mailQueue) {
        switch (msg.type) {
        case TYPE::KEY_TRIGGERED:
        //    mailbox[ADDRESS::INPUT].emplace_back(msg);
        break;
        case TYPE::KEY_DOWN:
            mailbox[ADDRESS::MOVEMENT].emplace_back(msg);
            break;
        case TYPE::KEY_UP:
            mailbox[ADDRESS::MODEL].emplace_back(msg);
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

