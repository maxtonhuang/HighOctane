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
        case TYPE::KEY_CHECK:
            mailbox[ADDRESS::INPUT].emplace_back(msg);
            break;
        case TYPE::KEY_DOWN:
            mailbox[ADDRESS::MOVEMENT].emplace_back(msg);
            break;
        case TYPE::KEY_UP:
            mailbox[ADDRESS::MODEL].emplace_back(msg);
            break;
        default:
            break;
        }        
    }
    mailQueue.clear();

}

void Mail::CreatePostcard(TYPE messageType, ADDRESS from, INFO info) {
    // Sending a message
    Postcard tmp_Msg{};
    tmp_Msg.type = messageType;
    tmp_Msg.from = from;
    tmp_Msg.info = info;
    mailQueue.emplace_back(tmp_Msg);
}


/*
// Function to convert Windows events to custom messages
void Mail::ConvertWindowsEventToPostcard(unsigned int uMsg) {
    Postcard message{};
    switch (uMsg) {
    case WM_CLOSE:
        CreatePostcard(TYPE::QUIT, ADDRESS::WINDOWS, INFO::NONE);
        break;
    case WM_KEYDOWN:
        message.type = MessageType::KeyDown;
        // Populate message with key data
        break;
    case WM_KEYUP:
        message.type = MessageType::KeyUp;
        // Populate message with key data
        break;
    case WM_LBUTTONDOWN:
        message.type = MessageType::MouseClick;
        // Populate message with mouse click data
        break;
        // Handle other Windows events...
        
    default:
        message.type = TYPE::CUSTOM_EVENT;
        // Populate message with relevant data
        break;
    }
}
*/