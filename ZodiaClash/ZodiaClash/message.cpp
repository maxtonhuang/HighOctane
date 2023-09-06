#include "message.h"



namespace Architecture {

    std::list<Message> messageQueue;

    void createMessage(MessageType messageType, int sender, int receipient) {
        // Sending a message
        Message tempMsg;
        tempMsg.type = messageType;
        tempMsg.senderEntityID = sender;
        tempMsg.recipientEntityID = receipient;
        messageQueue.emplace_back(tempMsg);
    }



    /* Receiving messages example:
    // Receiving messages
    void PhysicsSystem::ProcessMessages() {
        while (!messageQueue.empty()) {
            Message message = messageQueue.front();
            messageQueue.pop();

            // Check message type and process accordingly
            if (message.type == MessageType::Collision) {
                HandleCollision(message);
            }
            // Handle other message types...
        }
    }
    */

    // Function to convert Windows events to custom messages
    void ConvertWindowsEventToMessage(unsigned int uMsg) {
        Message message;
        switch (uMsg) {
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
            message.type = MessageType::CustomEvent;
            // Populate message with relevant data
            break;
        }

        // Send the converted message to the game's messaging system
        messageQueue.emplace_back(message);
    }
}