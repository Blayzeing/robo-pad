#include "ConfigManager.h"

#define DEBUG

ConfigManager::ConfigManager()
{
  // Create the config object, initialize EEPROM
  EEPROM.begin(sizeof(ConfigManager::ConfigData));
}

ConfigManager::~ConfigManager()
{
  // Clear the EEPROM accesses
  EEPROM.end();
}

bool ConfigManager::loadFromEEPROM()
{
  // Read from EEPROM, populate data field
  EEPROM.get(0, configData);
  Serial.println(F("\tLOADED CONFIGURATION:"));
  ConfigManager::configData.printToSerial();

  bool verification = configData.verify();
  Serial.print(F("\tChecking validity...\t"));
  if(verification)
  {
    Serial.println(F("\tData successfully verified as valid!"));
  }else{
    Serial.println(F("\tError: Data invalid! Resetting to default..."));
    this->setDefault();
  }
  return verification;
}

void ConfigManager::setDefault()
{
  // Sets the config object to defaults
  char nameTemplate[] = "RoboPad-%s\0";
  sprintf(configData.robotName, "RoboPad-%s\0", WiFi.macAddress().c_str());
}

void ConfigManager::save()
{
  // Saves the current data into EEPROM, also computes the check bytes
}

void writeToWifiClient(WiFiClient &wifiClient)
{
  // Writes a codified page to the given wifiClient
}

// ------------------- ConfigData -------------------------------------

void ConfigManager::ConfigData::printToSerial()
{
    Serial.print(F("\tRobot name: "));
    Serial.println(robotName);
    Serial.print(F("\tRobot password: "));
    Serial.println(password);
    Serial.print(F("\tGeneral Configuration bits: "));
    Serial.println(generalConfiguration, BIN);
    Serial.print(F("\tGateway name: "));
    Serial.println(gatewayName);
    Serial.print(F("\tGateway password: "));
    Serial.println(gatewayPassword);
    Serial.print(F("\tCheck bytes: "));
    Serial.print(checkvalue1, HEX);
    Serial.print(" ");
    Serial.println(checkvalue2, HEX);
}

void ConfigManager::ConfigData::generateCheckValues(uint8_t &checkValue1, uint8_t &checkValue2)
{
  // Generates check values for this object
  // Start with some magic numbers to stop zero-initializations being considered valid
  checkValue1 = 42; 
  checkValue2 = 57;
  size_t i;
  for(i = 0; i<CONFIG_STRING_LENGTH; i++)
    fletcherProcessByte(robotName[i], checkValue1, checkValue2);
  for(i = 0; i<CONFIG_STRING_LENGTH; i++)
    fletcherProcessByte(password[i], checkValue1, checkValue2);
  fletcherProcessByte(generalConfiguration, checkValue1, checkValue2);
  for(i = 0; i<CONFIG_STRING_LENGTH; i++)
    fletcherProcessByte(gatewayName[i], checkValue1, checkValue2);
  for(i = 0; i<CONFIG_STRING_LENGTH; i++)
    fletcherProcessByte(gatewayPassword[i], checkValue1, checkValue2);
}

bool ConfigManager::ConfigData::verify()
{
  // Checks the data object against it's check values
  uint8_t checkValue1;
  uint8_t checkValue2;
  this->generateCheckValues(checkValue1, checkValue2);
  return checkValue1 == this->checkvalue1 && checkValue2 == this->checkvalue2;
}

void ConfigManager::ConfigData::fletcherProcessByte(const uint8_t inputByte, uint8_t &cByte1, uint8_t &cByte2)
{
  //Takes a byte from a stream and updates two check bytes with it
  // Note: relies on 8 bit integer overflow to modulo 255
  cByte1 = cByte1 + inputByte; // Increase input byte accumulator
  cByte2 = cByte2 + cByte1; // Increase check byte accumulator
}
