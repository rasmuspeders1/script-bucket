#include <avr/pgmspace.h>

#include "DHT.h"
#include "U8glib.h"
#include "Wire.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_BMP085.h"
#include "TimerOne.h"
//~ #include "VirtualWire.h"

#define DHTPIN 2
#define DHTTYPE DHT22

#define HISTORY_SIZE 40

U8GLIB_ST7920_128X64 u8g(12, 11, 10, U8G_PIN_NONE);

DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP085 bmp = Adafruit_BMP085(10085);
volatile int count = 1000;

bool remote = false;

volatile float temperature;
volatile float temperature_min;
volatile float temperature_max;
volatile float temperature_history[HISTORY_SIZE];
volatile float humidity;
volatile float humidity_min;
volatile float humidity_max;
volatile float humidity_history[HISTORY_SIZE];
volatile float pressure;
volatile float pressure_min;
volatile float pressure_max;
volatile float pressure_history[HISTORY_SIZE];

volatile float remote_temperature;
volatile float remote_temperature_min;
volatile float remote_temperature_max;
volatile float remote_temperature_history[HISTORY_SIZE];
volatile float remote_humidity;
volatile float remote_humidity_min;
volatile float remote_humidity_max;
volatile float remote_humidity_history[HISTORY_SIZE];


void draw()
{
  noInterrupts();
  u8g.setPrintPos(0, 0);
  if(remote)
  {
    u8g.print(remote_temperature,1);
  }
  else
  {
    u8g.print(temperature,1);
  }
  u8g.print(" ");
  u8g.print((char) 176);
  u8g.print("C");
  u8g.drawLine(86, 1, 86, 18);
  u8g.drawHLine(86, 19, HISTORY_SIZE+1);

  u8g.drawHLine(0, 21, 128);
  u8g.setPrintPos(0, 22);
  if(remote)
  {
    u8g.print(remote_humidity,1);
  }
  else
  {
    u8g.print(humidity,1);
  }
  u8g.print(" %");
  u8g.drawLine(86, 23, 86, 40);
  u8g.drawHLine(86, 41, HISTORY_SIZE+1);
  
  u8g.drawHLine(0, 43, 128);
  u8g.setPrintPos(0, 44);
  u8g.print(pressure / 100.0,2);
  u8g.setFont(u8g_font_helvB08);
  u8g.setFontRefHeightExtendedText();
  u8g.setPrintPos(86, 64);
  u8g.print("hPa");
  u8g.drawLine(86, 45, 86, 53);
  u8g.drawHLine(86, 54, HISTORY_SIZE+1);

  if(remote)
  {
    for(unsigned int i = 0; i < HISTORY_SIZE; ++i)
    {
      u8g.drawPixel(87 + i, 18 - (remote_temperature_history[i] - remote_temperature_min)*(17)/(remote_temperature_max - remote_temperature_min));
    }

    for(unsigned int i = 0; i < HISTORY_SIZE; ++i)
    {
      u8g.drawPixel(87 + i, 40 - (remote_humidity_history[i] - remote_humidity_min)*(17)/(remote_humidity_max - remote_humidity_min));
    }
  }
  else
  {
    for(unsigned int i = 0; i < HISTORY_SIZE; ++i)
    {
      u8g.drawPixel(87 + i, 18 - (temperature_history[i] - temperature_min)*(17)/(temperature_max - temperature_min));
    }

    for(unsigned int i = 0; i < HISTORY_SIZE; ++i)
    {
      u8g.drawPixel(87 + i, 40 - (humidity_history[i] - humidity_min)*(17)/(humidity_max - humidity_min));
    }
  }
  

  
  for(unsigned int i = 0; i < HISTORY_SIZE; ++i)
  {
    u8g.drawPixel(87 + i, 53 - (pressure_history[i] - pressure_min)*(8)/(pressure_max - pressure_min));
  }

  u8g.setFont(u8g_font_helvB18);
  u8g.setFontRefHeightExtendedText();
  u8g.setFontPosTop();
  interrupts();
}

