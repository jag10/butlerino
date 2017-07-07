#include <LiquidCrystal.h>
#include <Glyphduino.h>
#include <Arduino.h>

//LiquidCrystal lcd(rs,en,d4,d5,d6,d7);
LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);
Glyphduino* glyph = new Glyphduino(&lcd);


int lcdWidth = 16;
int lcdHeight = 2;
String line1 = "Hey, I'm Devy. twat";
String line2 = "Devy   you"; // <3
String weather = "26";
int stringStart, stringStop = 0;
int scrollCursor = lcdWidth;

byte heart[8] = {
    B00000, B00000, B01010, B11111, B01110, B00100, B00000, B00000
};
byte cloud[8] = {
	B00000, B00110,	B01111, B11111, B00000, B00000,	B00000, B00000
};
byte sun[8] = {
	B10001,	B01010,	B00000,	B01110,	B01110,	B00000,	B01010,	B10001
};
byte celsius[8] = {
	B10000,	B01111,	B01000,	B01000,	B01000,	B01000,	B01000,	B01111
};

void printSymbol(int pos, byte symbol[]){
  lcd.createChar(0, symbol);
  lcd.setCursor(pos, 1);
  lcd.write((byte)0);
}

void scrollLine(String staticLine, String scrollingLine, int reps, int pos[], byte symbol[]){
  int i = 0;
  while (i < scrollingLine.length() * reps){
    lcd.setCursor(scrollCursor, 0);
    lcd.print(scrollingLine.substring(stringStart, stringStop));
    lcd.setCursor(0, 1);
    lcd.print(staticLine);
    for(int i = 0; i <= sizeof(pos)/sizeof(pos[0]); i++){
      printSymbol(pos[i], symbol);
    }
    // lcd.setCursor(0, 1);
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
    // lcd.clear();
  }
  delay(300);
  lcd.setCursor(0, 0);
  lcd.print(scrollingLine);
  // lcd.setCursor(0, 1);
  // lcd.print(staticLine);
  // for(int i = 0; i <= sizeof(pos)/sizeof(pos[0]); i++){
  //   printSymbol(pos[i], symbol);
  // }
}

/**
 * hello function: used as a Hello World at startup
 **/
void hello(int time, byte heart[]) {
  glyph->registerGlyph(0, GLYPHDUINO_HEART);
  lcd.setCursor(5, 0);
  lcd.print(F("Devy"));
  lcd.setCursor(6, 1);
  lcd.print(F("*-*"));

  lcd.setCursor(2, 0);
  glyph->printGlyph(GLYPHDUINO_HEART);
  lcd.setCursor(2, 1);
  glyph->printGlyph(GLYPHDUINO_HEART);

  lcd.setCursor(12, 0);
  glyph->printGlyph(GLYPHDUINO_HEART);
  lcd.setCursor(12, 1);
  glyph->printGlyph(GLYPHDUINO_HEART);

  delay(time);
  lcd.clear();
}


void setup() {
  Serial.begin(9600);
  lcd.begin(lcdWidth, lcdHeight);
  hello(5000, heart);
  int pos[2] = {5 ,6};
  byte symbols[2] = {heart, heart};
  scrollLine(line2, line1, 1, pos, heart);
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
