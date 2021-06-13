# Integration eines eingebetteten Systems in eine Cloud-Infrastruktur am Beispiel eines autonomen Schachtischs


## Ziel

Das Ziel dieser Arbeit ist es, einen Schach-Tisch zu konstruieren und programmieren, welcher in der Lage ist Schachfiguren autonom zu bewegen. Der Schwerpunkt liegt dabei insbesondere auf der Programmierung des eingebettenen Systems. Dieses besteht zum einem aus der Positionserkennung und Steuerung der Hardwarekomponenten (Schachfiguren) und zum anderen aus der Kommuniktation zwischen dem Tisch selbst und einem in einer Cloud befindlichen Server.


Mittels der Programmierung werden diverse Technologien von verschiedenen Einzelsystemen zu einem Gesamtprodukt zusammengesetzt.
Zu diesen Einzelsystemen gehören:

* Programmierung der Motorsteuerung, HMI (zB. Qt oder simple Buttons), NFC Tag erkennung
* Programmierung eines Wrappers für die Kommuniktion mit der Cloud (AWS)
* Statemaschiene und Implementierung der Spielflusssteuerung
* Backend mit Datenbankanbindung zwischen Server und Embedded-System
* Verwendung eines CI/CD Systems zum automatisierten bauen der Linux-Images für das Embedded-System




Die Grafik stellt das grundlegende System dar. Der Schachtisch selbst bildet ein kompaktes System, bestehend aus einem echtzeitfähigen STM32 als Steuerelement, welches zudem ein Interface mit Optionsverwaltung bietet. Die Mechanik selbst basiert auf Schrittmotoren, welche die Bewegung eines Moduls bestehend aus NFC-Lesers und einem Elektromagneten in der waagerechten x-y-Ebene ermöglicht um so unterhalb der Tischplatte die mit NFC-Tags versehenen Schachfiguren oberhalb der Tischplatte zu detektieren und bewegen. 

Zum einen ist es möglich, einen Spieler gegen einen Tisch als virtuellen Gegner spielen zu lassen. Es wird somit ein Spiel für eine Einzelperson ermöglicht. 

Zum anderen ist es möglich, zwei Tische miteinander zu verbinden und zwei Spieler geneinander spielen zu lassen. Die Tische dienen dann nur als Spiegel der Schachzüge. Die Züge des ersten Spielers werden dann auf den anderen Tisch (beim zweiten Spieler) gespiegelt und umgekehrt. So ist auch ein Spiel möglich, auch wenn sich die Spieler nicht am selben Ort befinden.

Jeder dieser Tische kommuniziert mittels RESTFUL Schnittstelle mit einem Server, welcher die vom Tisch erfassten Züge registriert und folgenden Züge berechnet. Aktuelle Positionen werden gespeichert und können von den Tischen angefragt werden. Mittels der Schach-AI (LC0) werden zudem Züge berechnet, was für ein Spiel gegen einen virtuellen Gegner nötig ist. 


## ERWARTETE FEATURES

* automatisches Bewegen der Spielfiguren des Gegenspielers
* automatisches Matchmaking
* simples Qt basierendes User Interface zum starten eines Spiels
* Spielmodi: MENSCH-MENSCH, MENSCH-AI
* Business-Logik auf zentralem Server, welcher in der Lage ist meherere Spiele gleichzeitig zu handeln
* Günstige User-Hardware (< 200€ pro Spieltisch, durch 3D Druck und Auslagerung der Logik in die Cloud)


## VORTEILE ZU BEREITS EXISTIERENDEN SYSTEMEN

* minimalistisches/intuitives Bedienkonzept
* automatisches Matchmaking basierend auf Skill des Spielers
* portables System ohne komplexen Aufbau
* einfach zu wartene Infrastruktur
* geringe Kosten auf der Spielerseite (einfaches embeddet system; Spiel-AI auf der Server-Seite)
* ausgeschiedene Figuren müssen nicht manuell vom Spielbrett entfernt werden


## THEMEN | MODULE

