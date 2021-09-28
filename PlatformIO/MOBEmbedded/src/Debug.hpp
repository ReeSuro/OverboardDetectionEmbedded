#pragma once
#define DEBUG
#ifdef DEBUG
#define DEBUG_LOGLN(a) Serial.println(a);
#define DEBUG_LOG(a) Serial.print(a);
#else
#define DEBUG_LOGLN(a)
#define DEBUG_LOG(a)
#endif