void setup()   {
  dht.begin();
  bmp.begin();
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(5, 0);
  analogWrite(6, 220);
  digitalWrite(7, 1);
  digitalWrite(8, 0);
  
  // Initialise the IO and ISR
  //~ vw_set_rx_pin(9);
  //~ vw_setup(2000);	 // Bits per sec
  //~ vw_rx_start();       // Start the receiver PLL running
  
  do
  {
    temperature = dht.readTemperature();  
  }while(isnan(temperature));
  for(unsigned int i = 0; i < HISTORY_SIZE; ++i)
  {
    temperature_history[i] = temperature;
  }
  
  humidity = dht.readHumidity();
  for(unsigned int i = 0; i < HISTORY_SIZE; ++i)
  {
    humidity_history[i] = humidity;
  }
  
  float p;
  bmp.getPressure(&p);
  pressure = p;
  for(unsigned int i = 0; i < HISTORY_SIZE; ++i)
  {
    pressure_history[i] = pressure;
  }

  u8g.setFont(u8g_font_helvB18);
  u8g.setFontRefHeightExtendedText();
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
  u8g.setColorIndex(1);
  
  save_values();
  
  Timer1.initialize(3600000);
  Timer1.attachInterrupt(save_values);

}
void loop() {
  do
  {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
  }while(isnan(temperature) || isnan(humidity));
  
  float p;
  bmp.getPressure(&p);
  pressure = p;
  
  //Draw loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  delay(500);
  //~ uint8_t buf[VW_MAX_MESSAGE_LEN];
  //~ uint8_t buflen = VW_MAX_MESSAGE_LEN;
//~ 
  //~ if (vw_get_message(buf, &buflen)) // Non-blocking
  //~ {
    //~ if(buflen >= 9)
    //~ {
    //~ char tmpstr[5] = {'1', '2', '3', '4'};
    //~ memcpy(tmpstr, buf, 4);
    //~ remote_temperature = atoi(tmpstr);
    //~ remote_temperature = remote_temperature / 100.0;
    //~ memcpy(tmpstr, buf+4, 4);
    //~ remote_humidity = atoi(tmpstr);
    //~ remote_humidity = remote_humidity / 100.0;
    //~ int i;
//~ 
    //~ digitalWrite(13, true); // Flash a light to show received good message
    //~ // Message with a good checksum received, dump it.
    //~ Serial.print("Got: ");
//~ 
    //~ for (i = 0; i < buflen; i++)
    //~ {
      //~ Serial.print(buf[i], HEX);
      //~ Serial.print(" ");
    //~ }
      //~ Serial.println("");
      //~ digitalWrite(13, false);
    //~ }
  //~ }
  
}

void save_values()
{
  count++;
  if(count>1000/HISTORY_SIZE)
  {
    count = 0;
    temperature_min = temperature;
    temperature_max = temperature;
    humidity_min = humidity;
    humidity_max = humidity;
    remote_temperature_min = temperature;
    remote_temperature_max = temperature;
    remote_humidity_min = humidity;
    remote_humidity_max = humidity;
    pressure_min = pressure;
    pressure_max = pressure;
    for(unsigned int i = 0; i < HISTORY_SIZE - 1 ; ++i)
    {
      if(temperature_history[i] < temperature_min) temperature_min = temperature_history[i];
      if(temperature_history[i] > temperature_max) temperature_max = temperature_history[i];
      temperature_history[i] = temperature_history[i+1];
      
      if(humidity_history[i] < humidity_min) humidity_min = humidity_history[i];
      if(humidity_history[i] > humidity_max) humidity_max = humidity_history[i];
      humidity_history[i] = humidity_history[i+1];
      
      if(remote_temperature_history[i] < remote_temperature_min) remote_temperature_min = remote_temperature_history[i];
      if(remote_temperature_history[i] > remote_temperature_max) remote_temperature_max = remote_temperature_history[i];
      remote_temperature_history[i] = remote_temperature_history[i+1];
      
      if(remote_humidity_history[i] < remote_humidity_min) remote_humidity_min = remote_humidity_history[i];
      if(remote_humidity_history[i] > remote_humidity_max) remote_humidity_max = remote_humidity_history[i];
      remote_humidity_history[i] = remote_humidity_history[i+1];
      
      if(pressure_history[i] < pressure_min) pressure_min = pressure_history[i];
      if(pressure_history[i] > pressure_max) pressure_max = pressure_history[i];
      pressure_history[i] = pressure_history[i+1];
    }
    temperature_history[HISTORY_SIZE - 1] = temperature;
    humidity_history[HISTORY_SIZE - 1] = humidity;
    remote_temperature_history[HISTORY_SIZE - 1] = temperature;
    remote_humidity_history[HISTORY_SIZE - 1] = humidity;
    pressure_history[HISTORY_SIZE - 1] = pressure;
    
    if(temperature_max - temperature_min < 1.0)
    {
      temperature_min = temperature - 0.5;
      temperature_max = temperature + 0.5;
    }
    if(humidity_max - humidity_min < 1.0)
    {
      humidity_min = humidity - 0.5;
      humidity_max = humidity + 0.5;
    }
    if(remote_temperature_max - remote_temperature_min < 1.0)
    {
      remote_temperature_min = remote_temperature - 0.5;
      remote_temperature_max = remote_temperature + 0.5;
    }
    if(remote_humidity_max - remote_humidity_min < 1.0)
    {
      remote_humidity_min = remote_humidity - 0.5;
      remote_humidity_max = remote_humidity + 0.5;
    }
    if(pressure_max - pressure_min < 20.0)
    {
      pressure_min = pressure - 10;
      pressure_max = pressure + 10;
    }
  }
}
