#include <avr/pgmspace.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

enum maintype
{
  typeMassage = 0,
  typeNusning
};

const char PROGMEM maintypestr[][8]= 
{
  "Massage",
  "Nusning"
};

const char PROGMEM locationstr[][6][12]=
{
  {"Skulder", "Ben", "Full Body", "Arme", "Ryg", "Fødder"},
  {"Numse", "Ben", "Full Body", "Arme", "Ryg", "Hår"}
};

static unsigned char PROGMEM heartbmp[] =
{ B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00111110, B00111110,
  B01111111, B01111111,
  B01111111, B11111111,
  B01111111, B11111111,
  B00111111, B11111110,
  B00011111, B11111100,
  B00000111, B11110000,
  B00000001, B11000000,
  B00000000, B10000000,
  B00000000, B00000000,
  B00000000, B00000000 };
  
char strbuf[32];

void displayString(char* str)
{
  uint8_t i=0;
  while(str[i] != 0)
  {
    display.write(str[i]);
    i++;
  }
}

void displayStringCenter(char* str)
{
  uint8_t strLen = strlen(str);
  int16_t x, y;
  display.getCursor(&x, &y);
  display.setCursor(x - display.getLeftTextOffset() - strLen*3, y);
  uint8_t i=0;
  while(str[i] != 0)
  {
    display.write(str[i]);
    i++;
  }
}

void setup()   {                
  Serial.begin(9600);
  display.begin();
  display.clearDisplay();
  //display.invertDisplay(true);
  
  display.drawRect(0, 0, display.width(), display.height(), WHITE);
  display.drawRect(1, 1, display.width()-2, display.height()-2, WHITE);
  display.display();
  
}

void displayFavor(maintype type, uint8_t location)
{
  display.fillRect(2, 2, display.width()-4, display.height()-4, BLACK);
  
  strcpy_P(strbuf, maintypestr[type]);
  display.setCursor(display.width()/2.0-2, 0);
  displayStringCenter(strbuf);
  strcpy_P(strbuf, locationstr[type][location]);
  display.setCursor(display.width()/2.0-2, 8);
  displayStringCenter(strbuf);
  display.display();
  //Seed the random generator with noise from unconnected analog input 0
  randomSeed(analogRead(0));
}

void loop() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setTextOffset(2);
  display.setRotation(0);
  displayFavor((maintype) random(2), random(6));
  display.drawBitmap(display.width()/2.0-8, 22, heartbmp, 16, 16, WHITE);
  display.display();
  delay(2000);
  
}


