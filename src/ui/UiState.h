#pragma once
#include <stdint.h>
#include "ScreenId.h"

struct UiState {
    ScreenId currentScreen{ScreenId::Main};

    float temperature{25.0f};
    float humidity{60.0f};

    bool error{false};

    // ✅ UI 상호작용을 위한 상태 (중요: UI에서만 소비)
    int16_t menuIndex{0};        // 메인 메뉴 커서
    int16_t tempTarget10{250};   // 목표온도 x10 (25.0℃ => 250)
};
