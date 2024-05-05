#include <knx.h>
#include <WiFiManager.h>

#define DEV_NAME "SenCai_Switch_4Ch"
// KNX Prog Hardware Define on the NodeMCU platform
#define PROG_BUTTON_PIN         0  //GPIO 0 --> Flash Button
#define PROG_LED_PIN            2  //GPIO 2 --> Built-in Led
#define PROG_LED_PIN_ACTIVE_ON  HIGH



// KNX Parameter List

// KNX Comm/Group Object List


// Create named references for easy access to group objects



// callback from switch-GO
{
      // do nothing  
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
    // do nothing

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
   
    // do nothing
        
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
    // custom html input
  
    wifiManager.addParameter(&custom_field);
}

void loop() 
{
    knx.loop();
    wifiManager.process(); 
    knx.toggleProgMode();
    // do nothing
}

    // only run the application code if the device was configured with ETS
    if (!knx.configured())  return;

    // nothing else to do.
}
