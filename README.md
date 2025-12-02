# 🚀 پروژه هوشمند RFID + کنترل از راه‌دست IR

## 📌 معرفی پروژه
سیستم ترکیبی **کنترل دسترسی RFID** و **کنترل IR** برای پروژه‌های امنیتی و اتوماسیون

## 📦 قطعات مورد نیاز
| قطعه | تعداد | توضیحات |
|------|-------|---------|
| آردوینو Uno | ۱ | پردازنده اصلی |
| ماژول RFID-RC522 | ۱ | خواندن کارت‌های RFID |
| گیرنده IR (VS1838B) | ۱ | دریافت سیگنال IR |
| ریموت کنترل IR | ۱ | ارسال دستورات |
| LED سبز، قرمز، زرد | هرکدام ۱ | نمایش وضعیت |
| مقاومت ۲۲۰Ω | ۳ | برای LEDها |
| برد بورد | ۱ | مونتاژ مدار |
| سیم جامپر | ۲۰+ | اتصالات |

## 🔌 نحوه اتصال
### 🔐 اتصال RFID-RC522:
```
SDA  → پین 10
SCK  → پین 13
MOSI → پین 11
MISO → پین 12
RST  → پین 9
GND  → GND
VCC  → 3.3V
```

### 📟 اتصال گیرنده IR:
```
VCC → 5V
GND → GND
OUT → پین 2
```

### 💡 اتصال LEDها:
```
سبز  → پین 6 + مقاومت 220Ω
قرمز → پین 7 + مقاومت 220Ω
زرد  → پین 5 + مقاومت 220Ω
```

## 📜 کد اصلی
```arduino
#include <SPI.h>
#include <MFRC522.h>
#include <IRremote.h>

#define RST_PIN 9
#define SS_PIN 10
#define IR_RECEIVE_PIN 2

#define LED_GREEN 6
#define LED_RED 7
#define LED_YELLOW 5

MFRC522 mfrc522(SS_PIN, RST_PIN);

// کدهای دکمه‌های ریموت (با ریموت خود تست کنید)
#define IR_BUTTON_1 0xFFA25D
#define IR_BUTTON_2 0xFF629D
#define IR_BUTTON_3 0xFFE21D

// کارت‌های مجاز
byte authorizedCards[][4] = {
  {0x12, 0x34, 0x56, 0x78},  // کارت ۱
  {0xAA, 0xBB, 0xCC, 0xDD}   // کارت ۲
};

void setup() {
  Serial.begin(9600);
  Serial.println("🚀 سیستم RFID + IR راه‌اندازی شد");
  
  SPI.begin();
  mfrc522.PCD_Init();
  IrReceiver.begin(IR_RECEIVE_PIN);
  
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  
  allLEDsOff();
  Serial.println("✅ سیستم آماده است");
}

void loop() {
  checkIRCommand();
  checkRFID();
  delay(100);
}

void checkIRCommand() {
  if (IrReceiver.decode()) {
    uint32_t command = IrReceiver.decodedIRData.decodedRawData;
    
    Serial.print("📟 دستور IR دریافت شد: 0x");
    Serial.println(command, HEX);
    
    switch(command) {
      case IR_BUTTON_1:
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_YELLOW, LOW);
        Serial.println("💚 LED سبز روشن");
        break;
        
      case IR_BUTTON_2:
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_YELLOW, LOW);
        Serial.println("❤️ LED قرمز روشن");
        break;
        
      case IR_BUTTON_3:
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_YELLOW, HIGH);
        Serial.println("💛 LED زرد روشن");
        break;
        
      default:
        Serial.println("⚠️ دستور ناشناخته");
        blinkAllLEDs();
    }
    IrReceiver.resume();
  }
}

void checkRFID() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;
  
  Serial.print("🔑 UID کارت: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();
  
  bool isAuthorized = false;
  for(int i = 0; i < sizeof(authorizedCards)/sizeof(authorizedCards[0]); i++) {
    if(memcmp(mfrc522.uid.uidByte, authorizedCards[i], 4) == 0) {
      isAuthorized = true;
      break;
    }
  }
  
  if(isAuthorized) {
    Serial.println("✅ دسترسی مجاز!");
    grantAccess();
  } else {
    Serial.println("❌ دسترسی رد شد!");
    denyAccess();
  }
  
  mfrc522.PICC_HaltA();
}

void grantAccess() {
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_RED, LOW);
  delay(2000);
  digitalWrite(LED_GREEN, LOW);
}

void denyAccess() {
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, HIGH);
  delay(2000);
  digitalWrite(LED_RED, LOW);
}

void blinkAllLEDs() {
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

void allLEDsOff() {
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_YELLOW, LOW);
}
```

## 🚀 راه‌اندازی سریع
1. **اتصالات** را مطابق جدول انجام دهید
2. **کتابخانه‌ها** را نصب کنید:
   - `MFRC522` برای RFID
   - `IRremote` برای کنترل IR
3. **کد** را در آردوینو IDE بارگذاری کنید
4. **کدهای ریموت** خود را پیدا کنید (مقادیر IR_BUTTON_1,2,3 را تغییر دهید)

## 🛠️ کد کمکی برای پیدا کردن کدهای ریموت
```arduino
void setup() {
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN);
  Serial.println("🔍 برای پیدا کردن کدهای ریموت، دکمه‌ها را فشار دهید");
}

void loop() {
  if (IrReceiver.decode()) {
    Serial.print("🆔 کد دریافتی: 0x");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    IrReceiver.resume();
  }
}
```

## ⚠️ نکات مهم
- ولتاژ ماژول RFID باید **۳.۳V** باشد (نه ۵V)
- کدهای IR بسته به ریموت متفاوت است
- برای ذخیره کارت‌های بیشتر از EEPROM استفاده کنید
- در صورت نیاز به برد بیشتر، از آنتن خارجی استفاده کنید

## 📞 پشتیبانی
برای سوالات و مشکلات:
1. اتصالات را بررسی کنید
2. سریال مانیتور را بررسی کنید
3. کدهای ریموت را تست کنید

---
**📅 آخرین بروزرسانی:** ۱۴۰۳/۰۲/۲۱  
**🔄 نسخه:** ۲.۰.۰
