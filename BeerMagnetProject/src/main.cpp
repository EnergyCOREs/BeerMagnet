#include <Arduino.h>
#include <TM1637Display.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define CLK 16
#define DIO 5
#define BUZZER 4
#define BUZZER_FREQ 1000
OneWire oneWire(14); //data wire connected to GPIO0
DallasTemperature sensors(&oneWire);
TM1637Display display(CLK, DIO);

DeviceAddress beerThermometer;

bool tonePhase = false;
const uint8_t SEG_IGIP[] = {
    SEG_B | SEG_C,                         // I
    SEG_A | SEG_C | SEG_D | SEG_E | SEG_F, // G
    SEG_B | SEG_C,                         // I
    SEG_A | SEG_B | SEG_E | SEG_F | SEG_G, // P
};

const uint8_t SEG_to[] = {
    SEG_D | SEG_E | SEG_F | SEG_G, // t
    SEG_A | SEG_B | SEG_G | SEG_F, // o
    0,
    0,
};

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16)
      Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void setup()
{
  Serial.begin(9600);
  display.setBrightness(0x0f);
  display.setSegments(SEG_IGIP);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  sensors.begin();

  if (!sensors.getAddress(beerThermometer, 0))
    Serial.println("Unable to find address for Device 0");

  Serial.print("Device 0 Address: ");
  printAddress(beerThermometer);
  Serial.println();

  sensors.setLowAlarmTemp(beerThermometer, 5);
  sensors.setHighAlarmTemp(beerThermometer, 50);

  delay(500);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
}

void loop()
{
  delay(500);
  tonePhase = !tonePhase;
  sensors.requestTemperatures();
  int tempC = sensors.getTempC(beerThermometer);

  if (tempC > -9)
  {
    display.setSegments(SEG_to, 2, 0);
    display.showNumberDec(tempC, false, 2, 2);
  }
  else
  {
    display.setSegments(SEG_to, 1, 0);
    display.showNumberDec(tempC, false, 3, 1);
  }

  if (sensors.hasAlarm(beerThermometer))
  {
    analogWriteFreq(BUZZER_FREQ);
    analogWrite(BUZZER,tonePhase? BUZZER_FREQ/2:0);
    digitalWrite(LED_BUILTIN, tonePhase ? LOW : HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN, HIGH);
    analogWrite(BUZZER,0);
  }

  Serial.println(sensors.getTempC(beerThermometer));
}