// This version of arduino code is much simplier and because of that response is much quicker
// Arduino receives just 1 byte code, defined in the enumerator below
// Response is also just 1 byte, either true or false value meaning successful command or failure

const uint8_t redLedPin = 13;
const uint8_t greenLedPin = 12;
const uint8_t blueLedPin = 11;

enum command{redOn, redOff, greenOn, greenOff, blueOn, blueOff};

void setup() {
  Serial.begin(9600);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  
  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(blueLedPin, LOW);
}
 
void loop() {
}
 
void serialEvent() {
  while (Serial.available()) {
    char command_code{Serial.read()};

    switch(command_code){
      case redOn:
        digitalWrite(redLedPin, HIGH); // red on
        break;
      case redOff:
        digitalWrite(redLedPin, LOW); // red off
        break;
      case greenOn:
        digitalWrite(greenLedPin, HIGH); // green on
        break;
      case greenOff:
        digitalWrite(greenLedPin, LOW); // green off
        break;
      case blueOn:
        digitalWrite(blueLedPin, HIGH); // blue on
        break;
      case blueOff:
        digitalWrite(blueLedPin, LOW); // blue off
        break;
      default:
        Serial.write(false);
        return; // if error, return from serialEvent()
    }
    
    Serial.write(true); // if there was no error respond with OK
  }
}
