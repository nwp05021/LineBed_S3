#pragma once
#include <Arduino.h>

#ifndef DEVICE_LINE_ID
#define DEVICE_LINE_ID "line-001"
#endif
#ifndef DEVICE_BED_ID
#define DEVICE_BED_ID "bed-001"
#endif

inline String topicBase() { return String("farm/v1/") + DEVICE_LINE_ID + "/" + DEVICE_BED_ID; }
inline String topicState() { return topicBase() + "/state"; }
inline String topicCmd()   { return topicBase() + "/cmd"; }
inline String topicCmdSet(){ return topicBase() + "/cmd/set"; }
inline String topicCmdReboot(){ return topicBase() + "/cmd/reboot"; }
