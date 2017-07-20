#include <LiquidCrystal.h>
#include <Glyphduino.h>
#include <ArduinoJson.h>
#include <Arduino.h>

//LiquidCrystal lcd(rs,en,d4,d5,d6,d7);
LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);
Glyphduino* glyph = new Glyphduino(&lcd);

const uint16_t BUTLERINO_SCROLLING_DELAY = 400;
const uint16_t BUTLERINO_DELAY_BETWEEN_REPS = 2000;

const int numChars = 512;

const int lcdWidth = 16;
const int lcdHeight = 2;

String receivedChars = "";   // an array to store the received data
boolean newData = false;
int totalReceived = 0;

/**
    Gets free ram on Arduino

    @return Free ram on Arduino
*/
int freeRam();

/**
    Clears a line on lcd

    @param line The line to clear
    @return void
*/
void clearLine(int line);

/**
    Prints glyphs from json string received on receivedChars

    @return void
*/
void printGlyphs();

/**
    Gets staticLine from json string received on receivedChars

    @return The string for the static line
*/
String getStaticLine();

/**
    Gets scrollingLine from json string received on receivedChars

    @return The string for the static line
*/
String getScrollingLine();

/**
    Prints staticLine with glyphs from json string received on receivedChars

    @return void
*/
void printStaticLine();

/**
    Prints scrollingLine for 'reps' times

    @param scrollingLine The scrolling line
    @param reps The number of repetitions for the scrolling line
    @return void
*/
void printScrollingLine(String scrollingLine, int reps);

/**
    Receives a string with and End Marker and saves it to 'receivedChars'

    @return The number of characters read
*/
int recvWithEndMarker();

/**
    Sends a string over Serial so butler knows Arduino is ready to
    receive data

    @return void
*/
void ready();

void setup() {
  Serial.begin(9600);
  Serial.println(F("<Arduino is ready>"));
  lcd.begin(lcdWidth, lcdHeight);
	glyph->registerGlyph(0, GLYPHDUINO_GITHUB);
	glyph->registerGlyph(1, GLYPHDUINO_CELSIUS);
	glyph->registerGlyph(2, GLYPHDUINO_CLOUD);
  glyph->registerGlyph(3, GLYPHDUINO_RAIN);
  glyph->registerGlyph(4, GLYPHDUINO_SUN);
  glyph->registerGlyph(5, GLYPHDUINO_BELL);
}

void loop() {
  int bytesReceived = recvWithEndMarker();
  if(bytesReceived > 0 && newData == true){
		newData = false;
    Serial.println("debug: " + receivedChars);
		printScrollingLine(getScrollingLine(), 2);
		totalReceived = 0;
		receivedChars.remove(0);
	}
	ready();
}

// Gets free ram on Arduino
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

// Clears a line on lcd
void clearLine(int line){
  for(int i = 0; i < lcdWidth; i++){
    lcd.setCursor(i, line);
    lcd.print(F(" "));
  }
}


// Prints glyphs from json string received on receivedChars
void printGlyphs(){
	const size_t bufferSize = 2*JSON_OBJECT_SIZE(3) + 160;
	DynamicJsonBuffer jsonBuffer(bufferSize);
	JsonObject& root = jsonBuffer.parseObject(receivedChars);

  // Test if parsing succeeds.
  if (!root.success()) {
    Serial.println(F("error: parseObject() failed: "));
    Serial.println(receivedChars);
    return;
  }
	JsonObject& glyphs = root["glyphs"];

	for(int i = 0; i < lcdWidth; i++){
		lcd.setCursor(i, 1);
		if(glyphs[String(i)] == "?"){ // Unknown glyphs
			lcd.print(F("?"));
		}
		else{
			int glyphId = glyphs[String(i)];
			if(glyphId > 0){
				glyph->printGlyph(glyphId);
			}
		}
	}
}

// Gets scrollingLine from json string received on receivedChars
String getStaticLine(){
	const size_t bufferSize = 2*JSON_OBJECT_SIZE(3) + 160;
	DynamicJsonBuffer jsonBuffer(bufferSize);
	JsonObject& root = jsonBuffer.parseObject(receivedChars);

  // Test if parsing succeeds.
  if (!root.success()) {
    Serial.println(F("error: parseObject() failed: "));
    Serial.println(receivedChars);
    return "error";
  }
	return root["staticLine"];
}

// Gets scrollingLine from json string received on receivedChars
String getScrollingLine(){
  const size_t bufferSize = JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + 190;
	DynamicJsonBuffer jsonBuffer(bufferSize);
	JsonObject& root = jsonBuffer.parseObject(receivedChars);

  // Test if parsing succeeds.
  if (!root.success()) {
    Serial.println(F("error: parseObject() failed: "));
    Serial.println(receivedChars);
    return "error";
  }
	String scrollingLine = root["scrollingLine"];
	return scrollingLine;
}

// Prints staticLine with glyphs from json string received on receivedChars
void printStaticLine(){
	lcd.print(getStaticLine());
	printGlyphs();
}

// Prints scrollingLine for 'reps' times
void printScrollingLine(String scrollingLine, int reps){
  for(int j = 0; j < reps; j++){
    uint16_t i = 0;
    uint16_t stringStart = 0; uint16_t stringStop = 0;
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
    }
    delay(BUTLERINO_DELAY_BETWEEN_REPS);
    clearLine(0);
  }
  lcd.setCursor(0, 0);
  lcd.print(scrollingLine.substring(0,16));
	printStaticLine();
}

// Receives a string with and End Marker and saves it to 'receivedChars'
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

// Sends a string over Serial so butler knows Arduino is ready to receive data
void ready(){
	Serial.println(F("<Ready>"));
}
