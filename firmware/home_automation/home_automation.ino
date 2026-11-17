/*
  STAGE 11 (ESP RAINMAKER): Native Cloud & Google Home Integration
  Features: QR Code Setup, SoftAP, Relays, Buttons, IR Remote, Free Cloud
  
  CRITICAL: You MUST go to Tools -> Partition Scheme and select "RainMaker" 
  or "Huge APP" before uploading!
*/

#include <Arduino.h>
#include <RMaker.h>
#include <WiFi.h>
#include <WiFiProv.h>
#include <IRremote.hpp>

// --- PROVISIONING CREDENTIALS ---
const char *service_name = "PROV_SMARTHOME";
const char *pop = "REPLACE_WITH_PRIVATE_POP"; // Set your unique private provisioning code before flashing

// --- HARDWARE CONFIGURATION ---
const int NUM_DEVICES = 8;
const int relayPins[NUM_DEVICES] = {4, 5, 6, 7, 15, 16, 17, 18};
const int RELAY_ON = LOW; 
const int RELAY_OFF = HIGH;
const int buttonPins[NUM_DEVICES] = {1, 2, 8, 9, 10, 11, 12, 13};
const int IR_RECEIVE_PIN = 14;

// The BOOT button. Hold for 3 seconds to reset Wi-Fi & RainMaker credentials!
const int RESET_BUTTON = 0; 

// --- DEVICE NAMES ---
// These names will appear in the RainMaker and Google Home apps
String deviceNames[NUM_DEVICES] = {
  "Light 1", "Light 2", "Light 3", "Light 4", 
  "Light 5", "Light 6", "Light 7", "Light 8"
};

// --- STATE TRACKING ---
bool deviceState[NUM_DEVICES] = {false, false, false, false, false, false, false, false};
int lastButtonState[NUM_DEVICES] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
int buttonState[NUM_DEVICES]     = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
unsigned long lastDebounceTime[NUM_DEVICES] = {0, 0, 0, 0, 0, 0, 0, 0};
const unsigned long debounceDelay = 50; 

// --- RAINMAKER OBJECTS ---
Node my_node;
Switch* mySwitches[NUM_DEVICES];

// --- RAINMAKER CLOUD CALLBACK ---
// This fires instantly when you toggle a switch in RainMaker or Google Home
void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx) {
  const char *device_name = device->getDeviceName();
  const char *param_name = param->getParamName();

  // If the command is a Power ON/OFF command
  if (strcmp(param_name, "Power") == 0) {
    bool state = val.val.b;
    
    // Find which light was triggered
    for (int i = 0; i < NUM_DEVICES; i++) {
      if (strcmp(device_name, deviceNames[i].c_str()) == 0) {
        
        // 1. Update our local state
        deviceState[i] = state;
        
        // 2. Trigger the physical relay
        digitalWrite(relayPins[i], state ? RELAY_ON : RELAY_OFF);
        
        Serial.printf("[CLOUD] %s turned %s\n", device_name, state ? "ON" : "OFF");
        
        // 3. Confirm the change back to the cloud
        param->updateAndReport(val);
        break;
      }
    }
  }
}

// --- LOCAL DEVICE CONTROL ---
void toggleDevice(int index) {
  if (index < 0 || index >= NUM_DEVICES) return; 
  
  // 1. Toggle local state
  deviceState[index] = !deviceState[index];
  
  // 2. Trigger physical relay
  digitalWrite(relayPins[index], deviceState[index] ? RELAY_ON : RELAY_OFF);
  Serial.printf("[LOCAL] %s turned %s\n", deviceNames[index].c_str(), deviceState[index] ? "ON" : "OFF");

  // 3. Sync the new state instantly to the RainMaker App & Google Home!
  mySwitches[index]->updateAndReportParam("Power", deviceState[index]);
}

// --- INPUT FUNCTIONS ---
void updateButtons() {
  unsigned long currentMillis = millis();
  for (int i = 0; i < NUM_DEVICES; i++) {
    int reading = digitalRead(buttonPins[i]);
    if (reading != lastButtonState[i]) lastDebounceTime[i] = currentMillis; 
    
    if ((currentMillis - lastDebounceTime[i]) > debounceDelay) {
      if (reading != buttonState[i]) {
        buttonState[i] = reading;
        if (buttonState[i] == LOW) {
          Serial.print("[BUTTON] ");
          toggleDevice(i);
        }
      }
    }
    lastButtonState[i] = reading;
  }
}

