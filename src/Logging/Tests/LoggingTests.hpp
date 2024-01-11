#pragma once


// class LoggingTests{
// public:
// void test_format(const __FlashStringHelper* format,...){
//     char buffer[128];
//     PGM_P pointer = reinterpret_cast<PGM_P>(format);
// 	va_list args;
// 	va_start(args, format);
// 	vsnprintf_P(buffer, sizeof(buffer), pointer, args);
// 	va_end(args);
//     Serial.println(buffer);
// }

// void test(SystemMessage msg,...){
//     char buffer[255];
//     const char* format=read_msg_table(msg);
//     va_list args;
// 	va_start(args, format);
// 	vsnprintf(buffer, sizeof(buffer), format, args);
// 	va_end(args);
//     Serial.println(buffer);
// }
// };