* DevOps CI/CD
* Docker / Container Virtualisierung
* Embedded Systeme (RPI / STM32) zur Steuerung der Figuren und dem Aufbau der Schaltung inkl Qt HMI
* Webtechnologien (Backend, WebApp) zur Spielflusssteuerung, Matchmaking
* Datenbanken (speicherung Spielstatus / Statisitken)
* IOT-Systeme (MQTT, Coap, Restful) zur Kommuniktation der einzelnen Komponenten
* System-Engeneering (Lastenheft, Projektplanung)

## GEPLANTE SCHRITTE

Jeder Schritt soll bereits eine grundlegende Dokumentation der Ergebnisse enthalten, um das letztliche erstellen der Gesamtdokumentation zu optimieren. 
Die hier dargestellte Planung basiert auf eine Laufzeit von 8 Wochen (da PO 15/16 max. 10 Wochen Laufzeit vorsieht)


### VORBEREITUNG

* Projektplanung
* ggf Lastenheft mit Abnahme
* erster 3D CAD-Prototyp zum verifizieren der mechanischen Machbarkeit
* testen von möglichen passenden Embedded-Systems (RPI + externen µC oder STM32MP1), kriterien: lauffähigkeit QT Quick 2, Schnittstellen, DiplayInterface
* ggf Materialbeschaffung ( zB TMC5160-BOB, lieferzeit 6 Wochen aktuell)

### PHASE_1 (3 Wochen)

* finalisierung 3D Design
* 3D Druck der Teile
* Materialbeschaffung / Bestellungen
* Server (OS, Basiskonfiguration) aufsetzten und einrichten
* CI System konfigurieren
* Aufsetzten des Embedded.Systems, installation notwendiger Software und bauen eines Basis-Images mit buildroot
* Erstellung eines einfach Backend-Server zum speichern der Spielstände
* Einbindung der Schach-AI in den Backend-Server zum berechnen des nächsten Spielzuges bei einem gegeben Spielbrett.
* Design des Protokolls zur Kommuniktation zwischen Spielbrett und Server (inkl. Authentifizierung, Initialisierung eines neuen Spiels)
* Programmierung des Motortreibers zum Anfahren aller möglichen Spielfigur-Postionen
* Programmierung des NFC Lesers zum erkennen der einzelnen Figuren

### PHASE_2 (3 Wochen)

* Zusammenbau zwei Spieltische
* Test der Kommunikation und der Initalisierung eines Spiel mit dem Server
* Programmierung der Matchmaking-Logic und das Verarbeiten der Spielzüge
* Implementierung des QT-UI, einbinden der Motorsteuerung
* Abschliessende Tests des Gesamsystems

### PHASE_3 (2 Wochen)

* Ausarbeitung, Gesamtdokumentation
* Bugfixing
* Live-Demo-Setup / Bugfixing



  

## ZU VERWENDENDE PROGRAMMIERSPRACHEN

* NodeJS (Backend, Matchmaking Logic, Spielbrett Logik)
* Python (Schack-Logik zur verifizierung getätigten Spielzüge)
* C/C++ (Motorsteuerung, QT-UI)

## GEPLANTE SOFTWARENUTZUNG

* Docker, Docker-Compose - Container Hosting
* Buildroot - automatisiertes bauen von Linux-Images mit allen benötigten Paketen
* Jenkins - CI/CD
* Code - PlattformIO
* Git / GitHub - VCS
* Fusion360 - CAD Software
* Hansoft - Projektplanung
* Jetrains Webstorm,PyCharm





## VIDEO - Ähnliche Projekte

* <https://www.youtube.com/watch?v=ta-q7Qbpj4Q>
* <https://www.youtube.com/watch?v=QEGJUZoUTCE&t=37s>
* <https://www.youtube.com/watch?v=dX37LFv8jWY>
* <https://www.youtube.com/watch?v=XCp4M08IWnA>
* <https://www.youtube.com/watch?v=OjJpzl3qlE8>




# FOLDER / BRANCH STRUCTURE

* buildroot
* controller
* documentation
* qtui
* server
* CI



