#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>


#define LED_PIN 4

BLECharacteristic *pCharacteristic;
bool deviceConnect = false;
int txValue = 0;
//the server and characteristic( what its going to do when it connects to the client)
#define SERVICE_UUID          "3b212f37-8502-46a0-9de4-d8ddd8ac74a0"  
#define CHARACTERISTIC_UUID_RX "bec5bc7e-7f89-4694-95d4-f3413557609a"
#define CHARACTERISTIC_UUID_TX   "b334b468-2732-4bb6-ba6e-819cd70ff73a"
// test of the client has been successfully connected
class MyServerCallBack: public BLEServerCallbacks{
  void onConnect(BLEServer* pServer){
    deviceConnect = true;
  };

  void OnDisconnect(BLEServer* pServer){
    deviceConnect = false;
  }
};
// the start of receiving commands
class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0) {
            Serial.print("Received: ");
            for (int i = 0; i < value.length(); i++){
              Serial.print((uint8_t)value[i]);
              Serial.print(" ");
            }
            Serial.println();

            uint8_t command = (uint8_t)value[0];
            Serial.print("Interpreted as: ");
            Serial.println(command, DEC); // The commands received and what they do

            if (command == 1){
              Serial.println("Light ON");
              digitalWrite(LED_PIN, HIGH);
            }else if (command == 0) {
                Serial.println("Light OFF");
                digitalWrite(LED_PIN,LOW);
            }
            //the end of the command line
            Serial.println();
            Serial.println("END");
        }
    }
};

void setup() {
    Serial.begin(9600);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Initialize BLE
    BLEDevice::init("ESP32_BLE_Light");  // 


    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallBack());

    // Create the BLE Service using custom UUID
    BLEService *pService = pServer->createService(SERVICE_UUID);  
    
    // Create the BLE Characteristic using custom UUID
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TX, 
        BLECharacteristic::PROPERTY_NOTIFY
    );

    pCharacteristic->addDescriptor(new BLE2902());

    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,  // Custom Characteristic UUID
        BLECharacteristic::PROPERTY_WRITE
    );

        // Attach callback for write operations
    pCharacteristic->setCallbacks(new MyCallbacks());

    // Start the service
    pService->start();

    // Start advertising the service
    pServer->getAdvertising()-> start();
    Serial.println("BLE Ready. Connect and send '0' or '1' to control LED.");
}

void loop() {
  
}
