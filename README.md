# ir.rimot

راهنمای کامل پروژه RFID با کنترل از راه‌دست (IR Remote)
📋 فهرست مطالب
معرفی پروژه

قطعات مورد نیاز

نصب کتابخانه‌ها

نحوه اتصال

کد اصلی

عیب‌یابی

کاربردها

معرفی پروژه
این پروژه یک سیستم کنترل دسترسی با استفاده از کارت‌های RFID و کنترل از راه‌دست مادون‌قرمز (IR Remote) ایجاد می‌کند. سیستم می‌تواند:

شناسایی کارت‌های RFID مجاز

دریافت دستورات از کنترل IR

کنترل خروجی‌ها مانند LED، رله یا سروو موتور

قطعات مورد نیاز
قطعه	تعداد	توضیحات
آردوینو Uno	1	برد اصلی کنترل
ماژول RFID-RC522	1	برای خواندن کارت‌های RFID
گیرنده مادون‌قرمز	1	دریافت سیگنال از کنترل IR
کنترل از راه‌دست IR	1	ارسال دستورات
LED	2-3	نشانگر وضعیت (سبز/قرمز)
مقاومت 220Ω	2-3	برای LEDها
برد بورد	1	
سیم جامپر	20+	
نصب کتابخانه‌ها
در آردوینو IDE، از طریق Library Manager کتابخانه‌های زیر را نصب کنید:

MFRC522 (برای RFID)

Sketch → Include Library → Manage Libraries

جستجوی "MFRC522" و نصب

IRremote (برای کنترل IR)

جستجوی "IRremote" و نصب نسخه 3.6.1 یا بالاتر

نحوه اتصال
اتصال ماژول RFID-RC522:
text
RFID-RC522  →  آردوینو Uno
SDA         →  پین 10
SCK         →  پین 13
MOSI        →  پین 11
MISO        →  پین 12
IRQ         →  متصل نشود
GND         →  GND
RST         →  پین 9
3.3V        →  3.3V
اتصال گیرنده IR:
text
گیرنده IR  →  آردوینو Uno
VCC        →  5V
GND        →  GND
OUT        →  پین 2
اتصال LEDها:
text
LED قرمز: آند → مقاومت 220Ω → پین 7 | کاتد → GND
LED سبز: آند → مقاومت 220Ω → پین 6 | کاتد → GND
LED زرد: آند → مقاومت 220Ω → پین 5 | کاتد → GND
کد اصلی


'''
#include <SPI.h>
#include <MFRC522.h>
#include <IRremote.h>

// تعریف پین‌های RFID
#define RST_PIN 9
#define SS_PIN 10

// تعریف پین گیرنده IR
#define IR_RECEIVE_PIN 2

// تعریف پین‌های LED
#define LED_GREEN 6
#define LED_RED 7
#define LED_YELLOW 5

// ایجاد شیء RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);

// تعریف کدهای کنترل IR (مطابق با کنترل شما)
#define IR_BUTTON_1 0xFFA25D
#define IR_BUTTON_2 0xFF629D
#define IR_BUTTON_3 0xFFE21D

// UID کارت‌های مجاز
byte authorizedCards[][4] = {
  {0x12, 0x34, 0x56, 0x78},  // کارت ۱
  {0xAA, 0xBB, 0xCC, 0xDD}   // کارت ۲
};

void setup() {
  Serial.begin(9600);
  Serial.println("RFID + IR Remote System Started");
  
  // راه‌اندازی RFID
  SPI.begin();
  mfrc522.PCD_Init();
  
  // راه‌اندازی IR
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  
  // تنظیم پین‌های LED به عنوان خروجی
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  
  // خاموش کردن همه LEDها در ابتدا
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
  
  Serial.println("System Ready. Waiting for RFID card or IR command...");
}

void loop() {
  // بررسی دستورات IR
  checkIRCommand();
  
  // بررسی کارت RFID
  checkRFID();
  
  delay(100);
}

void checkIRCommand() {
  if (IrReceiver.decode()) {
    uint32_t command = IrReceiver.decodedIRData.decodedRawData;
    
    Serial.print("IR Command Received: 0x");
    Serial.println(command, HEX);
    
    switch(command) {
      case IR_BUTTON_1:
        Serial.println("Button 1: Green LED ON");
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_YELLOW, LOW);
        break;
        
      case IR_BUTTON_2:
        Serial.println("Button 2: Red LED ON");
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_YELLOW, LOW);
        break;
        
      case IR_BUTTON_3:
        Serial.println("Button 3: Yellow LED ON");
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_YELLOW, HIGH);
        break;
        
      default:
        Serial.println("Unknown IR Command");
        // چشمک زدن همه LEDها
        for(int i=0; i<3; i++) {
          digitalWrite(LED_GREEN, HIGH);
          digitalWrite(LED_RED, HIGH);
          digitalWrite(LED_YELLOW, HIGH);
          delay(200);
          digitalWrite(LED_GREEN, LOW);
          digitalWrite(LED_RED, LOW);
          digitalWrite(LED_YELLOW, LOW);
          delay(200);
        }
    }
    
    IrReceiver.resume(); // آماده‌سازی برای دریافت کد بعدی
  }
}

void checkRFID() {
  // بررسی کارت جدید
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  // نمایش UID کارت
  Serial.print("Card UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();
  
  // بررسی مجاز بودن کارت
  bool isAuthorized = false;
  for(int i = 0; i < sizeof(authorizedCards)/sizeof(authorizedCards[0]); i++) {
    if(memcmp(mfrc522.uid.uidByte, authorizedCards[i], 4) == 0) {
      isAuthorized = true;
      break;
    }
  }
  
  // واکنش به کارت
  if(isAuthorized) {
    Serial.println("Access GRANTED!");
    // LED سبز به مدت ۲ ثانیه روشن
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, LOW);
    delay(2000);
    digitalWrite(LED_GREEN, LOW);
  } else {
    Serial.println("Access DENIED!");
    // LED قرمز به مدت ۲ ثانیه روشن
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, HIGH);
    delay(2000);
    digitalWrite(LED_RED, LOW);
  }
  
  // توقف ارتباط با کارت
  mfrc522.PICC_HaltA();
}
'''







قق
