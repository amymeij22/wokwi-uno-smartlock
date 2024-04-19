//Deklarasi library
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <RTClib.h>

//Inisialisasi keypad
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {8, 7, 6, 5};
byte colPins[COLS] = {4, 3, 2, 9};

//Deklarasi variabel defaultPin dan userPin
const String defaultPin = "";
String userPin = defaultPin;

//Inisialisasi modul
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27, 20, 4);
Servo myservo;
RTC_DS3231 rtc;

//Deklarasi pin digital 
const int redLedPin = 10;
const int blueLedPin = 11;
const int buzzerPin = 12;
const int servoPin = 13;

//Deklarasi karakter filled square
const byte filledSquareAddr = 0x00;
const byte filledSquare[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

int errorCount = 0; //Deklarasi integer counter
DateTime now; //Deklarasi variabel now

bool firstTimeSetup = true; //Deklarasi boolean Setup PIN

void setup() {
  //Program awal
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(redLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  myservo.attach(servoPin);
  myservo.write(90); 

  //Inisialisasi modul RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  //Tampilan awal
  lcd.clear();
  fadeInText("SMART LOCK SYSTEM", 1, 0);
  fadeInText("AHMAD MEIJLAN YASIR", 0, 1);
  fadeInText("41220008", 6, 2);
  fadeInText("INSTRUMENTASI 4D", 2, 3);
  delay(3000);
  lcd.clear();

  //Tampilan loading
  fadeInText("LOADING", 6, 1);
  displayLoadingAnimation();
  lcd.clear();

  //Eksekusi Setup PIN awal
  if (firstTimeSetup) {
    setupFirstTime();
    firstTimeSetup = false;
  }
}

void loop() {
  now = rtc.now(); //Deklarasi variabel now
  
  //Eksekusi Menu Utama jika sudah Setup PIN awal
  if (!firstTimeSetup) {
    setupMenu();
  }
}

//Menu Utama
void setupMenu() {
  myservo.write(90);
  lcd.setCursor(5, 0);
  lcd.print("SMART LOCK");
  lcd.setCursor(0, 1);
  lcd.print("A. BUKA PINTU");
  lcd.setCursor(0, 2);
  lcd.print("B. UBAH PIN");
  lcd.setCursor(0, 3);
  lcd.print("C. INFORMASI");
  
  char key = keypad.getKey();
  if (key != NO_KEY) {
      playTone(key);
      delay(200);
    if (key == 'A') {
      enterPin();
    } else if (key == 'B') {
      changePin();
    } else if (key == 'C') {
      authorInfo();
    } 
  }
}

//Eksekusi jika input opsi A. BUKA PINTU
void enterPin() {
  String enteredPin = "";

  lcd.clear();
  lcd.print("MASUKKAN PIN:");
  lcd.setCursor(0, 2);

  myservo.write(90); 

  while (enteredPin.length() < 5) {
    char key = keypad.getKey();
    if (key != NO_KEY) {
      lcd.print("*");
      enteredPin += key;
      playTone(key);
      delay(200);
    }
  }

  //Eksekusi jika PIN benar
  if (enteredPin == userPin) {
    digitalWrite(blueLedPin, HIGH);
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("SELAMAT DATANG!!");
    lcd.setCursor(0, 1);
    lcd.print("SISTEM KUNCI PINTAR!");
    lcd.setCursor(1, 3);
    lcd.print(now.year(), DEC);
    lcd.print('/');
    lcd.print(now.month(), DEC);
    lcd.print('/');
    lcd.print(now.day(), DEC);
    lcd.print(" ");
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print(now.second(), DEC);
    delay(2000);
    digitalWrite(blueLedPin, LOW);
    playTone('W');
    myservo.write(0); 
    delay(5000);
    lcd.clear();
  } else {
    handleIncorrectPin(); //Eksekusi jika PIN salah
  }
}

//Deklarasi fungsi untuk dijalankan ketika PIN salah
void handleIncorrectPin() {
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("PIN SALAH!");
  lcd.setCursor(1, 3);
  lcd.print(now.year(), DEC);
  lcd.print('/');
  lcd.print(now.month(), DEC);
  lcd.print('/');
  lcd.print(now.day(), DEC);
  lcd.print(" ");
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  lcd.print(now.minute(), DEC);
  lcd.print(':');
  lcd.print(now.second(), DEC);
  delay(2000);
  digitalWrite(redLedPin, LOW);
  lcd.clear();

  //Eksekusi jika PIN salah = 3 kali
  errorCount++;
  if (errorCount == 3) {
    digitalWrite(redLedPin, HIGH);
    lcd.setCursor(5, 0);
    lcd.print("PIN SALAH!");
    lcd.setCursor(1, 3);
    lcd.print(now.year(), DEC);
    lcd.print('/');
    lcd.print(now.month(), DEC);
    lcd.print('/');
    lcd.print(now.day(), DEC);
    lcd.print(" ");
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print(now.second(), DEC);
    playTone('E');
    delay(2000);
    digitalWrite(redLedPin, LOW);
    errorCount = 0;
    lcd.clear();
  }
}

//Deklarasi fungsi untuk dijalankan jika input opsi B. UBAH PIN
void changePin() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("UBAH PIN");
  lcd.setCursor(0, 1);
  lcd.print("MASUKKAN PIN LAMA:");
  lcd.setCursor(0, 2);

  String oldPin = "";

  while (oldPin.length() < 5) {
    char key = keypad.getKey();
    if (key != NO_KEY) {
      lcd.print("*");
      oldPin += key;
      playTone(key);
      delay(200);
    }
  }

  delay(1000);

  if (oldPin == userPin) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UBAH PIN");
    lcd.setCursor(0, 1);
    lcd.print("MASUKKAN PIN BARU:");
    lcd.setCursor(0, 2);

    String newPin = "";

    while (newPin.length() < 5) {
      char key = keypad.getKey();
      if (key != NO_KEY) {
        lcd.print("*");
        newPin += key;
        playTone(key);
        delay(200);
      }
    }

    delay(1000); 

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UBAH PIN");
    lcd.setCursor(0, 1);
    lcd.print("KONFIRMASI PIN:");
    lcd.setCursor(0, 2);

    String confirmPin = "";

    while (confirmPin.length() < 5) {
      char key = keypad.getKey();
      if (key != NO_KEY) {
        lcd.print("*");
        confirmPin += key;
        playTone(key);
        delay(200);
      }
    }

    if (newPin == confirmPin) {
      userPin = newPin;
      lcd.clear();
      lcd.print("UBAH PIN");
      lcd.setCursor(0, 1);
      lcd.print("PIN DIUBAH!");
      delay(2000);
    } else {
      lcd.clear();
      lcd.print("UBAH PIN");
      lcd.setCursor(0, 1);    
      lcd.print("UBAH PIN GAGAL!");
      delay(2000);
    }
  } else {
    handlePreviousPin(); //Eksekusi jika PIN Sebelumnya salah
  }

  lcd.clear();
}

