#ifdef BT_COMMS_NIMBLE
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
std::function<void()> calibrateCallback = nullptr;
std::function<void(float)> factorCallback = nullptr;

static int count = 1;

void setTareCallback(std::function<void()> callback) {
  tareCallback = callback;
}

void setCalibrateCallback(std::function<void()> callback) {
  calibrateCallback = callback;
}

void setFactorCallback(std::function<void(float)> callback)
{
  factorCallback = callback;
}

class ServerCallbacks : public NimBLEServerCallbacks
{
  void onConnect(NimBLEServer *pServer)
  {
    DEBUG_SERIAL.println("Client connected");
    DEBUG_SERIAL.println("Multi-connect support: start advertising");
    NimBLEDevice::startAdvertising();
  };
  /** Alternative onConnect() method to extract details of the connection.
   *  See: src/ble_gap.h for the details of the ble_gap_conn_desc struct.
   */
  void onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc)
  {
    DEBUG_SERIAL.print("Client address: ");
    DEBUG_SERIAL.println(NimBLEAddress(desc->peer_ota_addr).toString().c_str());
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
    DEBUG_SERIAL.println("Client disconnected - start advertising");
    NimBLEDevice::startAdvertising();
  };
  void onMTUChange(uint16_t MTU, ble_gap_conn_desc *desc)
  {
    DEBUG_SERIAL.printf("MTU updated: %u for connection ID: %u\n", MTU, desc->conn_handle);
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
  // void onRead(NimBLECharacteristic *pCharacteristic)
  // {
  //   DEBUG_SERIAL.print(pCharacteristic->getUUID().toString().c_str());
  //   DEBUG_SERIAL.print(": onRead(), value: ");
  //   DEBUG_SERIAL.println(pCharacteristic->getValue().c_str());
  //   // pCharacteristic->setValue(count++);
  // };

  void onWrite(NimBLECharacteristic *pCharacteristic)
  {
    DEBUG_SERIAL.print(pCharacteristic->getUUID().toString().c_str());
    DEBUG_SERIAL.print(": onWrite(), value: ");
    printHex(pCharacteristic->getValue().c_str());
  };
  /** Called before notification or indication is sent,
   *  the value can be changed here before sending if desired.
   */
  void onNotify(NimBLECharacteristic *pCharacteristic)
  {
    // DEBUG_SERIAL.println("Sending notification to clients");
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
    // DEBUG_SERIAL.println(str);
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

    DEBUG_SERIAL.println(str);
  };
};

class WriteCharacteristicCallbacks : public CharacteristicCallbacks {
  void onWrite(NimBLECharacteristic *pCharacteristic)
  {
    // special write char handling
    DEBUG_SERIAL.print(pCharacteristic->getUUID().toString().c_str());
    DEBUG_SERIAL.print(": onWrite(), value: ");
    NimBLEAttValue val = pCharacteristic->getValue();
    printHex(val.c_str());

    if (val.data()[0] != DecentHeader) {
      return;
    }
    const uint8_t *data = val.data();
    switch (data[1]) {
      case BT_Command::LED:
        DEBUG_SERIAL.print("LED CMD");
        break;
      case BT_Command::TIMER:
        DEBUG_SERIAL.print("TIMER CMD");
        break;
      case BT_Command::TARE:
        DEBUG_SERIAL.print("TARE CMD");
        if (tareCallback) {
          tareCallback();
        }
        break;
      case BT_CommandExtended::CALIBRATE:
        if (calibrateCallback) {
          calibrateCallback();
        }
        break;
      case BT_CommandExtended::FACTOR_SET:
        if (val.length() < 6) {
          break;
        }
        {
          uint32_t i32 = data[5] | (data[4] << 8) | (data[3] << 16) | (data[2] << 24);
          if (factorCallback)
          {
            DEBUG_SERIAL.printf("setting factor: %d/1000\n", i32);
            factorCallback(i32 / 1000.f);
        }
        }
        break;
    }
    if (val.length() > 4) {
      char payload[3];
      payload[0] = data[2];
      payload[1] = data[3];
      payload[2] = '\0';
      DEBUG_SERIAL.print(": ");
      printHex(payload);
    }
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

  DEBUG_SERIAL.println("Advertising Started");
}


void broadcastWeight(int gramsMultipliedByTen) {
  NimBLEService *scaleService = pServer->getServiceByUUID("FFF0");
  if (scaleService == NULL) {
    DEBUG_SERIAL.println("No service running, aborting");
    return;
  }

  NimBLECharacteristic *weightCharacteristic = scaleService->getCharacteristic("FFF4");

  if (weightCharacteristic == NULL) {
    DEBUG_SERIAL.println("No weight characteristic found, aborting");
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

void broadCastUnits(int unitsTimesThousand)
{
  NimBLEService *scaleService = pServer->getServiceByUUID("FFF0");
  if (scaleService == NULL)
  {
    DEBUG_SERIAL.println("No service running, aborting");
    return;
  }

  NimBLECharacteristic *writeCharacteristic = scaleService->getCharacteristic("36F5");

  if (writeCharacteristic == NULL)
  {
    DEBUG_SERIAL.println("No weight characteristic found, aborting");
    return;
  }
	DEBUG_SERIAL.printf("Sending value: %d\n", unitsTimesThousand);
  writeCharacteristic->setValue((uint32_t)unitsTimesThousand);
}
#endif //BT_COMMS_NIMBLE
