// This version of arduino code is much simplier and because of that response is much quicker
// Arduino receives just 1 byte code, defined in the enumerator below
// Response is also just 1 byte, either true or false value meaning successful command or failure

const uint8_t redLedPin = 13;
const uint8_t greenLedPin = 12;
const uint8_t blueLedPin = 11;

enum class Command : char{fail, redOn, redOff, greenOn, greenOff, blueOn, blueOff};

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
    char char_code{char(Serial.read())};
    Command command_code{Command(char_code)};

    switch(command_code){
      case Command::redOn:
        digitalWrite(redLedPin, HIGH); // red on
        break;
      case Command::redOff:
        digitalWrite(redLedPin, LOW); // red off
        break;
      case Command::greenOn:
        digitalWrite(greenLedPin, HIGH); // green on
        break;
      case Command::greenOff:
        digitalWrite(greenLedPin, LOW); // green off
        break;
      case Command::blueOn:
        digitalWrite(blueLedPin, HIGH); // blue on
        break;
      case Command::blueOff:
        digitalWrite(blueLedPin, LOW); // blue off
        break;
      default:
        Serial.write(char(Command::fail));
        return; // if error, return from serialEvent()
    }
    
    Serial.write(char_code); // if the command was read successfully write the name of the command back to the master
  }
}
