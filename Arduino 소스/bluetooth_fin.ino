#include <SoftwareSerial.h>
#include <Wire.h>
#include <MsTimer2.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DEBUG
#define BUTTON_PIN 2
#define LED_BUILTIN_PIN 13

#define ARR_CNT 5
#define CMD_SIZE 60
char lcdLine1[17] = "";
char lcdLine2[17] = "";
char sendBuf[CMD_SIZE];
char recvId[10] = "YJC_SQL";  // SQL 저장 클라이이언트 ID

unsigned int secCount;
SoftwareSerial BTSerial(10, 11); // RX ==>BT:TXD, TX ==> BT:RXD

Servo servo;
int floodmode = 0;
int servoAngle;
char line[10] = "";

void setup()
{
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("setup() start!");
#endif
  lcd.init();
  lcd.backlight();
  lcdDisplay(0, 0, lcdLine1);
  lcdDisplay(0, 1, lcdLine2);
  pinMode(LED_BUILTIN_PIN, OUTPUT);

  servo.attach(5, 0, 180);
  servo.write(0);
 
  BTSerial.begin(9600); // set the data rate for the SoftwareSerial port
  MsTimer2::set(1000, timerIsr); // 1000ms period
  MsTimer2::start();
}

void loop()
{
  if (BTSerial.available())
    bluetoothEvent();

  lcdDisplay(0, 0, lcdLine1);
  lcdDisplay(0, 1, lcdLine2);
}

void bluetoothEvent()
{
  int i = 0;
  char * pToken;
  char * pArray[ARR_CNT] = {0};
  char recvBuf[CMD_SIZE] = {0};
  int len = BTSerial.readBytesUntil('\n', recvBuf, sizeof(recvBuf) - 1);

#ifdef DEBUG
  Serial.print("Recv : ");
  Serial.println(recvBuf);
#endif


  pToken = strtok(recvBuf, "[@]");
  while (pToken != NULL)
  {
    pArray[i] =  pToken;
    if (++i >= ARR_CNT)
      break;
    pToken = strtok(NULL, "[@]");
  }


  if (!strcmp(pArray[1], "val")) {
    float waveValue = atoi(pArray[2]);
    if (waveValue < 30 && waveValue > 3) {
      floodmode = 1;
      sprintf(lcdLine1, "STOP! DANGER");
      sprintf(lcdLine2, "DO NOT ENTER!");
      //lcdDisplay(0, 0, lcdLine1);
      //lcdDisplay(0, 1, lcdLine2);
      digitalWrite(LED_BUILTIN_PIN, HIGH);

      servoAngle = servo.read();

      if(servoAngle = 0){
        for(int angle=0; angle<150; angle = angle+5) {
          servo.write(angle);
          delay(50);
        }
      }
    }
    else if (waveValue > 30) {
      floodmode = 0;
      sprintf(lcdLine1, "welcome");
      sprintf(lcdLine2, "welcome");
      //lcdDisplay(0, 0, lcdLine1);
      //lcdDisplay(0, 1, lcdLine2);
      digitalWrite(LED_BUILTIN_PIN, LOW);
      servo.write(0);
    }
    //sprintf(sendBuf, "[ALLMSG]FLOOD@DETECTED");
    //sprintf(sendBuf, "[%s]%s@%s\n", pArray[0], pArray[1], pArray[2]);
  }


  if (!strcmp(pArray[1], "FLOOD")) {
    if (!strcmp(pArray[2], "ON")) {
      floodmode = 1;
      sprintf(lcdLine1, "STOP! DANGER");
      sprintf(lcdLine2, "DO NOT ENTER!");
      //lcdDisplay(0, 0, lcdLine1);
      //lcdDisplay(0, 1, lcdLine2);
      digitalWrite(LED_BUILTIN_PIN, HIGH);

      //servoAngle = servo.read();

      //if(servoAngle = 0){
        for(int angle=0; angle<150; angle = angle+5) {
          servo.write(angle);
          delay(50);
        //}
      }
      sprintf(sendBuf, "%s\n",pArray[2]);
    }
    else if(!strcmp(pArray[2], "OFF")) {
      floodmode = 0;
      sprintf(lcdLine1, "welcome");
      sprintf(lcdLine2, "welcome");
      //lcdDisplay(0, 0, lcdLine1);
      //lcdDisplay(0, 1, lcdLine2);
      digitalWrite(LED_BUILTIN_PIN, LOW);
      servo.write(0);
      sprintf(sendBuf, "%s\n", pArray[2]);
    }

    
  }


  else if (!strncmp(pArray[1], " New", 4)) // New Connected
  {
    return ;
  }
  else if (!strncmp(pArray[1], " Alr", 4)) //Already logged
  {
    return ;
  }

#ifdef DEBUG
  Serial.print("Send : ");
  Serial.print(sendBuf);
#endif
  BTSerial.write(sendBuf);
}


void timerIsr()
{
  timerIsrFlag = true;
  secCount++;
}
void lcdDisplay(int x, int y, char * str)
{
  int len = 16 - strlen(str);
  lcd.setCursor(x, y);
  lcd.print(str);
  for (int i = len; i > 0; i--)
    lcd.write(' ');
}




