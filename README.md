# SmartFarm Line MCU V1 (Base Firmware Skeleton)

PlatformIO + Arduino(as component) 기반 **ESP32-S3** 기본 펌웨어 스켈레톤입니다.

## 포함 기능(기본 동작)

- EC11 로터리 엔코더 입력(회전/클릭/롱클릭)
- ST7789(170×320) + TFT_eSPI + Sprite 렌더(깜빡임 최소)
- UI: MENU + 온도/습도/LED 주기 편집(확정/취소 흐름)
- 설정 SSOT + 디바운스 저장(Preferences)
- LED On/Off 주기 제어 예제(GPIO)
- Wi-Fi 연결(저장된 자격증명 기반)
- (옵션) BLE Provisioning: `ENABLE_BLE_PROV=1` 빌드 플래그로 활성화
- MQTT 업링크 예제(기본: test.mosquitto.org)

> AWS IoT(TLS/인증서) 연결과 토픽 매핑은 다음 단계에서 적용하세요.

## 빌드

PlatformIO에서 `esp32s3` 환경 빌드/업로드.


