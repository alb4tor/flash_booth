

#define FADE 20
#define FADE_TEMPO 50
#define DEBOUNCE_TIME 10

byte pBtn1 = 6;
byte pBtn2 = 9;
byte pFla1 = 5;
byte pFla2 = 3;
byte pReset = 2;

byte pBtn1in = 8;
byte pBtn2in = 7;


byte pwm[17] = {0, 1, 2, 3, 4, 6, 8, 12, 16, 23, 32, 45, 64, 90, 128, 180, 255};


byte i = 0;
byte j = 1;

byte vF1 = 0;
byte vF2 = 0;
byte vB1 = 0;
byte vB2 = 0;

byte fade = 1;
byte sens = 1;
byte target = 0;

byte debounce_1 = 0;
byte debounce_2 = 0;

unsigned long last_millis = 0;

char c = 0;
char cmd[16];
byte cmd_ok = 0;
byte cmd_ptr = 0;
byte cmd_timeout = 0;

byte ok = 0;

byte str2byte(char *str){
  byte val = 0;
  val += str[0] - 48;
  val *= 10;
  val += str[1] - 48;

  return val;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(pBtn1, OUTPUT);
  digitalWrite(pBtn1, LOW);
  pinMode(pBtn2, OUTPUT);
  digitalWrite(pBtn2, LOW);
  pinMode(pFla1, OUTPUT);
  digitalWrite(pFla1, LOW);
  pinMode(pFla2, OUTPUT);
  digitalWrite(pFla2, LOW);

  pinMode(pBtn1in, INPUT);
  digitalWrite(pBtn1in, HIGH); // Pull-up
  pinMode(pBtn2in, INPUT);
  digitalWrite(pBtn2in, HIGH); // Pull-up


  //tests
  if(!Serial){
    digitalWrite(pBtn1, HIGH);
    digitalWrite(pBtn2, HIGH);
    delay(100);
    digitalWrite(pBtn1, LOW);
    digitalWrite(pBtn2, LOW);
    delay(100);
  }

  vB1 = FADE;
  vB2 = FADE;

}

void loop() {
  byte val = 0;
  if(Serial.available() >0 ){
    c = Serial.read();
    if(c == '\r' or c == '\n'){
      //Serial.println("ok");
      cmd_ok = 1;
      cmd[cmd_ptr] = 0;
      cmd_ptr = 0;
    } else {
      //Serial.print(".");
      cmd[cmd_ptr] = c;
      cmd_ptr++;
      if(cmd_ptr >=15){
        cmd_ptr = 0;
      }
      cmd_timeout = 100;
    }
  }

  if(cmd_ok){
    cmd_ok = 0;
    switch(cmd[0]){
      case 'S':
        val = str2byte((cmd+3));
        target = 0;
        if(strncmp(cmd+1, "F1", 2) == 0) {
          target = pFla1;
          vF1 = val;
        }
        if(strncmp(cmd+1, "F2", 2) == 0) {
          target = pFla2;
          vF2 = val;
        }
        if(strncmp(cmd+1, "B1", 2) == 0) {
          target = pBtn1;
          vB1 = val;
        }
        if(strncmp(cmd+1, "B2", 2) == 0) {
          target = pBtn2;
          vB2 = val;
        }
        if(target){
          if(val <= 16){
            analogWrite(target, pwm[val]);
          }
          Serial.println(val, DEC);
        }else{
          Serial.println("?");
        }
        break;
        
      case 'G':
        if(strncmp(cmd+1, "F1", 2) == 0) {
          Serial.println(vF1);
        }
        if(strncmp(cmd+1, "F2", 2) == 0) {
          Serial.println(vF2);
        }
        if(strncmp(cmd+1, "B1", 2) == 0) {
          Serial.println(vB1);
        }
        if(strncmp(cmd+1, "B2", 2) == 0) {
          Serial.println(vB2);
        }
        break;

      case 'R':
        Serial.println("bye");
        digitalWrite(pReset, LOW);
        break;

      default:
        Serial.println("?");
        goto looloop;
    }
  }


  // Buttons
  val = 0;
  if((digitalRead(pBtn1in) == LOW) && (debounce_1 == 0)) {
    val = 1;
    debounce_1 = DEBOUNCE_TIME;
  }

  if((digitalRead(pBtn2in) == LOW) && (debounce_2 == 0)) {
    val += 2;
    debounce_2 = DEBOUNCE_TIME;
  }

  if(val) {
    Serial.write('B');
    Serial.println(val, DEC);
  }


looloop:
  if(millis() - last_millis > FADE_TEMPO){
      
    if(debounce_1) {
      debounce_1--;
    }

    if(debounce_2) {
      debounce_2--;
    }

    
    fade += sens;
    if(fade == 0) {
      fade = 2;
      sens = 1;
    }
    if (fade == 17) {
      fade = 15;
      sens = -1;
    }

    if(vF1 == FADE) analogWrite(pFla1, pwm[fade]);
    if(vF2 == FADE) analogWrite(pFla2, pwm[fade]);
    if(vB1 == FADE) analogWrite(pBtn1, pwm[fade]);
    if(vB2 == FADE) analogWrite(pBtn2, pwm[fade]);

    if(cmd_timeout > 0) {
      cmd_timeout--;
      if(cmd_timeout == 1) {
        cmd_timeout = 0;
        cmd_ptr = 0;
      }
    }
    last_millis = millis();
  }
}
