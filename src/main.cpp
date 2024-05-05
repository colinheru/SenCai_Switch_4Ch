#include <knx.h>
#include <knx/bau57B0.h>
#include <WiFiManager.h>
#define DEV_NAME "SenCai_Switch_4Ch"
// KNX Prog Hardware Define on the NodeMCU platform
#define PROG_BUTTON_PIN         0  //GPIO 0 --> Flash Button
#define PROG_LED_PIN            2  //GPIO 2 --> Built-in Led
#define PROG_LED_PIN_ACTIVE_ON  HIGH

uint8_t relayChCount = sizeof(relayChDef)/sizeof(relayChDef[0]);

// KNX Parameter List

// KNX Comm/Group Object List
#define Relay_KoOffset 0
#define Relay_KoBlockSize 2

// Create named references for easy access to group objects
#define groupObjChSwitch(Idx) (knx.getGroupObject(Relay_KoOffset + Relay_KoBlockSize * Idx + 1))
#define groupObjChStatus(Idx) (knx.getGroupObject(Relay_KoOffset + Relay_KoBlockSize * Idx + 2))

#define groupObjChASwitch groupObjChSwitch(CH_A)
#define groupObjChAStatus groupObjChStatus(CH_A)
#define groupObjChBSwitch groupObjChSwitch(CH_B)
#define groupObjChBStatus groupObjChStatus(CH_B)
#define groupObjChCSwitch groupObjChSwitch(CH_C)
#define groupObjChCStatus groupObjChStatus(CH_C)
#define groupObjChDSwitch groupObjChSwitch(CH_D)
#define groupObjChDStatus groupObjChStatus(CH_D)

// callback from switch-GO
void chASwitchCallback(GroupObject& go)
{
    Serial.println("group Obj ChA Switch");
    if (groupObjChABlock.value())
    {
        Serial.println("\tChA Block --> enabled");
        return;
    }
    
    bool value = groupObjChASwitch.value();
    digitalWrite(RELAY_CH_A, value);
    groupObjChAStatus.value(value);
}

void chBSwitchCallback(GroupObject& go)
{
    Serial.println("group Obj ChB Switch");
    if (groupObjChBBlock.value())
    {
        Serial.println("\tChB Block --> enabled");
        return;
    }
    
    bool value = groupObjChBSwitch.value();
    digitalWrite(RELAY_CH_B, value);
    groupObjChBStatus.value(value);
}

void chCSwitchCallback(GroupObject& go)
{
    Serial.println("group Obj ChC Switch");
    if (groupObjChCBlock.value())
    {
        Serial.println("\tChC Block --> enabled");
        return;
    }
    
    bool value = groupObjChCSwitch.value();
    digitalWrite(RELAY_CH_C, value);
    groupObjChCStatus.value(value);
}

void chDSwitchCallback(GroupObject& go)
{
    Serial.println("group Obj ChD Switch");
    if (groupObjChDBlock.value())
    {
        Serial.println("\tChD Block --> enabled");
        return;
    }
    
    bool value = groupObjChDSwitch.value();
    digitalWrite(RELAY_CH_D, value);
    groupObjChDStatus.value(value);
}

// callback previous to OTA-Update
void wifimgr_pre_ota() {
    knx.enabled(false);
}