//Deklarasi fungsi untuk dijalankan apabila PIN Sebelumnya salah
void handlePreviousPin() {
    lcd.clear();
    lcd.print("UBAH PIN");
    lcd.setCursor(0, 1);
    lcd.print("PIN LAMA SALAH!");
    delay(2000);
    digitalWrite(redLedPin, LOW);
    lcd.clear();

    //Eksekusi jika PIN Sebelumnya salah = 3 kali
    errorCount++;
    if (errorCount == 3) {
      digitalWrite(redLedPin, HIGH);
      lcd.setCursor(5, 0);
      lcd.print("PIN SALAH!");
      lcd.setCursor(1, 3);
      lcd.print(now.year(), DEC);
      lcd.print('/');
      lcd.print(now.month(), DEC);
      lcd.print('/');
      lcd.print(now.day(), DEC);
      lcd.print(" ");
      lcd.print(now.hour(), DEC);
      lcd.print(':');
      lcd.print(now.minute(), DEC);
      lcd.print(':');
      lcd.print(now.second(), DEC);
      playTone('E');
      delay(2000);
      digitalWrite(redLedPin, LOW);
      errorCount = 0;
      lcd.clear();
  }
}

void authorInfo() {
  lcd.clear();
  fadeInText("SMART LOCK SYSTEM", 1, 0);
  fadeInText("AHMAD MEIJLAN YASIR", 0, 1);
  fadeInText("41220008", 6, 2);
  fadeInText("INSTRUMENTASI 4D", 2, 3);
  delay(3000);
  lcd.clear();
}

//Deklarasi fungsi Setup PIN Awal
void setupFirstTime() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SETUP PIN");
  lcd.setCursor(0, 1);
  lcd.print("MASUKKAN PIN BARU:");
  lcd.setCursor(0, 2);

  String newPin = "";

  while (newPin.length() < 5) {
    char key = keypad.getKey();
    if (key != NO_KEY) {
      lcd.print("*");
      newPin += key;
      playTone(key);
      delay(200);
    }
  }

  delay(1000); 

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SETUP PIN");
  lcd.setCursor(0, 1);
  lcd.print("KONFIRMASI PIN:");
  lcd.setCursor(0, 2);

  String confirmPin = "";

  while (confirmPin.length() < 5) {
    char key = keypad.getKey();
    if (key != NO_KEY) {
      lcd.print("*");
      confirmPin += key;
      playTone(key);
      delay(200);
    }
  }

  if (newPin == confirmPin) {
    userPin = newPin;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SETUP PIN");
    lcd.setCursor(0, 1);
    lcd.print("PIN DISIMPAN!");
    delay(2000);
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SETUP PIN");
    lcd.setCursor(0, 1);
    lcd.print("SETUP PIN GAGAL!");
    delay(2000);
    setupFirstTime(); 
  }

  lcd.clear();
}

//Deklarasi fungsi efek Fade In
void fadeInText(String text, int col, int row) {
  for (int i = 0; i < text.length(); i++) {
    lcd.setCursor(col + i, row);
    lcd.print(text[i]);
    delay(50);
  }
}

//Deklarasi fungsi Loading
void displayLoadingAnimation() {
  lcd.createChar(filledSquareAddr, (uint8_t *)filledSquare);
  for (int i = 0; i < 16; i++) {
    lcd.setCursor(i + 2, 2); 
    lcd.write((uint8_t)filledSquareAddr); 
    delay(100);
  }
}

//Deklarasi fungsi nada dan melodi
void playTone(char key) {
  int melody[] = {262, 330, 392, 523, 659};
  int warningMelody[] = {1000, 1000, 1000, 1000, 1000};
  int noteDuration = 300;
  int pauseBetweenNotes = 50;
  
  switch(key) {
    case 'W':
      for (int i = 0; i < 5; i++) {
        tone(buzzerPin, melody[i]);
        delay(noteDuration);
        noTone(buzzerPin);
        delay(pauseBetweenNotes);
      }
      break;
    case 'E':
      for (int i = 0; i < 5; i++) {
        tone(buzzerPin, warningMelody[i]);
        delay(noteDuration);
        noTone(buzzerPin);
        delay(500);
      }
      break;
    default:
      tone(buzzerPin, 988);
      delay(50);
      noTone(buzzerPin);
      break;
  }
}
