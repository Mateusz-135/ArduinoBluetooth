const uint8_t redLedPin = 13;
const uint8_t greenLedPin = 12;
const uint8_t blueLedPin = 11;

bool are_cstrings_equal(char* str1, char* str2){ // function that checks two c-style strings for equality
  while(*str1 != '\0' && *str2 != '\0'){
    if(*str1 != *str2)
      return false;
    else{
      str1++;
      str2++;
    }
  }
  return *str1 == *str2;
}

int parse_cstring(char* str){
  int ret{};
  while(*str != '\0'){
    switch(*str){
      case '0':
        break;
      case '1':
        ret += 1;
        break;
      case '2':
        ret += 2;
        break;
      case '3':
        ret += 3;
        break;
      case '4':
        ret += 4;
        break;
      case '5':
        ret += 5;
        break;
      case '6':
        ret += 6;
        break;
      case '7':
        ret += 7;
        break;
      case '8':
        ret += 8;
        break;
      case '9':
        ret += 9;
        break;
      default:
        return -1;
    }
    str++;
    ret *= 10;
  }
  return ret / 10;
}

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
    char message[10]{'\0'};
    Serial.readBytesUntil(' ', message, 10);

    char value[5]{'\0'};
    Serial.readBytesUntil('\n', value, 5);
    int state = parse_cstring(value);

    if(state == -1){
      Serial.print("Err\n");
      return;
    }
      
    uint8_t pin{};

    if(are_cstrings_equal(message, "red"))
      pin = redLedPin;
    else if(are_cstrings_equal(message, "green"))
      pin = greenLedPin;
    else if(are_cstrings_equal(message, "blue"))
      pin = blueLedPin;
    else{
      Serial.print("Err\n");
      return;
    }

    Serial.print("OK\n");
      
    digitalWrite(pin, state);
  }
}
