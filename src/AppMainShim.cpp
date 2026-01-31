// AppMainShim.cpp
// 목적:
// - esp-idf 링크가 요구하는 app_main() 심볼을 제공하되
// - Arduino 실행 모델(initArduino + loopTask)을 직접 구동해서
//   setup()/loop()가 정상 실행되게 한다.

#if defined(ARDUINO) && defined(ESP32)

#include <Arduino.h>

extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
}

static void ArduinoLoopTask(void* pv) {
  (void)pv;

  setup();

  for (;;) {
    loop();
    // Arduino-ESP32에서 흔히 사용하는 양보 (WDT/스케줄링 안정성)
    vTaskDelay(1);
  }
}

extern "C" void app_main(void) {
  // Arduino core init
  initArduino();

  // Arduino loop task 생성 (Arduino 코어가 하는 방식과 동일한 목적)
  xTaskCreatePinnedToCore(
    ArduinoLoopTask,
    "ArduinoLoop",
    8192,
    nullptr,
    1,
    nullptr,
    ARDUINO_RUNNING_CORE
  );
}

#endif
