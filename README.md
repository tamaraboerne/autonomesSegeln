# Projekt: Autonomes Segeln – Clifton Autopilot

Dieses Repository enthält den Code für das **autonome Landsegler-Fahrzeug Clifton**. Ziel ist die selbstständige Navigation entlang vorgegebener Wegpunkte mithilfe eines aerodynamischen Segels und einer Steuerklappe (Flap). Das System besteht aus **drei Komponenten**:

## 1. Autopilot auf der Clifton
- Läuft direkt auf dem Fahrzeug (STM32 / Embedded System)  
- Steuert **Segelstellung, Flap und Kursregelung**  
- Verantwortlich für:
  - Zyklische Ausführung der Regelungen (`Run()`-Methode)  
  - Flap- und Kursregelung zur Einhaltung der Route  
  - Vortriebsregelung (Segeloptimierung)  
  - Zustands- und Fehlercodierung (binär)  
  - Sicherheit: Abbruch bei Grenzwerten  

## 2. Laptop-Software
- Verbindung zum Autopiloten über **UART / USB / Funk**  
- Verantwortlich für:
  - Telemetrie-Empfang und Logging  
  - Parametrierung des Autopiloten  
  - Start-/Stopp-Kommandos für Testfahrten  
- Dient als **Middleware** zwischen Fahrzeug und Frontend  

## 3. Frontend
- Visualisierung und Steuerung in Echtzeit  
- Funktionen:
  - Anzeige von Position, Geschwindigkeit, Kurs, Flap-Stellung  
  - Wegpunkt-Management und Missionsplanung  
  - Statuswarnungen und Log-Analyse  
- Kann lokal auf dem Laptop oder einem Browser ausgeführt werden  

## Zielsetzung
- Stabiler Autopilot, der autonom navigiert  
- Optimierung von Geschwindigkeit und Kurs unter realen Bedingungen  
- Echtzeit-Monitoring und Analyse über Frontend  
- Sammlung von Telemetrie-Daten zur Optimierung der Regelalgorithmen  

## Projektstruktur (Beispiel)
├─ autopilot/ # Embedded Code für Clifton
├─ laptop/ # Code für Laptop / Telemetrie & Steuerung
├─ frontend/ # GUI / Visualisierung
└─ README.md

## Kontakt
- **Autorin:** Tamara Boerner  
- **Projekt:** Autonomes Segeln – Clifton Autopilot  
- **Hochschule:** TH Lübeck, Studiengang Informatik/Softwaretechnik
