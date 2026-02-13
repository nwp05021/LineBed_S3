# SmartFarm V1 — Enum Design & Usage Standard (동결 규격)

본 문서는 SmartFarm V1 펌웨어(LineBed/GrowBed 공통)에서 사용되는 `enum class`의
**명명 규칙, 사용 규칙, 직렬화(toString) 규칙**을 정의한다.

> V1 동안 이 규격은 변경하지 않는 것을 원칙으로 한다.

---

## 1. 기본 원칙

1) 상태/이벤트/모드/에러/표현(UI Visual)은 반드시 enum으로 정의한다.

2) 상태는 내부 플래그로 판단하지 않는다.
   - UI/로그/Cloud는 동일 enum을 사용한다.

3) enum → 문자열 변환은 **중앙 집중**한다.
   - 모듈마다 `switch`를 복사해서 쓰지 않는다.

---

## 2. 네이밍 규칙

### 2.1 Enum 타입 이름

- PascalCase
- 역할을 접미사로 구분

예)
- `SystemState`, `NetworkState`, `CloudState`
- `ErrorCode`
- `UiEventType`

### 2.2 Enum 값 이름

- PascalCase
- 상태는 명사/형용사 (동사 금지)

예)

```cpp
enum class SystemState : uint8_t {
  Booting, Idle, Provisioning, Running, Error
};
```

---

## 3. Underlying Type 규칙

- 상태/모드/표현: `uint8_t`
- ErrorCode: `uint16_t`
- Cloud 직렬화 대상은 반드시 underlying type을 명시

---

## 4. toString(직렬화) 규칙

### 4.1 Machine string 규칙

- Cloud/저장/로그용 문자열은 **lowercase**
- snake_case 허용(에러코드 등), 단 “한 번 정하면 변경 금지”
- enum 값과 1:1 대응

예)

```cpp
const char* toString(SystemState v) {
  switch(v) {
    case SystemState::Running: return "running";
    default: return "unknown";
  }
}
```

### 4.2 UI 문자열과 분리

- UI 문구는 `toUiText(...)` 같은 별도 경로로 관리한다.
- Cloud 문자열을 UI에 직접 노출하지 않는다.

---

## 5. 금지 사항

- enum과 문자열 직접 비교로 로직 수행
- enum 값 이름/순서 변경(특히 Cloud/대시보드가 해석하는 값)
- 프로젝트 내 enum 스타일 혼재

---

## 6. 코드 위치(프로젝트 규격)

```
src/core/enum/     # enum 정의 + toString
src/core/state/    # AppState + StateController
src/core/logging/  # Logger
```
