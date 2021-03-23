// The local header file
#include "canMessaging.h"

// A string for storing the command to be processed (not all commands can be sent with a single CAN packet)
String CANCommandString;

// CAN ID of the motor driver
// X:1, Y:2, Z:3, Z2:4..., E0:10, E1:11...
uint16_t canID = DEFAULT_CAN_ID;

// Function for initializing the CAN interface
void initCAN() {

    // Initialize the CAN interface
    CANInit(CAN_1000KBPS);

    // Clear the command string
    CANCommandString = String();
}

// Send a String over the CAN bus. Strings will have a "<" to start and a ">" to end
void sendCANString(uint16_t ID, String string) {

    // Add the beginning character to the string
    string = '<' + string;

    // Loop through forever, only exiting on sending the last string
    while (true) {

        // Check the length of the string (need an extra character for the ">" (ending character))
        if ((string.length() + 1) > 8) {

            // Set the ID
            canMessage message(ID, string.substring(0, 7));

            // Send the message over the CAN bus
            CANSend(message);

            // String needs to be split into multiple packets
            string.remove(0, 8);
        }
        else {
            // Build the CAN message
            canMessage message(ID, string.substring(0, string.length() - 1));

            // Add the terminator to the end
            message.data[string.length()] = '>';

            // Send the message over the CAN bus
            CANSend(message);
        }
    }
}

// Receive a message over the CAN bus (only uses characters)
void receieveCANMsg() {

    // Check to make sure that we can actually read a value
    if (CANMsgAvail()) {

        // Create the storage for the CAN message with the read value
        canMessage message = CANReceive();

        // Check to see what ID the message has, only listening if it's for this board
        if (message.id == canID) {

            // Copy data to the command string
            CANCommandString += message.toString();

            // Check to see if the command buffer is full ('>' is the termanator)
            if (CANCommandString.indexOf('>')) {

                // Parse the command
                parseString(CANCommandString.substring(0, CANCommandString.indexOf('>')));

                // Empty the buffer
                CANCommandString = "";
            }
        }
    }
}

// Sets the CAN ID of the board
void setCANID(uint16_t newCANID) {
    canID = newCANID;
}

// Gets the CAN ID of the board
uint16_t getCANID() {
    return canID;
}