WiFiManager wifiManager;
WiFiManagerParameter custom_field; // global param ( for non blocking w params )
char custom_field_str[200];
void setup()
{
    Serial.begin(115200);
    randomSeed(millis());
    ArduinoPlatform::SerialDebug = &Serial;

    delay(500);
    
    Serial.println();
    Serial.println("ESP32 Info.");
    Serial.println("****************************************");
  
    Serial.print("WiFi MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.printf("Chip ID as a 32-bit integer:\t%08X\n", ESP.getChipModel());  
    Serial.printf("Flash chip ID as a 32-bit integer:\t\t%08X\n", ESP.getFlashChipMode()); 
    Serial.printf("Flash chip frequency:\t\t\t\t%d (Hz)\n", ESP.getFlashChipSpeed());
    // ESP.getFlashChipSize() returns the flash chip size, in bytes as seen by the SDK (may be less than actual size).
    Serial.printf("Flash chip size:\t\t\t\t%d (bytes)\n", ESP.getFlashChipSize());
    Serial.printf("Free heap size:\t\t\t\t\t%d (bytes)\n", ESP.getFreeHeap());

    relayInit(relayChCount);

    wifiManager.setHostname(DEV_NAME);
    wifiManager.setClass("invert"); // Dark Mode
    wifiManager.setBreakAfterConfig(false);
    wifiManager.setTitle(DEV_NAME);
    wifiManager.setConfigPortalBlocking(false); //Not waiting for Connection/Configuration

    //Configure Menu 
    std::vector<const char *> menu = {"wifi","info","param","update","sep","restart", "erase"};
    wifiManager.setMenu(menu);

    // wifiManager.autoConnect(DEV_NAME);
    wifiManager.setTimeout(60000*8);
    if (!wifiManager.autoConnect(DEV_NAME)) {
        Serial.println("Failed to connect and hit timeout");
        delay(3000);
        // ESP.reset();
        delay(5000);
    }

    wifiManager.startWebPortal(); //Enable Webpage after initial Configuration
    wifiManager.setPreOtaUpdateCallback(wifimgr_pre_ota);

    // read adress table, association table, groupobject table and parameters from eeprom
    knx.readMemory();

    if (knx.configured())
    {
      Serial.println("Device is Configured!");
      
      groupObjChASwitch.callback(chASwitchCallback); // register callback for reset GO
      groupObjChASwitch.dataPointType(DPT_Switch);
      groupObjChAStatus.dataPointType(DPT_Bool);

      groupObjChBSwitch.callback(chBSwitchCallback);
      groupObjChBSwitch.dataPointType(DPT_Switch);
      groupObjChBStatus.dataPointType(DPT_Bool);

      groupObjChCSwitch.callback(chCSwitchCallback);
      groupObjChCSwitch.dataPointType(DPT_Switch);
      groupObjChCStatus.dataPointType(DPT_Bool);

      groupObjChDSwitch.callback(chDSwitchCallback);
      groupObjChDSwitch.dataPointType(DPT_Switch);
      groupObjChDStatus.dataPointType(DPT_Bool);
    } 
    else {
        Serial.println("Device is not Configured!");
    }
    
    // pin or GPIO the programming led is connected to. Default is LED_BUILTIN
    knx.ledPin(PROG_LED_PIN);
    // is the led active on HIGH or low? Default is LOW
    knx.ledPinActiveOn(PROG_LED_PIN_ACTIVE_ON);
    // pin or GPIO programming button is connected to. Default is 0
    knx.buttonPin(PROG_BUTTON_PIN);
    knx.setButtonISRFunction(NULL);

    // start the framework. Will get wifi first.
    knx.start();

    //Setup Page for detailed Information
    //show only programmed Physical Address
    uint16_t area = (knx.individualAddress() & 0xF000) >> 12;
    uint16_t line = (knx.individualAddress() & 0x0F00) >> 8;
    uint16_t device = knx.individualAddress() & 0x00FF;
    sprintf(custom_field_str,"<br/><label for='customfieldid'>Device is programmed with ETS</label><br>Physical-Address: %d.%d.%d", area, line, device);

    new (&custom_field) WiFiManagerParameter(custom_field_str); // custom html input
  
    wifiManager.addParameter(&custom_field);
}

void loop() 
{
    // don't delay here to much. Otherwise you might lose packages or mess up the timing with ETS
    knx.loop();
    wifiManager.process(); 
    knx.toggleProgMode();
    // do nothing
}

    // only run the application code if the device was configured with ETS
    if (!knx.configured())  return;

    // nothing else to do.
}
