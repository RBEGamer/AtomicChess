# Einleitung

## Motivation




* Beginn: Er zieht die Aufmerksamkeit des Lesers durch die Schilderung des Ereignisses auf sich, das zu dem Problem geführt hat.
* Hintergrundinformationen (Herstellung des Kontexts): Gehe tiefer auf das Ereignis ein, indem du mehr Informationen über es vermittelst und dabei auch den Rahmen deiner Forschung skizzierst.
* Brücke zur Problemstellung: Erläutere, inwiefern es sich hierbei um ein Problem handelt, und schlage somit die Brücke zur Problemstellung, die deiner Untersuchung zu Grunde liegt.


## Zielsetzung

Das Ziel dieser Arbeit ist es, einen autonomen Schach-Tisch, welcher in der Lage ist Schachfiguren autonom zu bewegen und auf Benutzerinteraktion zu reagieren.

Der Schwerpunkt liegt dabei insbesondere auf der Programmierung des eingebettenen Systems.
Dieses besteht zum einem, aus der Positionserkennung und Steuerung der Hardwarekomponenten (Schachfiguren) und zum anderen aus der Kommunikation zwischen dem Tisch selbst und einem in einer Cloud befindlichen Server.

Mittels der Programmierung werden diverse Technologien von verschiedenen Einzelsystemen zu einem Gesamtprodukt zusammengesetzt. Zu diesen Einzelsystemen gehören:

* Programmierung der Motorsteuerung, HMI (zB. Qt oder simple Buttons), NFC Tag erkennung
* Programmierung eines Wrappers für die Kommunikation mit der Cloud (AWS)
* Statemaschiene und Implementierung der Spielflusssteuerung
* Backend mit Datenbankanbindung zwischen Server und Embedded-System
* Verwendung eines CI/CD Systems zum automatisierten bauen der Linux-Images für das Embedded-System


## Aufbau der Arbeit



beleuchtung existierender ansätze && festlegung zu erwartener Features, Kapitel x+1 zusammenführung in die DK HW,Kaptiel x+4 test und fazit,demonstration und validierung der funktionsfähigkeit





# Analyse bestehender Systeme und Machbarkeitsanalyse

## Existierende Systeme im Vergleich




### Kommerzielle Produkte


: Auflistung kommerzieller autonomer Schachtische

|                                         	| Square Off - Kingdom [@squareoffkingdom]	| Square Off - Grand Kingdom [@squareoffgrand]			| DGT Smart Board [@dtgsmartboard]	| DGT Bluetooth Wenge [@dtgble] |
|-------------------------------------------|-------------------------------------------|---------------------------------------------------|-----------------------------------|-------------------------------|
| Erkennung Schachfigurstellung           	| nein (Manuell per Ausgangsposition) 			| nein (Manuell per Ausgangsposition) 							| ja (Resonanzspulen)  							| ja ((+rfid))          				|
| Tischabmessungen (LxBxH)                	| 486mm x 486mm x 75mm              				| 671mm x 486mm x 75mm              								| 540mm x 540mm x 20mm 							| 540mm x 540mm x 20mm 					|
| Konnektivität                            	| (+ble)                               			| (+ble)                               							| (+usb) / Seriell        					| Bluetooth 2.0        					|
| Automatisches Bewegen der Figuren       	| ja                                				| ja                                								| nein                 							| nein                 					|
| Spiel Livestream                        	| ja                                				| ja                                								| ja                   							| ja                   					|
| Cloud anbindung (online Spiele)         	| ja (über Mobiltelefon + App)      				| ja (über Mobiltelefon + App)      								| ja (über PC + App)   							| ja (über PC + App)   					|
| Parkposition für ausgeschiedene Figuren 	| nein                              				| ja                                								| nein                 							| nein                 					|
| Stand-Alone Funktionalität               	| nein (Mobiltelefon erforderlich)    			| nein (Mobiltelefon erforderlich)     							| nein (PC erforderlich)						| nein (PC erforderlich)				|   	   
| Besonderheiten                          	| Akku für 30 Spiele                    		| Akku für 15 Spiele					           						| - 																| -															|


Bei den DGT Schachbrettern ist zu beachten, dass diese die Schachfiguren nicht autonom bewegen können. Sie wurden jedoch in die Liste aufgenommen, da diese einen Teil der Funktionalitäten der Square Off Schachbrettern abdecken und lediglich das bewegen der Schachfiguren fehlt.
Die DGT Bretter können die Position der Figuren erkennen und ermöglichen so auch Spiele über das Internet bzw. können diese auch  Spiele als Livestream anbieten.
Diese werden bei Schachtunieren für die Übertragung der Partien sowie die Aufzeichnung der Spielzüge verwendet und bieten support für den anschluss von weiterer Peripherie wie z.B. Schachuhren.


