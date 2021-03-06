#include "SevenSegmentTM1637.h"
#include <Encoder.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SdFat.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_ImageReader.h>

Encoder myEnc(6, 7);
#define PIXEL_PIN 8
#define NUM_PIXELS 19
#define speakerPin 2
#define incorrectBuzzFreq 100
const byte PIN_CLK = 4 ;
const byte PIN_DIO = 5;
const byte Section1Button = 9;
const byte Section2Button = 10;
const byte Section3Button = 11;
const byte GridPins[4][4] = {{25,24,23,22},{26,27,28,29}};
const byte BinaryPins[8] = {30,31,32,33,34,35,36,37};
const byte JackPins[10] = {38,39,40,41,42,43,44,45,46,47};
const byte SDCardSSPin = 48;
const byte TFTScreenSSPin = 53;
const String Space = " ";
const byte ColorNums[] = {7,3,4,5,6,2,3,4,3,5,3,2};
const byte ProfileConversion[][7] = {
  {0,1,2,3,4,5,6},
  {0,1,4,0,0,0,0},
  {0,1,3,4,0,0,0},
  {0,1,2,3,4,0,0},
  {0,1,2,3,4,5,0},
  {0,4,0,0,0,0,0},
  {0,1,6,0,0,0,0},
  {0,1,4,6,0,0,0},
  {1,3,6,0,0,0,0},
  {0,1,3,4,5,0,0},
  {0,1,2,0,0,0,0},
  {4,6,0,0,0,0,0}};
const byte Colors[7][3] = {
  {0,0,0},
  {255,0,0},
  {255,255,0},
  {0,255,0},
  {0,255,255},
  {179,120,211},
  {255,255,255}
};
const byte buttonNums[4][4] = { 
  {13,9,5,1},
  {14,10,6,2},
  {15,11,7,3},
  {16,12,8,4}
};


ImageReturnCode stat;
byte  buttonValues[4][4] = {
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0}
};
byte prev[4][4] = {
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0}
};
int jackConnections[5][2] = {{0,0},{0,0},{0,0},{0,0},{0,0}};
String lines;
String inString;
int num;
int rotary;
int connections[5][2];
int colorProfile;
int startColors[16];
int endColors[16];
String result;
String filePrefix;
String ext = ".txt";
String picext = ".bmp";
String A = "a";
String B = "b";
String picPath = "/Pictures/";
byte tempScan;
byte x;
byte y;
byte i;
byte count;
long counter;
byte switches[8] = {0,0,0,0,0,0,0,0};
long oldPos = 0;
long pos = 0;
long newPos = 0;
long oldnewPos = 0;
short dialerValue = 0;
 byte section = 0;
byte lookingAt;
bool opened = false;

#define USE_SD_CARD
#define SD_CS   48 // SD card select pin
#define TFT_CS 53 // TFT select pin
#define TFT_DC  49 // TFT display/command pin

SdFat                SD;         // SD card filesystem
Adafruit_ImageReader reader(SD); // Image-reader object, pass in SD filesys

Adafruit_ILI9341     tft    = Adafruit_ILI9341(TFT_CS, TFT_DC);
Adafruit_Image       img;        // An image loaded into RAM
int32_t              width  = 0, // BMP image dimensions
                     height = 0;


