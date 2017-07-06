#include <Arduino.h>

/*
 This sketch enables/disables interfaces set in ports[] array in a remote device using a Keypad Shield
 Author: Jeferson Arboleda Gómez
 Last Modified: June 2015
 */

#include <LiquidCrystal.h> // LCD control
#include <avr/pgmspace.h> //Library needed to store data into flash memory

//#include <MemoryFree.h>
#include <SPI.h>         //  needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <EthernetUdp.h> //  UDP library from: bjoern@cs.stanford.edu 12/30/2008

#include "_4_clienteSNMP_LCD.h"

//LiquidCrystal lcd(rs,en,d4,d5,d6,d7);
LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);



/**
 * readButton function: Read button states.
 **/
int readButton()
{
  adc_key_in = analogRead(0);      // leer el valor del sensor
  // ranges: 0, 144, 329, 504, 741
  if (adc_key_in > 1000) return btnNONE; // Most common response
  // For V1.1 us this threshold
  /*
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 250)  return btnUP;
  if (adc_key_in < 450)  return btnDOWN;
  if (adc_key_in < 650)  return btnLEFT;
  if (adc_key_in < 850)  return btnSELECT;
  */

  // Para V1.0 utilizar estos umbrales:

  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 195)  return btnUP;
  if (adc_key_in < 380)  return btnDOWN;
  if (adc_key_in < 555)  return btnLEFT;
  if (adc_key_in < 790)  return btnSELECT;

  return btnNONE;  // no button is pressed. This statement should'nt be reached
}

/**
 * hello function: used as a Hello World at startup
 **/
void hello(int time) {
  byte heart[8] = {
    B00000, B00000, B01010, B11111, B01110, B00100, B00000, B00000
  };

  lcd.createChar(0, heart);
  lcd.setCursor(4, 0);
  lcd.print(F("ARDUINO"));
  lcd.setCursor(5, 1);
  lcd.print(F("SNMP"));

  lcd.setCursor(2, 0);
  lcd.write((byte)0);
  lcd.setCursor(2, 1);
  lcd.write((byte)0);

  lcd.setCursor(12, 0);
  lcd.write((byte)0);
  lcd.setCursor(12, 1);
  lcd.write((byte)0);

  delay(time);

}


void setup() {
  Serial.begin(9600);
  Serial.print(F("Free Ram: "));
  Serial.println(F("Trying DHCP..."));
  lcd.begin(16, 2);
  hello(2000);
  //Disable the SD card
  //  pinMode (4, OUTPUT);                  //disable SD card slot on ethernet shield, due to SPI bus
  //  digitalWrite (4, HIGH);
  lcd.setCursor(0, 0);

  lcd.clear();
  lcd.print(F("press a button:"));

}

void loop()
{
 lcd.setCursor(9,1);            // move cursor to second line "1" and 9 spaces over
 lcd.print(millis()/1000);      // display seconds elapsed since power-up


 lcd.setCursor(0,1);            // move to the begining of the second line
 lcd_key = readButton();  // read the buttons

 switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
     {
     lcd.print("RIGHT ");
     Serial.print("RIGHT");
     break;
     }
   case btnLEFT:
     {
     lcd.print("LEFT   ");
     break;
     }
   case btnUP:
     {
     lcd.print("UP    ");
     break;
     }
   case btnDOWN:
     {
     lcd.print("DOWN  ");
     break;
     }
   case btnSELECT:
     {
     lcd.print("SELECT");
     break;
     }
     case btnNONE:
     {
     lcd.print("NONE  ");
     break;
     }
 }

 if (Serial.available()) {
    delay(100);  //wait some time for the data to fully be read
    lcd.clear();
    while (Serial.available() > 0) {
      char c = Serial.read();
      lcd.write(c);
    }
  }

}
