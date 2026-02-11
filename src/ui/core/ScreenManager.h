//-----------------------------------------------
// ScreenManager.h
//
//  - 스택(push/pop) 또는 replace로 화면 전환
//  - “현재 화면만” 이벤트를 받고 렌더링
//-----------------------------------------------
#pragma once
#include <vector>
#include <memory>
#include "Screen.h"
#include "UiContext.h"
#include "UiCommand.h"

class ScreenFactory {
public:
  virtual ~ScreenFactory() = default;
  virtual std::unique_ptr<Screen> create(ScreenId id) = 0;
};

class ScreenManager : public UiCommandQueue {
public:
  ScreenManager(ScreenFactory& factory, UiStore& store)
    : _factory(factory), _store(store), _ctx{_store, *this} {}

  void init(IDisplay& d, ScreenId root) {
    _dispW = d.width(); _dispH = d.height();
    _stack.clear();
    push(root);
  }

  // UiCommandQueue
  void post(const UiCommand& cmd) override {
    if (_cmdCount < MAX_CMDS) _cmds[_cmdCount++] = cmd;
  }

  void dispatch(const UiEvent& e) {
    if (!_stack.empty()) {
      auto& top = *_stack.back();
      // store 변경 감지(단순)
      if (_store.revision != _lastRev) { _lastRev = _store.revision; top.onStoreChanged(_ctx); }
      top.handleEvent(_ctx, e);
    }
    applyCommands();
  }

  void renderIfNeeded(IDisplay& d) {
    if (_stack.empty()) return;
    if (_stack.back()->isDirty()) _stack.back()->draw(_ctx, d);
  }

private:
  void applyCommands() {
    for (uint8_t i=0; i<_cmdCount; i++) {
      const auto& c = _cmds[i];
      switch (c.type) {
        case UiCommandType::PushScreen:    push(c.data.nav.id); break;
        case UiCommandType::ReplaceScreen: replace(c.data.nav.id); break;
        case UiCommandType::PopScreen:     pop(); break;
        case UiCommandType::ShowToast:     showToast(c.data.toast.msg, c.data.toast.ms); break;
        default: break;
      }
    }
    _cmdCount = 0;
  }

  void push(ScreenId id) {
    auto s = _factory.create(id);
    if (!s) return;
    s->layout(_ctx, _dispW, _dispH);
    s->onEnter(_ctx);
    _stack.emplace_back(std::move(s));
  }

  void replace(ScreenId id) {
    if (!_stack.empty()) {
      _stack.back()->onExit(_ctx);
      _stack.pop_back();
    }
    push(id);
  }

  void pop() {
    if (_stack.size() <= 1) return; // root 보호
    _stack.back()->onExit(_ctx);
    _stack.pop_back();
    _stack.back()->onStoreChanged(_ctx);
  }

  void showToast(const char* msg, uint16_t ms) {
    // 지금은 단순: store에 토스트 상태 저장해서 overlay로 그리면 됨
    // (다음 단계에서 ToastOverlay Screen/Widget로 구현)
    (void)msg; (void)ms;
  }

private:
  static constexpr uint8_t MAX_CMDS = 8;

  ScreenFactory& _factory;
  UiStore& _store;
  UiContext _ctx;

  std::vector<std::unique_ptr<Screen>> _stack;

  UiCommand _cmds[MAX_CMDS];
  uint8_t _cmdCount = 0;

  uint32_t _lastRev = 0;
  int _dispW = 0, _dispH = 0;
};
