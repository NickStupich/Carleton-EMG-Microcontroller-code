#include "settings.h"

#ifndef BLUETOOTH

#define BLUETOOTH

#if HAS_STRING_FUNC == 1
void sendMsg(char* s);
#endif

void sendByte(char c);
#endif