SevenSegmentTM1637    display(PIN_CLK, PIN_DIO);
Adafruit_NeoPixel pixels(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
File myFile;
void setPinModes(){
  pinMode(speakerPin, OUTPUT);
  pinMode(Section1Button, INPUT_PULLUP);
  pinMode(Section2Button, INPUT_PULLUP);
  pinMode(Section3Button, INPUT_PULLUP);
  for(i = 0; i < 4; i++){
    pinMode(GridPins[0][i], OUTPUT);
  }
  for(i = 0; i < 4; i++){
    pinMode(GridPins[1][i], INPUT_PULLUP);
  }
  for(i = 0; i < 10; i++){
    pinMode(JackPins[i], INPUT_PULLUP);
  }
  for(i = 0; i < 8; i++){
    pinMode(BinaryPins[i], INPUT_PULLUP);
  }
  
}
void setup(void) {
  // put your setup code here, to run once:
  setPinModes();
  Serial.begin(9600);
  Serial.println("00000000000");
  display.begin();
  display.setBacklight(100);
  display.print("INIT");
  pixels.begin();
  pixels.clear();
  pixels.setBrightness(200);
  pixels.show();
  while(!Serial);
  tft.begin();
  if(!SD.begin(SD_CS, SD_SCK_MHZ(25))) {
    Serial.println(F("SD begin() failed"));
    display.print("NOSD");
    for(;;);
  }
  stat = reader.drawBMP("/Pictures/logo.bmp", tft, 0, 0);
  display.clear();
  oldPos = myEnc.read();
  result = Space + Space + Space + newPos;
  display.print(result);
  
}

void loop() {
  if(section == 0){
    section1();
  }
  else if(section == 1){
    section2();
  }
  else if(section == 2){
    section3();
  }
  else{
    Serial.println("Oh Poop, section variable got corrupted");
  }
}
void incorrect(byte section) {
  sendUpdate(false);
  for(i = 0; i < 3; i++){
    tone(speakerPin, incorrectBuzzFreq);
    pixels.setPixelColor(15 + section, pixels.Color(255, 0, 0));
    pixels.show();
    delay(750);
    noTone(speakerPin);
    pixels.setPixelColor(15 + section, pixels.Color(0, 0, 0));
    pixels.show();
    delay(750);
  }
}
void section1() {
  tempScan = digitalRead(Section1Button);
  if(tempScan == 0){
    pixels.setPixelColor(16, pixels.Color(255, 255, 255));
    pixels.show();
    for(i = 0; i < 8; i++){
      switches[i] = digitalRead(BinaryPins[i]);
    }
    count = 0;
    for(i = 0; i < 8; i++){
      if(switches[i] == 0){
        count += round(pow(2,i));
      }
    }
    readFile();
    if(opened){
      if(rotary == newPos){
        pixels.setPixelColor(16, pixels.Color(0, 255, 0));
        pixels.show();
        tone(speakerPin, 784);
        delay(100);
        tone(speakerPin, 1046);
        delay(200);
        noTone(speakerPin);
        filePrefix = count;
        sendUpdate(true);
        section = 1;
        result = picPath + filePrefix + A + picext;
        char file[result.length() + 1];
        strcpy(file, result.c_str()); 
        stat = reader.drawBMP(file, tft, 0, 0);
        opened = false;
      }
      else{
        incorrect(1);
      }
    }
    else{
      delay(928);
      incorrect(1);
    }
    
  }
  pos = myEnc.read();
  counter = counter + pos - oldPos;
  if(counter >= 4){
    newPos += 1;
    counter -= 4;
  }
  else if(counter <= -4){
    newPos -= 1;
    counter += 4;
  }
  if(newPos < 0){
    newPos = 0;
  }
  else if(newPos > 255){
    newPos = 255;
  }
  if (newPos != oldnewPos) {
    oldnewPos = newPos;
    if(newPos < 10){
      result = Space + Space + Space + newPos;
    }
    else if(newPos<100){
      result = Space + Space + newPos;
    }
    else {
      result = Space + newPos;
    }
    display.print(result);
  }
  oldPos = pos;
}

void section2(){
  tempScan = digitalRead(Section2Button);
  if(tempScan == 0){
    for(x = 0; x < 10; x++){
      jackConnections[x][0] = 0;
      jackConnections[x][1] = 0;
    }
    i = 0;
    for(x = 0; x < 10; x++){
      pinMode(JackPins[x], OUTPUT);
      digitalWrite(JackPins[x], LOW);
      for(y = 0; y < 10; y++){
        if(x != y){
          tempScan = digitalRead(JackPins[y]);
          if(tempScan == 0){
            if(i == 0){
              jackConnections[i][0] = x+1;
              jackConnections[i][1] = y+1;
              i++;
            }
            else{
              for(num = 0; num < i+1; num++){
                if(round(x + 1) == round(jackConnections[num][1]) && round(y + 1) == round(jackConnections[num][0])){
                  tempScan = 1;
                }
              }
              if(tempScan == 0){
                jackConnections[i][0] = x+1;
                jackConnections[i][1] = y+1;
                i++;
              }
            }
          }
        }
      }
      pinMode(JackPins[x], INPUT_PULLUP);
      delay(10);
      if(i == 5){
        break;
      }
    }
    //check if same as solution
    num = 0;
    for(x = 0; x < 5; x++){
      for(y = 0; y < 2; y++){
        if(jackConnections[x][y] != connections[x][y]){
          num++;
        }
      }
    }
    if(num == 0){ 
      pixels.setPixelColor(17, pixels.Color(0, 255, 0));
      pixels.show();
      tone(speakerPin, 784);
      delay(100);
      tone(speakerPin, 1046);
      delay(200);
      noTone(speakerPin);
      sendUpdate(true);
      section = 2;
      result = picPath + filePrefix + B + picext;
      char file[result.length() + 1];
      i = 0;
      for(x = 0; x < 4; x++){
        for(y = 0; y < 4; y++){
          buttonValues[y][3-x] = findIndex(ProfileConversion[colorProfile],startColors[i]);
          i++;
        } 
      }
      strcpy(file, result.c_str()); 
      stat = reader.drawBMP(file, tft, 0, 0);
    }
    else{
      incorrect(2);
    }
  }
  delay(50);
}

void section3(){
  updateButtons();
  checkButtonStatus();
  tempScan = digitalRead(Section3Button);
  num = 0;
  i = 0;
  if(tempScan == 0){
    tempScan = 1;
    for(x = 0; x < 4; x++){
      for(y = 0; y < 4; y++){
        if(endColors[i] != ProfileConversion[colorProfile][buttonValues[y][3-x]]){
          num++;
        }
        i++;
      }
    }
    if(num != 0){
      incorrect(3);
    }
    else{
      pixels.setPixelColor(18, pixels.Color(0, 255, 0));
      pixels.show();
      sendUpdate(true);
      section = 0; 
      sendData();
    }
  }
}

void checkButtonStatus(){
  for(x = 0; x < 4; x++){
    digitalWrite(GridPins[0][x],LOW);
    for(y = 0; y < 4; y++){
      tempScan = digitalRead(GridPins[1][y]);
      if(tempScan == 0 && prev[x][y] != 0){
        prev[x][y] = 0;
        buttonValues[x][y] += 1;
        if(buttonValues[x][y] >= ColorNums[colorProfile]){
          buttonValues[x][y] = 0;
        }
      }
      prev[x][y] = tempScan;
    }
    digitalWrite(GridPins[0][x],HIGH);
    delay(10);
  }
}

void updateButtons(){
  for(x = 0; x < 4; x++){
    for(y = 0; y < 4; y++){
      lookingAt = buttonNums[x][y];
      pixels.setPixelColor(lookingAt-1, pixels.Color(Colors[ProfileConversion[colorProfile][buttonValues[x][y]]][0],Colors[ProfileConversion[colorProfile][buttonValues[x][y]]][1],Colors[ProfileConversion[colorProfile][buttonValues[x][y]]][2]));
    }
  }
  pixels.show();
  delay(10);
}

void readFile(){
  myFile = SD.open(count + ext);
  if (myFile) {
    opened = true;
    myFile.seek(0);
    i = 0;
    while(myFile.available()){
      i++;
      if(i == 1){
        rotary = myFile.parseInt();
      }
      else if(i >= 2 && i<= 11){
        if(i % 2 == 0){
          connections[(i-2)/2][0] = myFile.parseInt();
        }
        else{
          connections[(i-3)/2][1] = myFile.parseInt();
        }
      }
      else if(i == 12){
        colorProfile = myFile.parseInt() - 1;
      }
      else if(i > 12 && i <= 28){
        startColors[i-13] = myFile.parseInt();
      }
      else if(i > 28 && i <= 44){
        endColors[i-29] = myFile.parseInt();
      }
      else{
        myFile.parseInt();
      }
    }
    myFile.close();
  } else {
    // if the file didn't open, print an error:
  }
 }

 void sendData(){
  delay(100);
  tone(speakerPin, 784);
  delay(100);
  tone(speakerPin, 1046);
  delay(200);
  noTone(speakerPin);
  stat = reader.drawBMP("/Pictures/success.bmp", tft, 0, 0);
  delay(2500);
  setup();
 }
 byte findIndex(byte list[], byte valueWanted){
  for(byte j = 0; j < 7; j++){
    if(valueWanted == list[j]){
      return(j);
      break;
    }
  }
 }

 void sendUpdate(bool Didpass){
  for(i = 0; i < 8; i++){
    if(switches[7-i]){
      Serial.print(0);
    }
    else{
      Serial.print(1);
    }
  }
  if(section == 0){
    Serial.print("100");
  }
  else if(section == 1){
    Serial.print("010");
  }
  else{
    Serial.print("001");
  }
  if(Didpass){
    Serial.println(1);
  }
  else{
    Serial.println(0);
  }
 }