void updateIR() {
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.protocol == NEC) {
      if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
        uint16_t command = IrReceiver.decodedIRData.command;
        Serial.print("[IR REMOTE] -> ");
        switch (command) {
          case 0x92: toggleDevice(0); break;
          case 0x93: toggleDevice(1); break;
          case 0xCC: toggleDevice(2); break;
          case 0x8E: toggleDevice(3); break;
          case 0x8F: toggleDevice(4); break;
          case 0xC8: toggleDevice(5); break;
          case 0x8A: toggleDevice(6); break;
          case 0x8B: toggleDevice(7); break;
        }
      }
    }
    IrReceiver.resume(); 
  }
}

void checkResetButton() {
  if (digitalRead(RESET_BUTTON) == LOW) {
    Serial.print("\n[SYSTEM] Reset button held. Keep holding");
    
    // Require holding for 30 loops (about 3 seconds)
    for(int i = 0; i < 30; i++) {
       delay(100);
       Serial.print(".");
       if(digitalRead(RESET_BUTTON) == HIGH) {
          Serial.println(" Cancelled.");
          return;
       }
    }
    
    Serial.println("\n[SUCCESS] Memory Wiped! Rebooting into Setup Mode...");
    RMakerFactoryReset(2); // Erase memory and reboot in 2 seconds
  }
}

bool irStarted = false;

// --- MAIN SETUP & LOOP ---
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n==============================================");
  Serial.println("   FINAL STAGE: ESP RAINMAKER (NATIVE CLOUD)");
  Serial.println("==============================================");

  Serial.println("[STEP 1] Initializing Hardware...");
  for (int i = 0; i < NUM_DEVICES; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], RELAY_OFF); 
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(RESET_BUTTON, INPUT_PULLUP);
  
  Serial.println("[STEP 2] Setting up Wi-Fi mode...");
  WiFi.onEvent([](arduino_event_id_t event, arduino_event_info_t info){}); 
  WiFi.mode(WIFI_STA);

  Serial.println("[STEP 3] Initializing RainMaker Node...");
  my_node = RMaker.initNode("ESP32 Smart Home");

  Serial.println("[STEP 4] Creating Virtual Switches...");
  for(int i = 0; i < NUM_DEVICES; i++) {
    mySwitches[i] = new Switch(deviceNames[i].c_str(), NULL);
    mySwitches[i]->addCb(write_callback);  
    my_node.addDevice(*mySwitches[i]);     
  }

  Serial.println("[STEP 5] Starting RainMaker Services...");
  RMaker.enableOTA(OTA_USING_PARAMS); 
  RMaker.enableTZService();           
  RMaker.enableSchedule();            
  RMaker.start();

  Serial.println("[STEP 6] Starting SoftAP Provisioning...");
  // Using SoftAP to bypass the broken Bluetooth radio entirely
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name);
  
  Serial.println("\n=======================================================");
  Serial.println("             --- PROVISIONING INSTRUCTIONS ---");
  Serial.println("   1. Open the ESP RainMaker App");
  Serial.println("   2. Click 'Add Device'");
  Serial.println("   3. Click 'I don't have a QR code' -> 'Provision via SoftAP'");
  Serial.println("   4. Connect your phone to the Wi-Fi: PROV_SMARTHOME");
  Serial.printf("   5. Enter the POP PIN: %s\n", pop);
  Serial.println("=======================================================\n");
  
  Serial.println("\n=======================================================");
  Serial.println("[STEP 7] Setup complete! Waiting for Wi-Fi connection...");
}

void loop() {
  // Check physical buttons and reset button immediately
  updateButtons();
  checkResetButton();

  // Only start the IR receiver AFTER provisioning finishes to prevent hardware timer clashes!
  if (WiFi.status() == WL_CONNECTED) {
    if (!irStarted) {
      Serial.println("\n[SYSTEM] Wi-Fi Connected! Activating IR Receiver...");
      IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);
      irStarted = true;
    }
    updateIR();
  }
}