#include <Blynk.h>
#include<Keypad.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

int getFingerprintIDez();
const byte ROWS = 4;    // 행(rows) 개수
const byte COLS = 4;    // 열(columns) 개수
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
const int analogOutPin = A2;
char* secretCode = "1234"; 
int position = 0; 
int wrong = 0;
const byte rows = 4;
const byte cols = 4;
byte rowPins[ROWS] = {10, 11, 12, 13};
byte colPins[COLS] = {6, 7, 8, 9};
int redPin = 5;
int greenPin = 4;
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
void setup()  
{
  Serial.begin(115200);
  Serial.println("fingertest");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }
  Serial.println("Waiting for valid finger...");

}

void loop(){
  char key = keypad.getKey(); // 키패드에서 입력된 값을 가져옵니다.
  
  if((key >= '0' && key <= '9') || (key >= 'A' && key <='D')
    || (key == '*' || key == '#')){
  // 키패드에서 입력된 값을 조사하여 맞게 입력된 값일 경우(키패드에 있는 버튼이 맞을경우) 비교

    if(key == '*' || key == '#'){ // *, # 버튼을 눌렀을 경우
      position = 0; 
      wrong = 0; // 입력 초기화
      setLocked(true); // 잠금상태로 세팅
    } 
 
    else if(key == secretCode[position]){ // 해당 자리에 맞는 비밀번호가 입력됬을 경우
      position++; // 다음 자리로 넘어 감
      wrong = 0; // 비밀번호 오류 값을 0으로 만듬
    }
    
    else if(key != secretCode[position]){ // 해당 자리에 맞지 않는 비밀번호가 입력됬을 경우
      position = 0; // 비밀번호를 맞았을 경우를 0으로 만듬
      setLocked(true); // 잠금상태로 세팅
      wrong++; // 비밀번호 오류 값을 늘려준다
    }
  
    if(position == 4){ // 4자리 비밀번호가 모두 맞았을 경우
      
      setLocked(false); // 잠금상태를 해제 함
      
    }
    
    if(wrong == 4){ // 비밀번호 오류를 4번 했을 경우
      blink(); // Red LED를 깜빡여 준다.
      wrong = 0; // 비밀번호 오류 값을 0으로 만들어 준다.
    }
  }
  getFingerprintIDez();
  delay(50);
}


uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  analogWrite(analogOutPin,255);
  return finger.fingerID;
}
void setLocked(int locked){ // 잠금시와 해제시에 맞는 LED를 세팅해 주는 함수
  if(locked) { // 잠금시 Red LED를 켜주고, Green LED를 꺼준다.
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, LOW); 
  }

  else{ // 해제시 Red LED를 꺼주고, Green LED를 켜준다.
    digitalWrite(redPin, LOW);
    for(int i = 0; i < 4; i++){
      digitalWrite(greenPin, LOW);
      delay(500);
      digitalWrite(greenPin, HIGH);
      delay(500);
      analogWrite(analogOutPin,255);
    }
    
  } 
}

void blink(){ // 비밀번호 4번 오류시 Red LED를 깜빡여 주는 함수.
  for(int i = 0; i < 8; i++){ // 딜레이 만큼 Red LED를 껐다 켰다 해준다. 총 8회
    digitalWrite(redPin, LOW);
    delay(500);
    digitalWrite(redPin, HIGH);
    delay(500);
  }
}
