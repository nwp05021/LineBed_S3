#pragma once
#include <Arduino.h>

namespace core::logging {

enum class Level : uint8_t { Debug, Info, Warn, Error };

class Logger {
public:
    static void log(Level lvl, const __FlashStringHelper* tag, const String& msg) {
        const char* pfx = prefix(lvl);
        Serial.print(pfx);
        Serial.print(" ");
        Serial.print(tag);
        Serial.print(": ");
        Serial.println(msg);
    }

    static void log(Level lvl, const __FlashStringHelper* tag, const char* msg) {
        const char* pfx = prefix(lvl);
        Serial.print(pfx);
        Serial.print(" ");
        Serial.print(tag);
        Serial.print(": ");
        Serial.println(msg);
    }

private:
    static const char* prefix(Level lvl) {
        switch (lvl) {
            case Level::Debug: return "[D]";
            case Level::Info:  return "[I]";
            case Level::Warn:  return "[W]";
            case Level::Error: return "[E]";
            default:           return "[?]";
        }
    }
};

} // namespace core::logging
