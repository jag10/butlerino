#include <LiquidCrystal.h>
#include <Glyphduino.h>
#include <ArduinoJson.h>
#include <Arduino.h>

//LiquidCrystal lcd(rs,en,d4,d5,d6,d7);
LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);
Glyphduino* glyph = new Glyphduino(&lcd);


int lcdWidth = 16;
int lcdHeight = 2;
String line1 = "Hey, I'm Devy. twat";
String line2 = "Devy    you"; // <3
String weather = "26";
int stringStart, stringStop = 0;
int scrollCursor = lcdWidth;

byte sun[8] = {
	B10001,	B01010,	B00000,	B01110,	B01110,	B00000,	B01010,	B10001
};


const int numChars = 256;
String receivedChars = "";   // an array to store the received data

boolean newData = false;
int totalReceived = 0;

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

/**
 * hello function: used as a Hello World at startup
 **/
void hello(int time, uint8_t glyphId) {
  glyph->registerGlyph(0, glyphId);
  lcd.setCursor(5, 0);
  lcd.print(F("Devy"));
  lcd.setCursor(6, 1);
  lcd.print(F("*-*"));

  lcd.setCursor(2, 0);
  glyph->printGlyph(glyphId);
  lcd.setCursor(2, 1);
  glyph->printGlyph(glyphId);

  lcd.setCursor(12, 0);
  glyph->printGlyph(glyphId);
  lcd.setCursor(12, 1);
  glyph->printGlyph(glyphId);

  delay(time);
  lcd.clear();
}

void printStaticLine(){
	const size_t bufferSize = 2*JSON_OBJECT_SIZE(3) + 160;
	DynamicJsonBuffer jsonBuffer(bufferSize);
	JsonObject& root = jsonBuffer.parseObject(receivedChars);

  // Test if parsing succeeds.
  if (!root.success()) {
    Serial.println("error: parseObject() failed: " + receivedChars);
		lcd.print("json error");
		lcd.print(bufferSize);
    return;
  }
	const char* staticLine = root["staticLine"];
	lcd.setCursor(0, 1);
	lcd.print(staticLine);

	totalReceived = 0;
	receivedChars.remove(0);
}

void scrollLine(String scrollingLine, int reps, int pos[], uint8_t glyphId){
  int i = 0;
  while (i < scrollingLine.length() * reps){
    lcd.setCursor(scrollCursor, 0);
    lcd.print(scrollingLine.substring(stringStart, stringStop));
    lcd.setCursor(0, 1);
    printStaticLine();
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
}

int recvWithEndMarker() {
  static int ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial.available() > 0 && newData == false) {
      rc = Serial.read();
      if (rc != endMarker) {
          receivedChars[ndx] = rc;
          receivedChars += rc;
          ndx++;
          if (ndx >= numChars) {
              ndx = numChars - 1;
          }
      }
      else {
          // receivedChars[ndx] = '\0'; // terminate the string
          // receivedChars += '\0';
          totalReceived = ndx;
          // ndx = 0;
          newData = true;
      }
  }
	return ndx;
}

void showNewData() {
  lcd.clear();
	Serial.println("error:" + receivedChars);
  for(int i = 0; i < receivedChars.length(); i++){
    lcd.setCursor(0, 0);
    lcd.print(receivedChars[i]);
    delay(300);
  }
}

void ready(){
	Serial.println("<Ready>");
}

void setup() {
  Serial.begin(9600);
  Serial.println("<Arduino is ready>");
  lcd.begin(lcdWidth, lcdHeight);
}

void loop() {
  int bytesReceived = recvWithEndMarker();
  if(bytesReceived > 0 && newData == true){
		newData = false;
		// showNewData();
		printStaticLine();
		totalReceived = 0;
		receivedChars.remove(0);
	}
	ready();
}
