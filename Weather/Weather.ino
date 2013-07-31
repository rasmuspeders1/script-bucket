#include <avr/pgmspace.h>

#include "DHT.h"
#include "U8glib.h"
 

#define DHTPIN 2
#define DHTTYPE DHT22

#define OLED_RESET 4


U8GLIB_ST7920_128X64 u8g(13, 11, 10, U8G_PIN_NONE);

DHT dht(DHTPIN, DHTTYPE);


float humidity;
float temperature;
void draw()
{
  u8g.setPrintPos(0, 0);
  u8g.print("Temperature: ");
  u8g.print(temperature,1);
  u8g.print((char) 176);
  u8g.print("C");
  
  u8g.setPrintPos(0, 8);
  u8g.print("Humidity:    ");
  u8g.print(humidity,1);
  u8g.print("%");

}

void setup()   {                  
  dht.begin();
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(5, 0);
  digitalWrite(6, 1);
  digitalWrite(7, 1);
  digitalWrite(8, 0);
  
  u8g.setFont(u8g_font_6x12);
  u8g.setFontRefHeightExtendedText();
  u8g.setDefaultForegroundColor();
  u8g.setFontPosTop();
  u8g.setColorIndex(1);
}
void loop() {
 
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  //Draw loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  delay(500);
  
}


