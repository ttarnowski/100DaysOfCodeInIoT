#pragma once

class EventDispatcher {
private:
  struct Listener {
    const char *eventName;
    std::function<void(void)> callback;
    bool isOnce;
  };
  std::vector<Listener> listeners;

public:
  void on(const char *eventName, std::function<void(void)> fn) {
    this->listeners.push_back(Listener{eventName, fn, false});
  }

  void once(const char *eventName, std::function<void(void)> fn) {
    this->listeners.push_back(Listener{eventName, fn, true});
  }

  void dispatch(const char *eventName) {
    for (auto it = this->listeners.begin(); it != this->listeners.end(); ++it) {
      if (strcmp(it->eventName, eventName) == 0) {
        it->callback();

        if (it->isOnce) {
          this->listeners.erase(it--);
        }
      }
    }
  }
};