#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEHIDDevice.h>
#include "bt_comms.h"
#include "bt_messages.h"
#include "bt_message_building.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

static NimBLEServer *pServer;

std::function<void()> tareCallback = nullptr;

static int count = 1;

void setTareCallback(std::function<void()> callback) {
  tareCallback = callback;
}

class ServerCallbacks : public NimBLEServerCallbacks
{
  void onConnect(NimBLEServer *pServer)
  {
    Serial.println("Client connected");
    Serial.println("Multi-connect support: start advertising");
    NimBLEDevice::startAdvertising();
  };
  /** Alternative onConnect() method to extract details of the connection.
   *  See: src/ble_gap.h for the details of the ble_gap_conn_desc struct.
   */
  void onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc)
  {
    Serial.print("Client address: ");
    Serial.println(NimBLEAddress(desc->peer_ota_addr).toString().c_str());
    /** We can use the connection handle here to ask for different connection parameters.
     *  Args: connection handle, min connection interval, max connection interval
     *  latency, supervision timeout.
     *  Units; Min/Max Intervals: 1.25 millisecond increments.
     *  Latency: number of intervals allowed to skip.
     *  Timeout: 10 millisecond increments, try for 5x interval time for best results.
     */
    pServer->updateConnParams(desc->conn_handle, 24, 48, 0, 60);
  };
  void onDisconnect(NimBLEServer *pServer)
  {
    Serial.println("Client disconnected - start advertising");
    NimBLEDevice::startAdvertising();
  };
  void onMTUChange(uint16_t MTU, ble_gap_conn_desc *desc)
  {
    Serial.printf("MTU updated: %u for connection ID: %u\n", MTU, desc->conn_handle);
  };
};

void printHex(const char *str, bool endline = true)
{
  // Print string as hexadecimal
  for (size_t i = 0; str[i] != '\0'; ++i)
  {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(str[i]);
  }
  if (endline) {
    std::cout << std::endl;
  }
}

/** Handler class for characteristic actions */
class CharacteristicCallbacks : public NimBLECharacteristicCallbacks
{
  void onRead(NimBLECharacteristic *pCharacteristic)
  {
    Serial.print(pCharacteristic->getUUID().toString().c_str());
    Serial.print(": onRead(), value: ");
    Serial.println(pCharacteristic->getValue().c_str());
    pCharacteristic->setValue(count++);
  };

  void onWrite(NimBLECharacteristic *pCharacteristic)
  {
    Serial.print(pCharacteristic->getUUID().toString().c_str());
    Serial.print(": onWrite(), value: ");
    printHex(pCharacteristic->getValue().c_str());
  };
  /** Called before notification or indication is sent,
   *  the value can be changed here before sending if desired.
   */
  void onNotify(NimBLECharacteristic *pCharacteristic)
  {
    // Serial.println("Sending notification to clients");
  };

  /** The status returned in status is defined in NimBLECharacteristic.h.
   *  The value returned in code is the NimBLE host return code.
   */
  void onStatus(NimBLECharacteristic *pCharacteristic, Status status, int code)
  {
    String str = ("Notification/Indication status code: ");
    str += status;
    str += ", return code: ";
    str += code;
    str += ", ";
    str += NimBLEUtils::returnCodeToString(code);
    // Serial.println(str);
  };

  void onSubscribe(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc *desc, uint16_t subValue)
  {
    String str = "Client ID: ";
    str += desc->conn_handle;
    str += " Address: ";
    str += std::string(NimBLEAddress(desc->peer_ota_addr)).c_str();
    if (subValue == 0)
    {
      str += " Unsubscribed to ";
    }
    else if (subValue == 1)
    {
      str += " Subscribed to notfications for ";
    }
    else if (subValue == 2)
    {
      str += " Subscribed to indications for ";
    }
    else if (subValue == 3)
    {
      str += " Subscribed to notifications and indications for ";
    }
    str += std::string(pCharacteristic->getUUID()).c_str();

    Serial.println(str);
  };
};

class WriteCharacteristicCallbacks : public CharacteristicCallbacks {
  void onWrite(NimBLECharacteristic *pCharacteristic)
  {
    // special write char handling
    Serial.print(pCharacteristic->getUUID().toString().c_str());
    Serial.print(": onWrite(), value: ");
    NimBLEAttValue val = pCharacteristic->getValue();
    printHex(val.c_str());

    if (val.length() < 4 || val.data()[0] != DecentHeader) {
      return;
    }
    const uint8_t *data = val.data();
    switch (data[1]) {
      case BT_Command::LED:
        Serial.print("LED CMD");
        break;
      case BT_Command::TIMER:
        Serial.print("TIMER CMD");
        break;
      case BT_Command::TARE:
        Serial.print("TARE CMD");
        if (tareCallback) {
          tareCallback();
        }
        break;
    }
    char payload[3];
    payload[0] = data[2];
    payload[1] = data[3];
    payload[2] = '\0';
    Serial.print(": ");
    printHex(payload);
  };
};

static CharacteristicCallbacks weightCharacteristicCallbacks;
static WriteCharacteristicCallbacks writeCharacteristicCallbacks;

void initBT()
{
  NimBLEDevice::init("Decent Scale");
#ifdef ESP_PLATFORM
  NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */
#else
  NimBLEDevice::setPower(9); /** +9db */
#endif

  NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);

  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  NimBLEService *scaleService = pServer->createService("FFF0");
  NimBLECharacteristic *weightCharacteristic = scaleService->createCharacteristic(
      "FFF4",
      NIMBLE_PROPERTY::READ |
          NIMBLE_PROPERTY::NOTIFY);

  NimBLECharacteristic *writeCharacteristic = scaleService->createCharacteristic(
      "36F5",
      NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);

  weightCharacteristic->setValue("0");
  
  weightCharacteristic->setCallbacks(&weightCharacteristicCallbacks);
  writeCharacteristic->setCallbacks(&writeCharacteristicCallbacks);
  NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();

  scaleService->start();


  pAdvertising->addServiceUUID(scaleService->getUUID());
  
  pAdvertising->setScanResponse(true);
  NimBLEHIDDevice device(pServer);
  device.pnp(3, 0xf0a, 0, 0x03);
  device.startServices();
  pAdvertising->start();

  Serial.println("Advertising Started");
}


void broadcastWeight(int gramsMultipliedByTen) {
  NimBLEService *scaleService = pServer->getServiceByUUID("FFF0");
  if (scaleService == NULL) {
    Serial.println("No service running, aborting");
    return;
  }

  NimBLECharacteristic *weightCharacteristic = scaleService->getCharacteristic("FFF4");

  if (weightCharacteristic == NULL) {
    Serial.println("No weight characteristic found, aborting");
    return;
  }

  uint32_t time = millis();
  int minutes = time / 1000 / 60;
  int seconds = time / 1000 - (minutes * 60);
  int milliseconds = time - (seconds * 1000) - (minutes * 60 * 1000);
  std::vector<uint8_t> message = buildWeightMessage(gramsMultipliedByTen, minutes, seconds, milliseconds);

  weightCharacteristic->setValue(message);
  if (weightCharacteristic->getSubscribedCount() > 0) {
    weightCharacteristic->notify();
  }
  
}