#include <string>
#include <sstream>
#include <iomanip>
#include "almost-decent.h"
#include "bt_message_building.h"
#include "bt_comms.h"
#include "bt_messages.h"

#include <HX711.h>

// https://decentespresso.com/decentscale_api

class AlmostDecent_ {
  public:
  ScaleState m_state = ScaleState::startup;
  int m_doutPin, m_sckPin;
  HX711 m_scale;
  long m_last_broadcast_millis = 0;

  int beginScale() {
    m_scale.begin(m_doutPin, m_sckPin);
    return m_scale.wait_ready_retry(3, 1000);
  }

  void btTareCallback() {
    if (m_scale.is_ready() == false) {
      return;
    }
    m_scale.tare();
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
    if (scale->m_logCallback) {
      scale->m_logCallback(message);
    }
}

AlmostDecentScale::AlmostDecentScale(int loadCellDoutPin, int loadCellSckPin)
{
  m_internal = new AlmostDecent_();
  m_internal->m_doutPin = loadCellDoutPin;
  m_internal->m_sckPin = loadCellSckPin; 
}

void AlmostDecentScale::begin(bool ownThread) 
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
  m_internal->m_state = ScaleState::ready;
  setTareCallback([this]() {
    this->m_internal->btTareCallback();
  });
}

void AlmostDecentScale::calibration()
{
  almostDecentLog(this, "starting calibration, please wait for tare");
  m_internal->m_state = ScaleState::calibrating;
  m_internal->m_scale.set_scale();
  delay(5000);
  m_internal->m_scale.tare(100);
  delay(1000);
  almostDecentLog(this, "add a known weight");
  delay(5000);
  float units = m_internal->m_scale.get_units(10);
  char msg[50];
  snprintf(msg, 49, "units value is: %f", units);
  almostDecentLog(this, msg);
  delay(2000);
  almostDecentLog(this, "waiting for set factor (units/known_weight)");
}

void AlmostDecentScale::setFactor(float factor)
{
  m_internal->m_scale.set_scale(factor);
  if (m_internal->m_state != ScaleState::calibrating) {
    return;
  }
  almostDecentLog(this, "calibration done, ready to measure");
  m_internal->m_state = ScaleState::measuring;
}

void AlmostDecentScale::tick()
{
  ScaleState currentState = getState();
  switch (currentState)
  {
  case ScaleState::calibrating:
    /* calibration code */
    break;
  case ScaleState::measuring:
  {
    long currentMillis = millis();
    if (currentMillis - m_internal->m_last_broadcast_millis < 100)
    {
      break;
    }
    float weight = m_internal->m_scale.get_units(5);
    sendWeight((int)weight * 10);
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

