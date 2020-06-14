#include "HID-Project.h"
String fin;
char temp;
int i;
void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
  Gamepad.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial1.available() > 0){
    temp = Serial1.read();
    fin += temp;
    if(temp =='\n'){
      gamepad();
      fin = "";
    }
  }
}

void gamepad(){
  for(i = 0; i < 12; i++){
    if(fin[i] == '1'){
      Gamepad.press(i+1);
    }
  }
  Gamepad.write();
  delay(100);
  Gamepad.releaseAll();
  Gamepad.write();
}
