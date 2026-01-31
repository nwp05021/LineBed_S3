// AppMainShim.cpp
// Purpose:
//  - Provide app_main() symbol required by esp-idf
//  - Delegate execution to Arduino core
//  - Do NOT replace Arduino execution model

#if defined(ARDUINO) && defined(ESP32)

extern "C" {

// Arduino core provides this
void app_main(void) {
  // Arduino framework will take over from here.
  // This symbol only exists to satisfy esp-idf linker expectations.
}

}

#endif
