void setup() {
  Serial.begin(9600);
}

String serialInput = "";
enum LED_Formats {
  RGB
};
uint8_t num_leds = 1;
enum LED_Formats ledformat = RGB;
void loop() {
  delay(1);
}

void readSerial() {
  int data = 0;
  while (Serial.available() > 0) {
    data = Serial.read();
    serialInput += (char) data;
    Serial.print((char) data);
    if ('\n' == data || '\r' == data) {
      Serial.print(handleSerial(String(serialInput)));
      serialInput = "";
    }
  }
}

String handleSerial(String i) {
  i.trim();
  switch(i.charAt(0)) {
    case '?': // poke
      return String("!\n");
    case 'v': // version
      return String("1\n");
    case 'q': // query
      // uhhh we'll come back to that
      return String("e2\n");
    case 'n': // number
      if(i.charAt(0) == ' ') {
        long n = i.substring(2).toInt();
        if(n > 0 && n < (1 << 8)) {
          num_leds = n;
          return String("ok\n");
        } else {return String("e1\n");}
      } else {return String("e1\n");}
    case 'f': // format
      if(i.charAt(0) == ' ') {
        String fmt = i.substring(2);
        if(fmt == "rgb") {
          ledformat = RGB;
        }
        else {return String("e1\n");}
      }
      else {return String("e1\n");}
    case 'm': // mode
      // We'll come back to this too
      return String("e2\n");
  } // end of single-letter instructions
  // I have a horrible feeling parsing like this is a Bad Idea
  if(i.startsWith("solid")) {
    if(i.length() == 14) {
      // TODO 
    }
    else {return String("e1\n");}
  }
  
  
  return String("e0");
}
