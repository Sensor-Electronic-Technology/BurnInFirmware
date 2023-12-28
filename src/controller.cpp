#include "controller.hpp"

// void controller::handle_serial(){
//     byte inByte1 = 0;
//     word buff = 0;
//     while (Serial.available()) {
//         inByte1 = (byte)Serial.read();
//         if (((char)inByte1 == 'S') && (!this->systemState.IsRunning())) {
//             //this->StartTest();
//         } else if ((char)inByte1 == 'R') {
//             //this->Reset();
//         } else if (((char)inByte1 == 'T') && (this->canTestCurrent())) {
//             //this->TestProbe();
//         } else if (((char)inByte1 == 'H') && (!this->systemState.IsRunning())) {
//             //this->ToggleHeating();
//         } else if (((char)inByte1 == 'P')  && (this->systemState.IsRunning())) {
//             //this->PauseTest();
//         } else if (((char)inByte1 == 'C') && (!this->systemState.IsRunning())) {
//             //this->ToggleCurrent();
//         } else if (((char)inByte1 == 'U') && (!this->systemState.IsRunning())) {

//         }
//     }
// }