#include <LiquidCrystal.h>
#include <Glyphduino.h>
#include <ArduinoJson.h>
#include <Arduino.h>

//LiquidCrystal lcd(rs,en,d4,d5,d6,d7);
LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);
Glyphduino* glyph = new Glyphduino(&lcd);

const uint16_t BUTLERINO_SCROLLING_DELAY = 400;
const uint16_t BUTLERINO_DELAY_BETWEEN_REPS = 2000;

const int numChars = 256;

const int lcdWidth = 16;
const int lcdHeight = 2;
// int scrollCursor = lcdWidth;
// int stringStart, stringStop = 0;

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

void printGlyphs(){
	const size_t bufferSize = 2*JSON_OBJECT_SIZE(3) + 160;
	DynamicJsonBuffer jsonBuffer(bufferSize);
	JsonObject& root = jsonBuffer.parseObject(receivedChars);

  // Test if parsing succeeds.
  if (!root.success()) {
    Serial.println("error: parseObject() failed: " + receivedChars);
    return;
  }
	JsonObject& glyphs = root["glyphs"];

	for(int i = 0; i < lcdWidth; i++){
		lcd.setCursor(i, 1);
		if(glyphs[String(i)] == "?"){
			lcd.print("?");
		}
		else{
			int glyphId = glyphs[String(i)];
			if(glyphId > 0){
				// Serial.println("debug: " + String(glyphId));
				glyph->printGlyph(glyphId);
			}
		}
	}
}

void printStaticLine(){
	const size_t bufferSize = 2*JSON_OBJECT_SIZE(3) + 160;
	DynamicJsonBuffer jsonBuffer(bufferSize);
	JsonObject& root = jsonBuffer.parseObject(receivedChars);

  // Test if parsing succeeds.
  if (!root.success()) {
    Serial.println("error: parseObject() failed: " + receivedChars);
  }
	const char* staticLine = root["staticLine"];
	lcd.setCursor(0, 1);
	lcd.print(staticLine);
	printGlyphs();
}

String getScrollingLine(){
	const size_t bufferSize = 2*JSON_OBJECT_SIZE(3) + 160;
	DynamicJsonBuffer jsonBuffer(bufferSize);
	JsonObject& root = jsonBuffer.parseObject(receivedChars);

  // Test if parsing succeeds.
  if (!root.success()) {
    Serial.println("error: parseObject() failed: " + receivedChars);
    return;
  }
	String scrollingLine = root["scrollingLine"];
	return scrollingLine;
}

void clearLine(int line){
  for(int i = 0; i < lcdWidth; i++){
    lcd.setCursor(i, line);
    lcd.print(" ");
  }
}

void printScrollingLine(String scrollingLine, int reps){
  for(int j = 0; j < reps; j++){
    int i = 0;
    int stringStart = 0; int stringStop = 0;
    int scrollCursor = lcdWidth;
    while (i < scrollingLine.length() + 1){
      lcd.setCursor(scrollCursor, 0);
      lcd.print(scrollingLine.substring(stringStart, stringStop));
      lcd.setCursor(0, 1);
      printStaticLine();
      delay(BUTLERINO_SCROLLING_DELAY);
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
    delay(BUTLERINO_DELAY_BETWEEN_REPS);
    clearLine(0);
  }
  lcd.setCursor(0, 0);
  lcd.print(scrollingLine);
	printStaticLine();
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

String getStaticLine(){
	const size_t bufferSize = 2*JSON_OBJECT_SIZE(3) + 160;
	DynamicJsonBuffer jsonBuffer(bufferSize);
	JsonObject& root = jsonBuffer.parseObject(receivedChars);

  // Test if parsing succeeds.
  if (!root.success()) {
    Serial.println("error: parseObject() failed: " + receivedChars);
    return;
  }
	return root["staticLine"];
}

void setup() {
  Serial.begin(9600);
  Serial.println("<Arduino is ready>");
  lcd.begin(lcdWidth, lcdHeight);
	glyph->registerGlyph(0, GLYPHDUINO_GITHUB);
	glyph->registerGlyph(1, GLYPHDUINO_CELSIUS);
	glyph->registerGlyph(2, GLYPHDUINO_CLOUD);
  glyph->registerGlyph(3, GLYPHDUINO_RAIN);
  glyph->registerGlyph(3, GLYPHDUINO_SUN);
}

void loop() {
  int bytesReceived = recvWithEndMarker();
  if(bytesReceived > 0 && newData == true){
		newData = false;
		// showNewData();
		// printStaticLine();
		printScrollingLine(getScrollingLine(), 2);
		totalReceived = 0;
		receivedChars.remove(0);
	}
	ready();
}
