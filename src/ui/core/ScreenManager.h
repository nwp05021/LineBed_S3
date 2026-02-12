#pragma once
#include <vector>
#include <memory>
#include "Screen.h"
#include "UiContext.h"
#include "UiCommand.h"

#include "../overlay/Overlay.h"
#include "../overlay/ToastOverlay.h"
#include "../overlay/DialogOverlay.h"

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
    _dispW = d.width();
    _dispH = d.height();
    _stack.clear();
    _overlay.reset();
    _cmdCount = 0;
    _lastRev = _store.revision;

    push(root);
  }

  // -----------------------------
  // CommandQueue
  // -----------------------------
  void post(const UiCommand& cmd) override {
    if (_cmdCount < MAX_CMDS)
      _cmds[_cmdCount++] = cmd;
  }

  //---------------------------------------------
  // Dispatch 
  //  - InputLock + Modal Interaction 안정화
  //---------------------------------------------
  void dispatch(const UiEvent& e) {
    syncStore();

    // Overlay가 있으면 우선 처리
    if (_overlay) {
      // KeyVeryLongPress은 항상 홈(escape)로 처리
      if (e.type == UiEventType::KeyVeryLongPress) {
        _ctx.commands.post(UiCommand::Dismiss());
        return;
      }

      if (_overlay->handleEvent(_ctx, e)) {
        processCommands();
        return;
      }

      // overlay 존재 + 이벤트 소비되지 않은 경우
      return;
    }

    // 그 외 이벤트 처리
    routeToScreen(e);
    processCommands();
  }

  // -----------------------------
  // Render Flow
  // -----------------------------
  void render(IDisplay& d) {
    renderScreen(d);
    renderOverlay(d);
  }

  Screen* current() {
    return _stack.empty() ? nullptr : _stack.back().get();
  }

private:
  static constexpr uint8_t MAX_CMDS = 8;

  ScreenFactory& _factory;
  UiStore& _store;
  UiContext _ctx;

  std::vector<std::unique_ptr<Screen>> _stack;
  std::unique_ptr<Overlay> _overlay;

  UiCommand _cmds[MAX_CMDS];
  uint8_t _cmdCount = 0;

  uint32_t _lastRev = 0;
  int _dispW = 0, _dispH = 0;

  // ====================================================
  // 🔹 DISPATCH INTERNAL
  // ====================================================

  void syncStore() {
    if (!_stack.empty() && _store.revision != _lastRev) {
      _lastRev = _store.revision;
      _stack.back()->onStoreChanged(_ctx);
    }
  }

  bool routeToOverlay(const UiEvent& e) {
    if (!_overlay) return false;
    return _overlay->handleEvent(_ctx, e);
  }

  void routeToScreen(const UiEvent& e) {
    if (!_stack.empty())
      _stack.back()->handleEvent(_ctx, e);
  }

  void processCommands() {
    while (_cmdCount > 0) {
      UiCommand c = popCommand();
      executeCommand(c);
    }
  }

  // ====================================================
  // 🔹 RENDER INTERNAL
  // ====================================================

  void renderScreen(IDisplay& d) {
    if (_stack.empty()) return;
    if (_stack.back()->isDirty())
      _stack.back()->draw(_ctx, d);
  }

  void renderOverlay(IDisplay& d) {
    if (!_overlay) return;

    _overlay->draw(_ctx, d);

    if (_overlay->isExpired(millis())) {
      _overlay->onHide(_ctx);
      _overlay.reset();
    }
  }

  // ====================================================
  // 🔹 COMMAND EXECUTION
  // ====================================================

  void executeCommand(const UiCommand& c) {
    switch (c.type) {

      case UiCommandType::PushScreen:
        push(c.data.nav.id);
        break;

      case UiCommandType::ReplaceScreen:
        replace(c.data.nav.id);
        break;

      case UiCommandType::PopScreen:
        pop();
        break;

      case UiCommandType::ShowToast:
        _overlay = std::make_unique<ToastOverlay>(
            c.data.toast.msg, c.data.toast.ms);
        _overlay->onShow(_ctx);
        break;

      case UiCommandType::ShowDialog:
        _overlay = std::make_unique<DialogOverlay>(
            c.data.dialog.title,
            c.data.dialog.message,
            c.data.dialog.confirmOnly);
        _overlay->onShow(_ctx);
        break;

      case UiCommandType::DismissDialog:
        if (_overlay) {
          _overlay->onHide(_ctx);
          _overlay.reset();
        }
        break;

      default:
        break;
    }
  }

  UiCommand popCommand() {
    UiCommand c = _cmds[0];
    for (uint8_t i = 1; i < _cmdCount; ++i)
      _cmds[i - 1] = _cmds[i];
    _cmdCount--;
    return c;
  }

  // ====================================================
  // 🔹 NAVIGATION
  // ====================================================

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
    if (_stack.size() <= 1) return;

    _stack.back()->onExit(_ctx);
    _stack.pop_back();

    if (!_stack.empty())
      _stack.back()->onStoreChanged(_ctx);
  }
};