Somit gibt es zum Zeitpunkt der Recherche nur einen Hersteller von autonomen Schachbrettern.



### Open-Source Projekte

Bei allen Open-Source Projekten wurden die Features anhand der Beschreibung und der aktuellen Software extrahiert.
Besonders bei work-in-progress Projekten können sich die Features noch verändern und so weitere Funktionalitäten hinzugefügt werden.

Des Weiteren gibt es weitere derartige Projekte, in der Tabelle wurde nur diese Aufgelistet welche sich von anderen Projekten in mindestens einem Feature unterscheiden.

Auch existieren weitere Abwandlungen von autonomen Schachbrettern, bei welchem die Figuren von oberhalb des Spielbretts gegriffen bzw. bewegt werden. In einigen Projekten wird dies mittels eines Industrie-Roboters [@actprojectrobot] oder eines modifizierten 3D-Druckers[@atcproject3dprinter] realisiert, diese wurden hier nicht aufgrund der Mechanik welche über dem Spielbrett  montiert werden muss nicht berücksichtigt.

: Auflistung von Open-Source Schachtisch Projekten

|                                         		| Automated Chess Board (Michael Guerero) [@actproject1]	| Automated Chess Board (Akash Ravichandran) [@actproject2]	| DIY Super Smart Chessboard [@actproject3]	|
|---------------------------------------------|-------------------------------------------------------	|----------------------------------------------------------	|------------------------------------------	|
| Erkennung Schachfigurstellung              	| nein (Manuell per Ausgangsposition)     								| ja (Kamera / OpenCV)                       								| nein                       								|
| Tischabmessungen (LxBxH)                		| keine Angabe                            								| keine Angabe                               								| 450mm x 300mm x 50mm       								|
| Konnektivität                           		| (+usb)                                   								| Ethernet, (+wlan)                           							| Ethernet, (+wlan)          								|
| Automatisches Bewegen der Figuren       		| ja                                      								| ja                                         								| nein                       								|
| Spiel Livestream                        		| nein                                    								| nein                                       								| nein                       								|
| Cloud anbindung (online Spiele)         		| nein                                    								| nein                                       								| ja                         								|
| Parkposition für ausgeschiedene Figuren 		| nein                                    								| nein                                       								| nein                       								|
| Stand-Alone Funktionalität              		| nein (PC erforderlich)                   								| ja                                         								| ja                         								|
| Besonderheiten                          		| -                                       								| Sprachsteuerung (Amazon Alexa)             								| Zuganzeige über LED Matrix 								|
| Lizenz                                 			| (+gpl) 3+                                								| (+gpl)                                       							| -                          								|


In den bestehenden Projekten ist zu erkennen, dass ein autonomer Schachtisch sehr einfach und mit einfachsten Mittel konstruiert werden kann. Hierbei fehlen in der Regel einige Features wie das automatische Erkennen von Figuren oder das Spielen über das Internet.

Einige Projekte setzten dabei auf eingebettete Systeme, welche direkt im Schachtisch montiert sind, andere hingegen nutzen einen externen PC, welcher die Steuerbefehle an die Elektronik sendet.

Bei der Konstruktion der Mechanik und der Methode mit welcher die Figuren über das Feld bewegt werden ähneln sich jedoch die meisten dieser Projekte. Hier wird meist eine einfache X und Y-Achse verwendet, welche von zwei Schrittmotoren bewegt werden. Die Schachfiguren werden dabei mittels eines Elektromagneten über die Oberseite gezogen. Hierbei ist ein Magnet oder eine kleine Metallplatte in den Fuß der Figuren eingelassen worden.

Die Erkennung der Schachfiguren ist augenscheinlich die schwierigste Aufgabe. Hier wurde in der Mehrzahl der Projekte eine Kamera im Zusammenspiel mit einer auf OpenCV basierenden Figur-Erkennung. Diese Variante ist je nach Implementierung des Vision-Algorithmus fehleranfälliger bei sich ändernden Lichtverhältnissen, auch muss die Kamera oberhalb der Schachfiguren platziert werden, wenn kein transparentes Schachfeld verwendet werden soll.



## User Experience

Ein wichtiger Aspekt bei diesem Projekt spiel die User-Experience. Diese beschreibt die Ergonomie der Mensch-Maschine Interaktion und wird durch die DIN 9241[@din9241] beschrieben.
Hierbei geht es primär um das Erlebnis, welches der Benutzer bei dem Verwenden eines Produktes erlebt und welche Erwartungen der Benutzer an die Verwendung des Produktes hat.

