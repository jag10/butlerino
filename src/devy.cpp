#include <LiquidCrystal.h>
#include <Arduino.h>

//LiquidCrystal lcd(rs,en,d4,d5,d6,d7);
LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);

int lcdWidth = 16;
int lcdHeight = 2;
String line1 = "Hey, I'm Devy. I hope we can get on well";
String line2 = "Devy   you"; // <3
int stringStart, stringStop = 0;
int scrollCursor = lcdWidth;

byte heart[8] = {
    B00000, B00000, B01010, B11111, B01110, B00100, B00000, B00000
};

void printSymbol(int pos, byte symbol[]){
  lcd.createChar(0, symbol);
  lcd.setCursor(pos, 1);
  lcd.write((byte)0);
}

void scrollLine(String staticLine, String scrollingLine, int reps, int pos, byte symbol[]){
  int i = 0;
  while (i < scrollingLine.length() * reps){
    lcd.setCursor(scrollCursor, 0);
    lcd.print(scrollingLine.substring(stringStart, stringStop));
    lcd.setCursor(0, 1);
    lcd.print(staticLine);
    if(pos != -1){
      printSymbol(pos, symbol);
      lcd.setCursor(0, 1);
    }
    delay(300);
    if(stringStart == 0 && scrollCursor > 0){
      scrollCursor--;
      stringStop++;
    } else if (stringStart == stringStop){
      stringStart = stringStop = 0;
      scrollCursor = lcdWidth;
    } else if (stringStop == scrollingLine.length() && scrollCursor == 0) {
      stringStart++;
    } else {
      stringStart++;
      stringStop++;
    }
    i++;
    lcd.clear();
  }
  delay(300);
  lcd.setCursor(0, 0);
  lcd.print(scrollingLine);
  lcd.setCursor(0, 1);
  lcd.print(staticLine);
  if(pos != -1){
    printSymbol(pos, symbol);
    lcd.setCursor(0, 1);
  }
}

/**
 * hello function: used as a Hello World at startup
 **/
 //TODO: use printSymbol function
void hello(int time, byte heart[]) {
  lcd.createChar(0, heart);
  lcd.setCursor(5, 0);
  lcd.print(F("Devy"));
  lcd.setCursor(6, 1);
  lcd.print(F("*-*"));

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
  lcd.begin(lcdWidth, lcdHeight);
  hello(4000, heart);
  scrollLine(line2, line1, 1, 5, heart);
}

void loop() {
  if (Serial.available()) {
    lcd.clear();
    lcd.setCursor(16, 1);
    lcd.print(1);
    lcd.setCursor(16, 0);
    delay(100);  //wait some time for the data to fully be read
    // lcd.clear();
    lcd.autoscroll();
    while (Serial.available() > 0) {
      char c = Serial.read();
      lcd.print(c);
      delay(300);
    }
    lcd.noAutoscroll();
    // lcd.clear();
  }
}
