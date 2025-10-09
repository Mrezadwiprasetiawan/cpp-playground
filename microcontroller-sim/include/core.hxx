#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#define HIGH 1
#define LOW  0

class Pin {
  uint8_t value;
  friend class Board;
  friend class Device;
  std::atomic<bool> pwmActive{false};
  std::thread       pwmThread;
  std::mutex        pwmLock;

  void stopPWM() {
    if (pwmActive) {
      pwmActive = false;
      if (pwmThread.joinable()) pwmThread.join();
    }
  }

  void set_value(bool val) {
    stopPWM();
    if (value & 128) value &= 128 + val;
    else value = val;
  }

  void setMode(bool input) {
    if (input) value |= 128;
    else value &= 127;
  }

  void startPWM(uint8_t duty) {
    if (pwmActive) return;
    pwmActive = true;
    pwmThread = std::thread([this, duty]() {
      using namespace std::chrono;
      using namespace std::chrono_literals;
#if defined(__OPTIMIZE__) && !defined(__NO_INLINE__) || defined(__MSC_VER)
      (void)std::chrono::steady_clock::now();  // suppress unused <chrono> warning
#endif

      auto period = 1000us;
      while (pwmActive) {
        {
          std::lock_guard<std::mutex> lock(pwmLock);
          value = HIGH;
        }
        std::this_thread::sleep_for(period * duty / 255);
        {
          std::lock_guard<std::mutex> lock(pwmLock);
          value = LOW;
        }
        std::this_thread::sleep_for(period * (255 - duty) / 255);
      }
    });
  }

 public:
  Pin() = default;
  ~Pin() { stopPWM(); }

  Pin(Pin&& other) noexcept {
    std::lock_guard<std::mutex> lock(other.pwmLock);
    value     = other.value;
    pwmActive = other.pwmActive.load();
    if (other.pwmThread.joinable()) other.pwmThread.detach();
  }

  Pin& operator=(Pin&& other) noexcept {
    if (this != &other) {
      stopPWM();
      std::lock_guard<std::mutex> lock(other.pwmLock);
      value     = other.value;
      pwmActive = other.pwmActive.load();
      if (other.pwmThread.joinable()) other.pwmThread.detach();
    }
    return *this;
  }

  Pin(const Pin&)            = delete;
  Pin& operator=(const Pin&) = delete;

  uint8_t get_value() {
    std::lock_guard<std::mutex> lock(pwmLock);
    return value;
  }
};

// unimplemented yet
class Device {};

class Board {
  std::vector<Pin>                       pins;
  std::function<void(Board&)>            mainLogic = nullptr;
  std::function<void(std::vector<Pin>&)> debugger  = nullptr;

  std::thread mainThread;
  std::thread debuggerThread;

 public:
  Board(uint8_t pinSize) { pins.resize(pinSize); }

  void set_main(void (*fn)(Board&)) { mainLogic = fn; }
  void set_debugger(void (*fn)(std::vector<Pin>&)) { debugger = fn; }

  void digitalWrite(uint8_t pinNumber, bool val) { pins[pinNumber].set_value(val); }

  void analogWrite(uint8_t pinNumber, uint8_t duty) {
    if (duty == 0) pins[pinNumber].set_value(LOW);
    else if (duty == 255) pins[pinNumber].set_value(HIGH);
    else pins[pinNumber].startPWM(duty);
  }

  void start() {
    if (mainLogic && !mainThread.joinable()) mainThread = std::thread(mainLogic, std::ref(*this));
    if (debugger && !debuggerThread.joinable()) debuggerThread = std::thread(debugger, std::ref(pins));
  }

  void join() {
    if (mainThread.joinable()) mainThread.join();
    for (auto& pin : pins) pin.stopPWM();
    if (debuggerThread.joinable()) debuggerThread.join();
  }
};