Bei dem autonomen Schachtisch, soll der Benutzer eine ähnlich einfache Erfahrung erleben, wie bei einer Schachpartie mit einem menschlichen Gegenspieler.
Der Benutzer soll direkt nach dem einschalten des Tisches und dem Aufstellen der Figuren in der Lage sein, mit dem Spiel beginnen zu können. Dies soll wie ein reguläres Schachspiel ablaufen, der Spieler vor dem Tisch soll die Figuren mit der Hand bewegen können und der Tisch soll den Gegenspieler darstellen. Dieser bewegt die Figuren der Gegenseite.

Nach Beendigung einer Partie, soll das Spielbrett wieder in die Ausgangssituation gebracht werden, die kann zum einem vom Tisch selber oder vom Benutzer per Hand geschehen. Danach ist der Tisch für die nächste Partie bereit, welche einfach per Knopfdruck gestartet werden können sollte.

Ein weiter Punkt welcher bei der User-Experience beachtet werden soll, ist die zeitliche Konstante. Ein Spiel auf einem normalen Schachspiel hat je nach Spielart kein Zeitlimit, dies kann für das gesamte Spiel gelten oder auch für die Zeit zwischen einzelnen Zügen.
Der autonome Schachtisch soll es dem Spieler z.B. ermöglichen ein Spiel am Morgen zu beginnen und erst am nächsten Tag dieses fortzusetzen.




## Anforderungsanalyse

alle key requirements welcher der tisch haben soll


## Machbarkeitsanalyse

welche technologien werden benötigt

### Technologien im Makerspace

stehen diese im makerspace zur verfüfung



###




# Erstellung erster Prototyp

## Technologieauswahl für ersten Protoypen




#

# Vinaque sanguine metuenti cuiquam Alcyone fixus

## Aesculeae domus vincemur et Veneris adsuetus lapsum

Lorem markdownum Letoia, et alios: figurae flectentem annis aliquid Peneosque ab
esse, obstat gravitate. Obscura atque coniuge, per de coniunx, sibi **medias
commentaque virgine** anima tamen comitemque petis, sed. In Amphion vestros
hamos ire arceor mandere spicula, in licet aliquando.

```java
public class Example implements LoremIpsum {
	public static void main(String[] args) {
		if(args.length < 2) {
			System.out.println("Lorem ipsum dolor sit amet");
		}
	} // Obscura atque coniuge, per de coniunx
}
```

Listing: TEST
```c++
// Your First C++ Program

#include <iostream>

int main() {
    std::cout << "Hello World!";
    return 0;
}

}
```


Porrigitur et Pallas nuper longusque cratere habuisse sepulcro pectore fertur.
Laudat ille auditi; vertitur iura tum nepotis causa; motus. Diva virtus! Acrota
destruitis vos iubet quo et classis excessere Scyrumve spiro subitusque mente
Pirithoi abstulit, lapides.

## Lydia caelo recenti haerebat lacerum ratae at

Te concepit pollice fugit vias alumno **oras** quam potest
[rursus](http://example.com#rursus) optat. Non evadere orbem equorum, spatiis,
vel pede inter si.

1. De neque iura aquis
2. Frangitur gaudia mihi eo umor terrae quos
3. Recens diffudit ille tantum

\begin{equation}\label{eq:neighbor-propability}
    p_{ij}(t) = \frac{\ell_j(t) - \ell_i(t)}{\sum_{k \in N_i(t)}^{} \ell_k(t) - \ell_i(t)}
\end{equation}

Tamen condeturque saxa Pallorque num et ferarum promittis inveni lilia iuvencae
adessent arbor. Florente perque at condeturque saxa et ferarum promittis tendebat. Armos nisi obortas refugit me.

> Et nepotes poterat, se qui. Euntem ego pater desuetaque aethera Maeandri, et
[Dardanio geminaque](http://example.com#Dardanio_geminaque) cernit. Lassaque poenas
nec, manifesta $\pi r^2$ mirantia captivarum prohibebant scelerato gradus unusque
dura.

- Permulcens flebile simul
- Iura tum nepotis causa motus diva virtus Acrota. Tamen condeturque saxa Pallorque num et ferarum promittis inveni lilia iuvencae adessent arbor. Florente perque at ire arcum.



## Image with Caption

![Kalibrierungeschema der Mechanik zeigt welche Abstände in der Konfiguration eigetragen werden müssen](images/ATC_Calibration_Guide.png)
