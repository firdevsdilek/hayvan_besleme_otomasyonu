# Hayvan Besleme Otomasyonu (Arduino)
# Animal Feeding Automation (Arduino)

## 🇹🇷

Bu proje, Arduino kullanılarak geliştirilen zaman tabanlı bir **hayvan besleme otomasyonu** sistemidir.  
Belirlenen saatlerde otomatik mama dağıtımı yapar ve ortam koşullarını sensörler aracılığıyla takip eder.

### Proje Özeti
Sistem; gerçek zamanlı saat (RTC) modülü ile zamanı takip eder, DHT11 ve LDR sensörleri ile ortam verilerini okur ve servo motor mekanizması sayesinde belirlenen saatlerde otomatik olarak mama dağıtımı gerçekleştirir. Proje, donanım–yazılım entegrasyonunu deneyimlemek amacıyla uygulamalı olarak geliştirilmiştir.

### Kullanılan Donanımlar
- Arduino Uno  
- DS3231 RTC modülü  
- DHT11 sıcaklık ve nem sensörü  
- LDR (ışık sensörü)  
- Servo motor  
- 16x2 I2C LCD ekran  

### Sistem Özellikleri
- Zaman tabanlı otomatik besleme  
- Ortam neminin izlenmesi ve uyarı mekanizması  
- LCD ekran üzerinden saat, durum ve sonraki besleme bilgisinin gösterilmesi  
- Sensör verilerinin seri port üzerinden izlenebilmesi  

### Amaç
Bu proje ile Arduino üzerinde sensör entegrasyonu, zaman tabanlı kontrol yapıları ve gömülü yazılım mantığı üzerine pratik kazanılması hedeflenmiştir.

### Not
Proje eğitim ve öğrenme amacıyla geliştirilmiştir.

---

## 🇬🇧

This project is a **time-based animal feeding automation** system developed using Arduino.  
It automatically dispenses food at scheduled times and monitors environmental conditions through various sensors.

### Project Overview
The system keeps track of time using a Real-Time Clock (RTC) module, reads environmental data via DHT11 and LDR sensors, and automatically dispenses food at predefined times using a servo motor mechanism. The project was developed as a hands-on study to gain experience in hardware–software integration.

### Hardware Components
- Arduino Uno  
- DS3231 RTC module  
- DHT11 temperature and humidity sensor  
- LDR (light sensor)  
- Servo motor  
- 16x2 I2C LCD display  

### System Features
- Time-based automatic feeding  
- Monitoring of ambient humidity with warning mechanism  
- Display of time, system status, and next feeding time on the LCD  
- Sensor data monitoring via serial communication  

### Purpose
The aim of this project is to gain practical experience in sensor integration, time-based control structures, and embedded software development using Arduino.

### Note
This project was developed for educational and learning purposes.
