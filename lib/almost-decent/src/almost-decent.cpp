#include <string>
#include <sstream>
#include <iomanip>
#include "almost-decent.h"
#include "bt_message_building.h"
#include "bt_comms.h"
#include "bt_messages.h"

#include <HX711.h>
#include <mutex>

// https://decentespresso.com/decentscale_api

std::mutex serial_mtx;

class AlmostDecent_ {
  public:
  ScaleState m_state = ScaleState::startup;
  int m_doutPin, m_sckPin;
  HX711 m_scale;
  long m_last_broadcast_millis = 0;

  int beginScale() {
    m_scale.begin(m_doutPin, m_sckPin);
    bool success = m_scale.wait_ready_retry(5, 1000);
    m_scale.set_scale();
    m_scale.power_up();
    success = m_scale.wait_ready_retry(5, 1000);
    DEBUG_SERIAL.printf("scale init: %d\n", success);
    return success;
  }

  void btTareCallback() {
    if (m_scale.wait_ready_timeout(500UL) == false) {
      DEBUG_SERIAL.println("scale not ready");
      return;
    }
    std::lock_guard<std::mutex> lck(serial_mtx);
    ScaleState prevState = m_state;
    m_state = ScaleState::taring;
    DEBUG_SERIAL.println("taring internal");
    m_scale.tare();
    m_state = prevState;
  }
};

void initBluetooth() {
  initBT();
}

std::string formatWeightWithTimestamp(int weight, int minutes, int seconds, int milliseconds)
{

  std::vector<unsigned char> message = buildWeightMessage(weight, minutes, seconds, milliseconds);

  std::stringstream stream;

  for (auto b : message) {
    stream << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << (int)b;
  }

  return stream.str();
}

void sendWeight(int gramsMultipliedByTen) {
  broadcastWeight(gramsMultipliedByTen);
}

void almostDecentLog(AlmostDecentScale *scale, const char *message){
    if (!scale->m_logCallback) {
      return;
    }
    scale->m_logCallback(message);
}

AlmostDecentScale::AlmostDecentScale(int loadCellDoutPin, int loadCellSckPin)
{
  m_internal = new AlmostDecent_();
  m_internal->m_doutPin = loadCellDoutPin;
  m_internal->m_sckPin = loadCellSckPin; 
}

void AlmostDecentScale::initialize(bool ownThread) 
{
  almostDecentLog(this, "initializing");
  bool success = m_internal->beginScale();
  if (!success) {
    almostDecentLog(this, "failed to connect to scale, aborting");
    m_internal->m_state = ScaleState::error;
    return;
  }
  initBT();
  almostDecentLog(this, "Almost decently initialized");
  setTareCallback([this]() {
    almostDecentLog(this, "taring");
    this->tare();
  });
  setCalibrateCallback([this]() {
    this->calibration();
  });
  setFactorCallback([this] (float newFactor) {
    almostDecentLog(this, "setting new factor");
    this->setFactor(newFactor);
  });
  m_internal->m_state = ScaleState::ready;
}

void AlmostDecentScale::tare()
{
  almostDecentLog(this, "taring");
  m_internal->btTareCallback();
}

void AlmostDecentScale::begin()
{
  char msg[50];
  snprintf(msg, 50, "scale state: %d", m_internal->m_state);
  almostDecentLog(this, msg);
  if (m_internal->m_state != ScaleState::measuring &&
      m_internal->m_state != ScaleState::ready)
  {
    almostDecentLog(this, "Scale not ready!");
    return;
  }
  m_internal->m_scale.tare();
  m_internal->m_state = ScaleState::measuring;
}

void AlmostDecentScale::calibration()
{
  almostDecentLog(this, "starting calibration, please remove all weights and wait for tare");
  m_internal->m_state = ScaleState::calibrating;
  m_internal->m_scale.set_scale();
  delay(5000);
  m_internal->m_scale.tare(100);
  delay(1000);
  almostDecentLog(this, "add a known weight (10s)");
  delay(10000);
  float units = m_internal->m_scale.get_units(50);
  char msg[50];
  snprintf(msg, 49, "units value is: %f", units);
  broadCastUnits(units * 1000.f);
  almostDecentLog(this, msg);
  delay(2000);
  almostDecentLog(this, "waiting for set factor (units/known_weight)");
}

void AlmostDecentScale::setFactor(float factor)
{
  m_internal->m_scale.set_scale(factor);
  // if (m_internal->m_state != ScaleState::calibrating) {
  //   return;
  // }
  if (m_factorCallback) {
    m_factorCallback(factor);
  }
  almostDecentLog(this, "calibration done, ready to measure");
  m_internal->m_state = ScaleState::measuring;
}

const int broadcastIntervalMillis = 50;

void AlmostDecentScale::tick()
{
  std::lock_guard<std::mutex> lck(serial_mtx);
  ScaleState currentState = getState();
  switch (currentState)
  {
  case ScaleState::calibrating:
    /* calibration code */
    break;
  case ScaleState::measuring:
  {
    float weight = m_internal->m_scale.get_units(1);
    long currentMillis = millis();
    long timeDelta = currentMillis - m_internal->m_last_broadcast_millis;
    if (timeDelta < broadcastIntervalMillis)
    {
      break;
    }
    #ifdef DEBUG
    char msg[50];
    snprintf(msg, 49, "time delta: %d\n", timeDelta);
    almostDecentLog(this, msg);
    #endif
    sendWeight((int)(weight * 10.f));
    m_internal->m_last_broadcast_millis = currentMillis;
  }
  break;
  default:
    break;
  }
}

ScaleState AlmostDecentScale::getState()
{
  return m_internal->m_state;
}

const char *AlmostDecentScale::getStateString() 
{
  switch (m_internal->m_state)
  {
  case ScaleState::startup:
    return "startup";
  case ScaleState::ready:
    return "ready";
  case ScaleState::calibrating:
    return "calibrating";
  case ScaleState::measuring:
    return "measuring";
  case ScaleState::taring:
    return "taring";
  case ScaleState::error:
    return "error";
  default:
    return "unknown";
  }
}
