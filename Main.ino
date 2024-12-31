#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Robojax_L298N_DC_motor.h>

// motor 1 settings
#define CHA 0
#define ENA 19 // this pin must be PWM enabled pin if Arduino board is used
#define IN1 18
#define IN2 5

const int CCW = 2; // do not change
const int CW  = 1; // do not change

#define motor1 1 // do not change
// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA);


#define alcohol A0
float sensorValue =0;
float val=0;
float fin =0;
int red = 25;
int green = 26;
int buzzer = 27;
int flagD=1;
int flagN =1;

// Replace with your network credentials
const char* ssid = "Your SSID";
const char* password ="Your Password";

// Initialize Telegram BOT
#define BOTtoken "Your Bot Token"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "Your Chat_ID"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);


void setup() {
  
  Serial.begin(9600);
  pinMode(alcohol, INPUT);
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(buzzer, OUTPUT);

  robot.begin();
  lcd.begin(); //initilize the lcd monitor
  lcd.backlight(); // Turn on the blacklight and print a message.
  lcd.print("Welcome to !!!");
  delay(2000);
  lcd.clear();
  lcd.print("IOT based");
  lcd.setCursor (0,1);
  lcd.print("alcohol monitor");
  delay(3000);
  lcd.clear();
 
  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  lcd.clear();
  lcd.print("  Connecting");
  lcd.setCursor (0,1);
  lcd.print("  WiFi.......");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  lcd.clear();
  lcd.print("WiFi connected");
  Serial.println("");
  Serial.println("WiFi connected");
  delay(1500);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
}

void loop() {
   robot.rotate(motor1, 100, CW); //dc start 
check:
  lcd.clear();
  
  sensorValue=0; 
  for(int i = 0; i<25; i++)
    {
      sensorValue += analogRead(alcohol); // Reading the gas value from analog pin
      delay(10);
    }
      val = ((sensorValue/25) * (5.0/2048.0));
      fin = val * .10;
      Serial.print("alcohol  ");
      Serial.println(sensorValue/25);
      Serial.print("val ");
      Serial.println(fin);
      delay(1000);
            

  if(fin<=0.8)
  {
     digitalWrite(red, LOW);
     digitalWrite(buzzer, LOW);
     digitalWrite(green, HIGH);
     lcd.print("AL = ");
     lcd.print(fin,3);
     lcd.print("mg/L");
     lcd.setCursor (0,1);
     lcd.print("Normal ");
     notify();
     flagD=1;
     delay(1000);
  }
  else
  { 
     digitalWrite(red, HIGH);
     digitalWrite(green, LOW);
     digitalWrite(buzzer, HIGH);
     robot.brake(1); //dc off 
     lcd.print("AL = ");
     lcd.print(fin,3);
     lcd.print("mg/L");
     lcd.setCursor (0,1);
     lcd.print("Drunk ");
     message();
     flagN = 1;
     delay(1000);
     goto check;
  }
 
}

void message()
{
  if(flagD== 1)
  {
    bot.sendMessage(CHAT_ID, "Drunk", "");
    flagD +=1; 
  }
}

void notify()
{
  if(flagN== 1)
  {
    bot.sendMessage(CHAT_ID, "Normal", "");
    flagN += 1; 
  }
}
