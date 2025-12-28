#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Servo.h>
#include <DHT.h>


#define LDR_PIN A0        
#define DHT_PIN 2         
#define SERVO_PIN 9       

#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);


LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;
Servo servoMotor;

const int beslemeSayisi = 3;
int beslemeSaatleri[beslemeSayisi][2] = {
  {8, 0},    // Sabah 08:00
  {14, 44},   // Öğlen 13:00
  {15, 16}    // Akşam 19:00
};


bool beslemeYapildi[beslemeSayisi] = {false, false, false};
bool nemUyariAktif = false;
unsigned long sonGuncellemeSuresi = 0;
unsigned long sonNemOkumaSuresi = 0;


const int LDR_ESIK = 500;      // Karanlık eşiği (0-1023 arası)
const float NEM_MIN = 20.0;    // Minimum nem %20
const float NEM_MAX = 70.0;    // Maximum nem %


float sonNemDegeri = 0;
float sonSicaklikDegeri = 0;
bool dhtBasarili = false;

void setup() {
  Serial.begin(9600);
  Serial.println("=========================");
  Serial.println("Sistem Baslatiliyor...");
  Serial.println("=========================");
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hayvan Besleme");
  lcd.setCursor(0, 1);
  lcd.print("Otomasyonu ");
  delay(2000);
  
 
  Serial.print("RTC baslatiliyor... ");
  if (!rtc.begin()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RTC Hata!");
    lcd.setCursor(0, 1);
    lcd.print("Kontrol Edin");
    Serial.println("HATA!");
    while (1) delay(1000);
  }
  Serial.println("OK");
  
  if (rtc.lostPower()) {
    Serial.println("RTC zamani kayboldu!");
    Serial.println("UYARI: Zamani ayarlayin!");
    // İlk kullanımda bu satırı aktif edin, sonra tekrar yorum yapın:
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  
  Serial.print("DHT11 baslatiliyor... ");
  dht.begin();
  delay(2000);  // DHT11'in stabilize olması için bekleme
  

  float testNem = dht.readHumidity();
  float testSicaklik = dht.readTemperature();
  
  if (isnan(testNem) || isnan(testSicaklik)) {
    Serial.println("HATA!");
    Serial.println("DHT11 Baglanti Kontrolu:");
    Serial.println("- VCC -> 5V");
    Serial.println("- DATA -> D2 (Pin 2)");
    Serial.println("- GND -> GND");
    Serial.println("- 10K pull-up direnci (DATA-VCC arasi)");
    dhtBasarili = false;
  } else {
    Serial.println("OK");
    Serial.print("Nem: ");
    Serial.print(testNem);
    Serial.print("% - Sicaklik: ");
    Serial.print(testSicaklik);
    Serial.println("C");
    sonNemDegeri = testNem;
    sonSicaklikDegeri = testSicaklik;
    dhtBasarili = true;
  }
  
  
  Serial.print("Servo motor baslatiliyor... ");
  servoMotor.attach(SERVO_PIN);
  servoMotor.write(180);  // Başlangıç pozisyonu (180'den başlayacak)
  delay(500);
  Serial.println("OK");
  
  lcd.clear();
  Serial.println("=========================");
  Serial.println("Sistem Hazir!");
  Serial.println("=========================");
}

void loop() {
  DateTime now = rtc.now();
  
  // LDR okuma
  int ldrDeger = analogRead(LDR_PIN);
  
  if (millis() - sonNemOkumaSuresi >= 2000) {
    float nem = dht.readHumidity();
    float sicaklik = dht.readTemperature();
    
    if (!isnan(nem) && !isnan(sicaklik)) {
      sonNemDegeri = nem;
      sonSicaklikDegeri = sicaklik;
      dhtBasarili = true;
      
      // Serial'e yazdır
      Serial.print("Nem: ");
      Serial.print(nem);
      Serial.print("% | Sicaklik: ");
      Serial.print(sicaklik);
      Serial.print("C | LDR: ");
      Serial.println(ldrDeger);
    } else {
      dhtBasarili = false;
      Serial.println("DHT11 okuma hatasi!");
    }
    
    sonNemOkumaSuresi = millis();
  }
  
  
  nemKontrol(sonNemDegeri, dhtBasarili);
  
  
  if (millis() - sonGuncellemeSuresi >= 1000) {
    if (!nemUyariAktif) {
      guncelleLCD(now, ldrDeger, sonNemDegeri, sonSicaklikDegeri, dhtBasarili);
    }
    sonGuncellemeSuresi = millis();
  }
  
  
  beslemeKontrol(now);
  
  delay(100);
}

void guncelleLCD(DateTime now, int ldr, float nem, float sicaklik, bool dhtOK) {
  
  lcd.setCursor(0, 0);
  
  
  if (now.hour() < 10) lcd.print("0");
  lcd.print(now.hour());
  lcd.print(":");
  if (now.minute() < 10) lcd.print("0");
  lcd.print(now.minute());
  lcd.print(":");
  if (now.second() < 10) lcd.print("0");
  lcd.print(now.second());
  
  
  lcd.print(" ");
  if (ldr < LDR_ESIK) {
    lcd.print("A");  
  } else {
    lcd.print("K");  
  }
  
  // LDR değeri
  lcd.print(":");
  lcd.print(ldr);
  lcd.print("   ");
  
  
  lcd.setCursor(0, 1);
  
  if (!dhtOK) {
    lcd.print("Nem:Hata        ");
  } else {
    lcd.print("N:");
    lcd.print((int)nem);
    lcd.print("% ");
    
    
    int sonraki = sonrakiBesleme(now);
    if (sonraki != -1) {
      lcd.print("S:");
      if (beslemeSaatleri[sonraki][0] < 10) lcd.print("0");
      lcd.print(beslemeSaatleri[sonraki][0]);
      lcd.print(":");
      if (beslemeSaatleri[sonraki][1] < 10) lcd.print("0");
      lcd.print(beslemeSaatleri[sonraki][1]);
    } else {
      lcd.print("Tamamlandi");
    }
    lcd.print("   ");
  }
}

void beslemeKontrol(DateTime now) {
  int suankiSaat = now.hour();
  int suankiDakika = now.minute();
  int suankiSaniye = now.second();
  
  
  for (int i = 0; i < beslemeSayisi; i++) {
    if (suankiSaat == beslemeSaatleri[i][0] && 
        suankiDakika == beslemeSaatleri[i][1] && 
        suankiSaniye < 5 &&
        !beslemeYapildi[i]) {
      
      Serial.print("BESLEME ZAMANI: ");
      Serial.print(i + 1);
      Serial.println(". besleme");
      
      mamaVer();
      beslemeYapildi[i] = true;
    }
  }
  
  
  if (suankiSaat == 0 && suankiDakika == 0 && suankiSaniye < 5) {
    for (int i = 0; i < beslemeSayisi; i++) {
      beslemeYapildi[i] = false;
    }
    Serial.println("Yeni gun - Besleme bayraklari sifirlandi");
  }
}

void mamaVer() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   MAMA   ");
  lcd.setCursor(0, 1);
  lcd.print("  VERILIYOR...");
  
  Serial.println(">>> Mama veriliyor...");
  
  
  servoMotor.write(180);
  delay(500);
  
  
  for (int aci = 180; aci >= 0; aci -= 5) {
    servoMotor.write(aci);
    delay(50);
  }
  
  delay(1000);  
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  MAMA VERILDI");
  lcd.setCursor(0, 1);
  lcd.print(" AFIYET OLSUN ;)");
  
  Serial.println(">>> Mama verildi!");
  
  delay(3000);
  lcd.clear();
}

