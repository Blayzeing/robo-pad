#ifndef CONFIG_OBJECT_H
#define CONFIG_OBJECT_H

#include <EEPROM.h>
#include <ESP8266WebServer.h>

class ConfigManager
{
  public:
    struct ConfigData
    {
      static const size_t CONFIG_STRING_LENGTH = 32;

      char robotName[CONFIG_STRING_LENGTH];        // Stores the name (also used as SSID of this robot)
      char password [CONFIG_STRING_LENGTH];         // Stores the robot's password
      uint8_t generalConfiguration; // bit 1    : Whether this device is in client mode
                                 // bit 2    : PWM input mode
                                 // bits 3,4 : PWM input pin
                                 // bit 5    : Weapon type (spinner or flipper)
                                 // bits 6,7 : Weapon pin
                                 // bit 8    : ???
      char gatewayName[CONFIG_STRING_LENGTH];      // If in client mode, the gateway this robopad will connect to
      char gatewayPassword[CONFIG_STRING_LENGTH];  // If in client mode, the gateway password this robopad will provide when connecting to the gateway
      uint8_t checkvalue1;       // Fletcher-16 byte 1
      uint8_t checkvalue2;       // Fletcher-16 byte 2

      void printToSerial();
      bool verify();

      private:
      static void fletcherProcessByte(const uint8_t inputByte, uint8_t &cByte1, uint8_t &cByte2);
      void generateCheckValues(uint8_t &checkValue1, uint8_t &checkValue2);
    };

    ConfigManager();
    ~ConfigManager();

    ConfigData configData;

    bool loadFromEEPROM(); // Returns false if the loaded data was eroneous
    void setDefault();     // Sets the loaded data to default values
    void save();           // Saves the current data into EEPROM, calculating and saving the checkvalues
    void writeToWifiClient(WiFiClient &wifiClient); // Writes a codified page to the given WiFiClient
    
  private:
    //static bool verifyData(); // Uses the loaded Fletcher-16 checkvalues to verify the integrity of the loaded data
    //static void fletcherProcessByte(uint8_t inputByte, uint8_t cByte1, uint8_t cByte2);

};

#endif