void nemKontrol(float nem, bool dhtOK) {
  
  if (!dhtOK) {
    if (!nemUyariAktif) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  DHT11 HATA! ");
      lcd.setCursor(0, 1);
      lcd.print("Baglanti Kontrol");
      nemUyariAktif = true;
      delay(3000);
      nemUyariAktif = false;
      lcd.clear();
    }
    return;
  }
  
  
  if (nem >= NEM_MIN && nem <= NEM_MAX) {
    if (nemUyariAktif) {
      nemUyariAktif = false;
      lcd.clear();
    }
    return;
  }
  

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   UYARI!   ");
  lcd.setCursor(0, 1);
  
  if (nem < NEM_MIN) {
    lcd.print("Nem Dusuk:");
    Serial.print("UYARI: Nem cok dusuk - ");
  } else {
    lcd.print("Nem Yuksek:");
    Serial.print("UYARI: Nem cok yuksek - ");
  }
  
  lcd.print((int)nem);
  lcd.print("%");
  Serial.print(nem);
  Serial.println("%");
  
  nemUyariAktif = true;
  delay(3000);
  nemUyariAktif = false;
  lcd.clear();
}

int sonrakiBesleme(DateTime now) {
  int suankiSaat = now.hour();
  int suankiDakika = now.minute();
  int suankiDakikaToplam = suankiSaat * 60 + suankiDakika;
  
  for (int i = 0; i < beslemeSayisi; i++) {
    int beslemeDakikaToplam = beslemeSaatleri[i][0] * 60 + beslemeSaatleri[i][1];
    
    if (beslemeDakikaToplam > suankiDakikaToplam && !beslemeYapildi[i]) {
      return i;
    }
  }
  
  return 0;  
}