# Einleitung

## Motivation



In der heutigen Zeit m




* Beginn: Er zieht die Aufmerksamkeit des Lesers durch die Schilderung des Ereignisses auf sich, das zu dem Problem geführt hat.
* Hintergrundinformationen (Herstellung des Kontexts): Gehe tiefer auf das Ereignis ein, indem du mehr Informationen über es vermittelst und dabei auch den Rahmen deiner Forschung skizzierst.
* Brücke zur Problemstellung: Erläutere, inwiefern es sich hierbei um ein Problem handelt, und schlage somit die Brücke zur Problemstellung, die deiner Untersuchung zu Grunde liegt.


## Zielsetzung

Das Ziel der nachfolgenden Arbeit ist es, einen autonomen Schachtisch zu entwickeln, welcher in der Lage ist, Schachfiguren autonom zu bewegen und auf Benutzerinteraktion zu reagieren.
Der Schwerpunkt liegt dabei insbesondere auf der Programmierung des eingebetteten Systems und dem Zusammenspiel von diesem mit einem aus dem Internet erreichbaren Servers, welcher als Vermittlungsstelle zwischen verschiedenen Schachtischen und anderen Endgeräten dient.
Dieses besteht zum einem aus der Positionserkennung und Steuerung der Hardwarekomponenten (Schachfiguren) und zum anderen aus der Kommunikation zwischen dem Tisch selbst und einem in einer Cloud befindlichem Server.
Mittels der Programmierung werden diverse Technologien von verschiedenen Einzelsystemen zu einem Gesamtprodukt zusammengesetzt.

## Methodik

Im ersten Abschnitt werden die zum Zeitpunkt existierenden Ansätze und deren Umsetzung beleuchtet. Hier wurde insbesondere darauf geachtet
Anschliessend werden die zuvor verwendeten Technologien betrachtet, welche bei bei den beiden darauffolgenden Prototypen verwendet werden sollen. Hierbei stehen insbesondere solche Technologien im Vordergrund der Untersuchung welche möglichst einfach zu Beschaffen sind und im besten Fall einer breiten Masse zur Verfügung stehen.

Das sechste Kapitel widmet sich der Realisierung des ersten Protoypen des autonomen Schachtischs.
Hier werden die Erkenntnisse der zuvor evaluierten Technologien verwendet um ein Modell zu entwickeln welches den im ersten Abschnitt erarbeiteten Vorgaben entspricht. Der nach der Implementierung durchgeführte Dauertest, zeigt anschliessend mögliche Probleme auf.

Im anschließenden Kapitel, wird auf der Basis des ersten Prototyps und seiner im Betrieb auftretenden Probleme, der finale Prototyp entwickelt.
Hier werden die Probleme durch die Vereinfachung der Elektronik sowie der Mechanik gelößt.
Die Zuverlässigkeit wurde hier durch stetige Testläufe mit kontrollierten Schachzug-Szenarien überwacht
und so ein produktreifer Prototyp entwickelt.

Im darauffolgenden Abschnitt wird die Cloud-Infrastruktur thematisiert, welche für eine Kommunikation zwischen den autonomen Schachtischen entscheidend ist.
Auch wird hier die Software, welche auf dem eingebetteten System ausgeführt im Details beschrieben und deren Kommunikation mit der Cloud-Infrastruktur, sowie mit den elektrischen Komponenten.




# Analyse bestehender Systeme und Machbarkeitsanalyse

## Existierende Systeme im Vergleich

* Nischenprodukt, jedoch einige Projekte im OpenSource bereich verfügbar
* Ein kommerzieller Hersteller

### Kommerzielle Produkte


: Auflistung kommerzieller autonomer Schachtische

|                                         	| Square Off - Kingdom [@squareoffkingdom]	| Square Off - Grand Kingdom [@squareoffgrand]		| DGT Smart Board [@dtgsmartboard]	| DGT Bluetooth Wenge [@dtgble] |
|-------------------------------------------|-------------------------------------------|---------------------------------------------------|-----------------------------------|-------------------------------|
| Erkennung Figurstellung           	    | nein (Manuell per Ausgangsposition) 		| nein (Manuell per Ausgangsposition) 				| ja                  				| ja                     		|
| Abmessungen (LxBxH)                		| 486mm x 486mm x 75mm              		| 671mm x 486mm x 75mm              				| 540mm x 540mm x 20mm 				| 540mm x 540mm x 20mm 			|
| Konnektivität                            	| Bluetooth                            		| Bluetooth                         				| Seriell           			    | Bluetooth         			|
| Automatisches Bewegen der Figuren       	| ja                                		| ja                                				| nein                 				| nein                 			|
| Spiel Livestream                        	| ja                                		| ja                                				| ja                   				| ja                   			|
| Cloud anbindung (online Spiele)         	| ja (Mobiltelefon + App)      		        | ja (Mobiltelefon + App)      				        | ja (PC + App)   				    | ja (PC + App)      			|
| Parkposition für ausgeschiedene Figuren 	| nein                              		| ja                                				| nein                 				| nein                 			|
| Stand-Alone Funktionalität               	| nein (Mobiltelefon erforderlich)    		| nein (Mobiltelefon erforderlich)     				| nein (PC erforderlich)			| nein (PC erforderlich)		|   	   
| Besonderheiten                          	| Akku für 30 Spiele                    	| Akku für 15 Spiele					           	| - 								| -								|


Bei den DGT-Schachbrettern ist zu beachten, dass diese die Schachfiguren nicht autonom bewegen können. Sie wurden jedoch in die Liste aufgenommen, da diese einen Teil der Funktionalitäten der Square Off Schachbrettern abdecken und lediglich die automatische Bewegung der Schachfiguren fehlt.
Die DGT-Bretter können die Position der Figuren erkennen und ermöglichen so auch Spiele über das Internet; diese können sie auch als Livestream anbieten.
Bei Schachturnieren werden diese für die Übertragung der Partien sowie die Aufzeichnung der Spielzüge verwendet und bieten Support für den Anschluss von weiterer Peripherien wie z.B. Schachuhren.


Somit gibt es zum Zeitpunkt der Recherche nur einen Hersteller von autonomen Schachbrettern, welcher auch die Figuren bewegen kann.



### Open-Source Projekte

Bei allen Open-Source Projekten wurden die Features anhand der Beschreibung und der aktuellen Software extrahiert.
Besonders bei Projekten welche sich noch in der Entwicklung befinden, können sich die Features noch verändern und so weitere Funktionalitäten hinzugefügt werden. Alle Features der Projekte wurden zum Zeitpunkt der Recherche analysiert und Dokumentiert und wurden im Laufe der Entwicklung nicht weiter aktualisiert.

Zusätzlich zu den genannten Projekten sind weitere derartige Projekte verfügbar; in der Tabelle wurde nur jene aufgelistet, welche sich von anderen Projekten in mindestens einem Feature unterscheiden.

Auch existieren weitere Abwandlungen von autonomen Schachbrettern, bei welchem die Figuren von oberhalb des Spielbretts gegriffen bzw. bewegt werden. In einigen Projekten wird dies mittels eines Industrie-Roboters [@actprojectrobot] oder eines modifizierten 3D-Druckers[@atcproject3dprinter] realisiert. Diese wurden hier aufgrund der Mechanik, welche über dem Spielbrett montiert werden muss, nicht berücksichtigt.

: Auflistung von Open-Source Schachtisch Projekten

|                                         		| Automated Chess Board (Michael Guerero) [@actproject1]| Automated Chess Board (Akash Ravichandran) [@actproject2]	| DIY Super Smart Chessboard [@actproject3]	|
|---------------------------------------------	|-------------------------------------------------------|----------------------------------------------------------	|------------------------------------------	|
| Erkennung Figurstellung              	        | nein (Manuell per Ausgangsposition)     				| ja (Kamera / OpenCV)                       				| nein                       				|
| Abmessungen (LxBxH)                			| keine Angabe                            				| keine Angabe                               				| 450mm x 300mm x 50mm       				|
| Konnektivität                           		| (+usb)                                   				| (+wlan)                           				        | (+wlan)          			            	|
| Automatisches Bewegen der Figuren       		| ja                                      				| ja                                         				| nein                       				|
| Spiel Livestream                        		| nein                                    				| nein                                       				| nein                       				|
| Cloud-Anbindung (online Spiele)         		| nein                                    				| nein                                       				| ja                         				|
| Parkposition für ausgeschiedene Figuren 		| nein                                    				| nein                                       				| nein                       				|
| Stand-Alone Funktionalität              		| nein (PC erforderlich)                   				| ja                                         				| ja                         				|
| Besonderheiten                          		| -                                       				| Sprachsteuerung (Amazon Alexa)             				| Zuganzeige über LED Matrix 				|
| Lizenz                                 		| (+gpl) 3+                                				| (+gpl)                                       				| -                          				|


In den bestehenden Projekten ist zu erkennen, dass ein autonomer Schachtisch sehr einfach und mit simplen Mittel konstruiert werden kann. Hierbei fehlen in der Regel einige Features, wie das automatische Erkennen von Figuren oder das Spielen über das Internet.

Einige Projekte setzten dabei auf eingebettete Systeme, welche direkt im Schachtisch montiert sind, andere hingegen nutzen einen externen PC, welcher die Steuerbefehle an die Elektronik sendet.

Bei der Konstruktion der Mechanik und der Methode mit welcher die Figuren über das Feld bewegt werden ähneln sich jedoch die meisten dieser Projekte. Hier wird in der Regel eine einfache X- und Y-Achse verwendet, welche von zwei Schrittmotoren bewegt werden.
Die Schachfiguren werden dabei mittels eines Elektromagneten über die Oberseite gezogen. Hierbei ist ein Magnet oder eine kleine Metallplatte in den Fuß der Figuren eingelassen worden.

Die Erkennung der Schachfiguren ist augenscheinlich die schwierigste Aufgabe. Hier wurde in der Mehrzahl der Projekte eine Kamera im Zusammenspiel mit einer auf OpenCV basierenden Figur-Erkennung verwendet.
Diese Variante ist je nach Implementierung des Vision-Algorithmus fehleranfälliger bei sich ändernden Lichtverhältnissen, auch muss die Kamera oberhalb der Schachfiguren platziert werden, wenn kein transparentes Schachfeld verwendet werden soll.

Eine weitere Alternative ist die Verwendung einer Matrix aus Reed-Schaltern oder Halleffekt-Sensoren. Diese werden in einer 8x8 Matrix Konfiguration unterhalb der Platte montiert und reagieren auf die Magnete in den Figuren. So ist es möglich zu erkennen, welches der Schachfelder belegt ist, jedoch nicht konkret von welchem Figurtypen.
Dieses Problem wird durch eine definierte Ausgangsstellung beim Spielstart gelöst. Nach jedem Zug durch den Spieler und der dadurch resultierenden Änderungen in der Figurpositionen in der Matrix können die neuen Figurstellungen berechnet werden.




## User Experience

Ein wichtiger Aspekt bei diesem Projekt stellt die User-Experience dar. Diese beschreibt die Ergonomie der Mensch-Maschine-Interaktion und wird durch die DIN 9241[@din9241] beschrieben.
Hierbei geht es primär um das Erlebnis, welches der Benutzer bei dem Verwenden eines Produktes erlebt und welche Erwartungen der Benutzer an die Verwendung des Produktes hat.

Bei dem autonomen Schachtisch, soll der Benutzer eine ähnlich authentische Erfahrung erleben wie bei einer Schachpartie mit einem menschlichen Gegenspieler.
Der Benutzer soll direkt nach dem Einschalten des Tisches und dem Aufstellen der Figuren in der Lage sein, mit dem Spiel beginnen zu können. Dies soll wie ein reguläres Schachspiel ablaufen; der Spieler vor dem Tisch soll die Figuren mit der Hand bewegen können und der Tisch soll den Gegenspieler darstellen.
Dieser bewegt die Figuren der Gegenseite.

Nach Beendigung einer Partie, soll das Spielbrett wieder in die Ausgangssituation gebracht werden; dies kann zum einem vom Tisch selbst oder vom Benutzer manuell geschehen.
Danach ist der Tisch für die nächste Partie bereit, welche einfach per Knopfdruck gestartet werden können sollte.

Dies soll auf für abgebrochene Spiele gelten, welche von Benutzer oder durch das System abgebrochen werden. Hierbei soll das Schachbrett sich ebenfalls selbständig zurücksetzten können.

Ein weiter Punkt welcher bei der User-Experience beachtet werden soll, ist die zeitliche Konstante. Ein Spiel auf einem normalen Schachspiel hat je nach Spielart kein Zeitlimit, dies kann für das gesamte Spiel gelten oder auch für die Zeit zwischen einzelnen Zügen.
Der autonome Schachtisch soll es dem Spieler z.B. ermöglichen ein Spiel am Morgen zu beginnen und dieses erst am nächsten Tag fortzusetzen.

Auch muss sich hier die Frage gestellt werden, was mit den ausgeschiedenen Figuren geschieht. Bei den autonomen Schachbrettern von Square Off[@squareoffgrand], werden die Figuren an die Seite auf vordefinierte Felder bewegt und können so wieder bei der nächsten Partie vom System aufgestellt werden. Viele andere Projekte schieben die Figuren auf dem Feld heraus, können diese aber im Anschluss nicht mehr gezielt in das Feld zurückholen. So muss diese Aufgabe vom Benutzer geschehen. Auch wir diese Funktionalität von einigen Projekten nicht abgedeckt und der Benutzer muss die Figuren selbständig vom Feld entfernen.



## Anforderungsanalyse

Nach Abschluss der Recherche, kann somit eine Auflistung aller Features angefertigt werden, welche ein autonomer Schachtisch haben sollte.
Hierbei werden vor allem Funktionalitäten berücksichtig, welche die Bedienung und Benutzung des autonomen Schachtisch dem  Benutzer einen Mehrwert in puncto Benutzerfreundlichkeit bieten.

: Auflistung der Anforderungen an den autonomen Schachtisch

|                                         	| (+atc)                               	|
|-----------------------------------------	|-----------------------------------	  |
| Erkennung Schachfigurstellung           	| ja                                 	  |
| Konnektivität                           	| (+wlan), (+usb)                   	  |
| Automatisches Bewegen der Figuren       	| ja                                	  |
| Spiel Livestream                        	| ja                                	  |
| Cloud anbindung (online Spiele)         	| ja                                	  |
| Parkposition für ausgeschiedene Figuren 	| ja                                	  |
| Stand-Alone Funktionalität              	| ja (Bedienung direkt am Tisch)    	  |
| Besonderheiten                          	| visuelle Hinweise per Beleuchtung 	  |


Die Abmessungen und das Gewicht des autonomen Schachtisches, ergeben sich aus mechanischen Umsetzung und werden hier aufgrund der zur Verfügung stehenden Materialen und fertigungstechniken nicht festgelegt.
Dennoch wird Wert darauf gelegt, dass das Verhältnis zwischen den Spielfeldabmessungen und den Abmessungen des Tisches so gering wie möglich ausfällt. Auch müssen die Figuren für den Benutzer eine gut handhabbare Grösse aufweisen um ein angenehmes haptisches Spielerlebnis zu gewährleisten. Auch wird kein besonderer Augenmerk auf die Geschwindigkeit der Figurbewegeungen gelegt, da hier die Zuverlässigkeit und Wiederholgenauigkeit dieser im Vordergrund stehen.




## Machbarkeitsanalyse

* welche technologien werden benötigt
* software architektur anfoderungen
* hardware anforderungen
* grosse
* wiederholgenauigkeit
* lautstärke
* vorerfahrnung in cad ed druck und schaltungsdesign









# Grundlegende Verifikation der ausgewälten Technologien

## Erprobung Buildroot-Framework
* erstellen eines einfachen images für das embedded System
* inkl ssh Server und SFTP
* qt 5 libraries
* eigenes package atctp
* test der toolchain


## Verifikfation NFC Technologie

* warum gewählter nfc reader => ndef lesen
* reicheweiten test mit 22mm
* test mit benachbarten figuren
* warum kein RFID => keine speicherung von id auf der controller seite
* selbherstellung von eigenen figuren ohne modifikation der controllerseite

![Grove PN532 NFC Reader mit Kabelgebundener Antenne \label{ATC_nfc_range_test}](images/ATC_nfc_range_test.png)

* test mit figuren nebeneinander




## Schrittmotor / Schrittmotorsteuerung
* warum => einfache ansteuerung
* keine STEP DIR somit muss embedded nicht echtzeitfähigsein und kann ggf auch andere task abbarbeiten
* TMC schrittmotortreiber spi configuration
* und goto move  => wait for move finished irw testen
* dafür einfacher python testreiber geschribene
* schrittverlust nicht zu erwarten



## 3D Druck für den mechanischen Aufbau


Da es sich hier nur um einen Protoypen handelt, wurde hier auf ein einfach zu handbabendes Filament vom Typ (+pla) zurückgegriffen.
Dieses ist besonders gut für die Prototypenendwicklung geeignet und kann mit nahezu jeden handelsüblichen (+fdm) 3D-Drucker verarbeitet werden.

Zuvor wurden einige Testdrucke durchgeführt um die Qualität der zuvor gewählten Druckparameter zu überprüfen und diese gegebenenfalls anzupassen.
Auch wurden verschiedene weitere Bauteile gedruckt, an welchen die Toleranzen für die späteren (+cad) Zeichnungen abgeschätz werden können.
Dies betrifft vor allem die Genuigkeit der Bohrungen in den gefertigten Objekten, da hier später Bolzen und Schrauben ein nahezu spielfrei eingeführt werden müssen.
Ein Test, welcher die Machbarkeit von Gewinden zeigt wurde nicht durchgeführt, da alle Schrauben später mit der passenden Mutter gesichert werden sollen.
So soll eine Abnutzung durch häufige Montage der gedruckten Bauteile verhindert werden.

Bei dem Design der zu druckenden Bauteile wurde darauf geachtet, dass diese den Bauraum von 200x200x200mm nicht überschreiten und somit auch von einfachen (+fdm) 3D-Druckern erstellt werden können.

Als Software wurde der Open-Source Slicer Ultimaker Cura [@ultimakercura] verwendet, da dieser zum einen bereits fertige Konfigurationen für den verwendeten 3D-Drucker enhält und zum anderen experimentelle Features bereitstellt.

![3D Druck: Objekt (rot,gelb,grün),Tree Structure (cyan) \label{3d_print_tree_structure}](images/3d_print_tree_structure.png)

Hier wurde für die Bauteile, welche eine Sützstruktur benötigen, die von Cura bereitgestellte Tree Support Structure aktiviert. \ref{3d_print_tree_structure}
Diese bietet den Vorteil gegenüber anderen Stützstrukturen, dass sich diese leichter entfernen lässt und weniger Rückstände an den Bauteilen hinterlässt.
Diese Vorteile wurde mit verschiedenen Testdrucken verifiziert und kommen insbesondere bei komplexen Bauteilen mit innenliegenden Elementen zum tragen bei denen eine Stützstruktur erforderlich sind.



: Verwendete 3D Druck Parameter. Temperatur nach Herstellerangaben des verwendeten PLA Filament.

| Ender 3 Pro 0.4mm Nozzle 	| PLA  Settings 	|
|--------------------------	|---------------	|
| Layer Height             	| 0.2mm         	|
| Infill                   	| 50.00%        	|
| Wall Thickness           	| 2.0mm         	|
| Support Structure        	| Tree          	|
| Top Layers               	| 4             	|
| Bottom Layers            	| 4             	|

Zusätzliche Parameter wie die Druckgeschwindigkeit, sind hierbei individuell für den zu gewählten 3D Drucker zu ermitteln. Allgemein wurden hier die Standarteinstellungen verwendet, welche in diesem Falle einen guten Kompromiss zwischen  Qualität und Druckzeit lieferten.








# Erstellung erster Prototyp

* proof of conzept
*

![Prototyp Hardware: Erster Prototyp des autonomen Schachtisch \label{dk}](images/table_images/dk.png)






## Mechanik



* vorgaben IKEA tisch als grundbasis => bereits fertiger grundrahmen in denen die einzelteile integriert werden können
* xy riemen führung
* spiel in Mechanik
* Einabrietung in Fusion360
* Cad design aller bauteile
* standartxy
* erweituerng des spielraums durch zwei Magnete



## Parametrisierung Schachfiguren

Da das System die auf dem Feld befindlichen Schachfiguren anhand von (+nfc) Tags erkennt, müssen diese zuerst mit Daten beschrieben werden.
Die verwendeten NXP NTAG 21 Chips, besitzen einen vom Benutzer verwendbaren Speicher von 180 Byte. Dieser kann über ein (+nfc)-Lese/Schreibgerät mit Daten verschiednster Art beschrieben und wieder ausgelesen werden.
Moderne Mobiltelefone besitzen in der Regel auch die Fähigkeit mit passenden (+nfc) Tags kommunizieren zu können; somit sind keine Stand-Alone Lesegeräte mehr notwendig.

Der Schachtisch verwendet dabei das (+ndef) Dateiformat welches Festlegt, wie die Daten auf dem (+nfc) Tag gespeichert werden. Da diesen ein Standatisiertes Format ist, können alle gängigen Lesegeräte und Chipsätze diese Datensätze lesen. Der im autonomen Schachtisch verwendete Chipsatz PN532 von NXP ist dazu ebenfalls in der Lage.

Um das (+ndef) Format verwenden zu können, müssen die (+nfc) Tags zuerst auf diese Formatiert werden. Die mesten käuflichen Tags sind bereits derart formatiert. Alternativ kann dies mittels Mobiltelefon und passender App geschehen.
Da (+ndef) Informationen über die Formatierung und der gepeicherten Einträge speichert, stehen nach der Formatierung nur noch 137 Bytes des NXP NTAG 21 zur verfügung.

Per Lesegerät können anschliessend mehrere (+ndef) Records auf den Tag geschrieben werden. Diese sind mit Dateien auf einer Festplatte vergleichbar und können verschiedenen Dateiformate und Dateigrössen annehmen.
Ein typischer Anwendungsfall ist der (+ndefrtd) URL Datensatz. Dieser kann dazu genutzt werden eine spezifizierte URL auf dem Endgeräte aufzurufen, nachdem der (+nfc) Tag gescant wurde.[@nordicnfclibndef]

Der autonome Schachtisch verwendet den einfachsten (+ndefrtd) Typ, welcher der Text-Record ist, und zum speichern von Zeichenketten genutzt werden kann, ohne das eine Aktion auf dem Endgerät ausgeführt wird.
Jeder Tag einer Schafigur, welche für den autonomen Schachtisch verwendet werden kann, besitzt diesen (+ndef) Record an der ersten Position. Alle weiteren eventuell vorhandenen Records werden vom Tisch ignoriert.[@nordicnfclib]

![Prototyp Hardware: Tool zur Erstellung des NDEF Payloads: ChessFigureIDGenerator.html \label{ATC_ChessFigureIDGenerator}](images/ATC_ChessFigureIDGenerator.png)

Um die Payload für den (+nfc) Record zu erstellen wurde ein kleine Web-Applikation erstellt, welche den Inhalt der Text-Records erstellt.
Dieser ist für jede Figur individuell und enthält den Figur-Typ und die Figur-Farbe. Das Tool unterstüzt auch das Speichern weiterer Attribute wie einem Figur-Index, welcher aber in der finalen Software-Version nicht genutzt wird. \ref{ATC_ChessFigureIDGenerator}

Nach dem Beschreiben eines (+nfc) Tags ist es zusätzlich möglich, diesen gegen auslesen mittels einer Read/Write-Protection zu schützen.
Diese Funktionalität wird nicht verwendet um das Kopieren einzelner Figuren durch den Benutzer zu ermöglichen.
Somit kann dieser leicht seine eigenen Figuren erschaffen, ohne auf das Tool angewiesen zu sein.
Auch ist es möglich so möglich verschiedene Figur-Sets zu mischen, so kann ein Spieler verschiedene Sets an Figuren mit dem autonomen Schachtisch verwenden.

![Prototyp Hardware: NDEF Text Record Payload für einen weissen Turm \label{ndef_record_rook}](images/ndef_record_rook.png)


## Schaltungsentwurf

![Prototyp Hardware: Blockdiagramm \label{ATC_Hardware_Architecture_DK}](images/ATC_Hardware_Architecture_DK.png)

* auswahl der Motortreiber (leise, bus ansteuerung)
* ansteuerung pn532 und umsetzung auf uart
* platinendesign
* ansterung elektromagnetet


### Implementierung HAL

* ansteuerung des TMC5160
* ansterung des Microncontollers (PN532, LED)
* integration in controller software
* welche funktion stehen bereit tabelle

## Fazit zum ersten Prototypen

* nicht für production geeignet
* aufbau und calibrierung langwiehrig
* trotzdem robustes design auf kleinem formfaktor
* verwendeten elektromagnete nicht stark genug, somit über aqusserhalb der specs betrieben was zu temeraturproblemen führte
* gewicht der Figuren zu klein bzw magnete zu start
* workarounds in der software nötig durch die beiden magnete
* nicht die beste entscheidung direkt auf grösse zu optimieren








# Erstellung zweiter Prototyp

![Producation Hardware: Finaler autonomer Schachtisch \label{prod}](images/table_images/prod.png)

## Modifikation der Mechanik

* dauertest hat gezeigt dass mechnik zu viel spiel aufweisst
* Motorenhalterung der y achse schränkt des bewegungsspielraum um mehr als 10cm ein, welches zu einem unwesentlichen grösseren verhältnis von Spielfeldgrösse und Abmessungen des Schachtischs
* CoreXY bietet Vorteil:
* Motoren fest am rahmen => weniger kabel + gewicht an der Y Achse
* jedoch komplexerer Aufwand der riemenverlegung so komplexere 3d bauteile
* Tischabmessungen 620x620mm dabei Bewegungsspielraum vom 580x580 zuvor nur 480x480



## Optimierungen der Spielfiguren

Die bisherigen genutzten vorgefertigten Figuren funktionierten mit dem ersten Prototyp problemlos.
Sie wiesen aber trotzdem eine zu hohe Fehleranfälligkeit, im Bezug auf das gegenseitige Beeinflussen (abstoßen, anziehen) durch die verwendeten Magnete auf.

Die Größe der Figuren kann durch die fest definierte Feldgrösse von 55mm und der verwendeten (+nfc) Tags nicht verändert werden.
Nach einigen Testdurchläufen mit dem ersten Prototyp war zu erkennen, dass sich die Figuren je nach aktueller Situation auf dem Spielfeld weiterhin magnetisch anziehen.
Weiterhin wurden verschiedenen Bewegungsgeschwindikeiten getestet, brachten hierbei keine Verbesserung der Situation

Dies führt je nach Spielverlauf zu Komplikationen, sodass die Figuren manuell vom Benutzer wieder mittig auf den Felder platziert werden müssen.

Um dies zu verhindern, wurde einige Figuren zusätzlich mit einer 20mm Unterlegscheibe am Boden beschwert, welche diese Problem behob, jedoch das (+nfc) Tag nicht mehr als lesbar erwies.
Dies resultierte in der Idee die Schachfiguren ebenfalls selbst mit dem 3D-Drucker herzustellen und die Magnete direkt in den Boden der Figur einlassen zu können.

Die aktuell verwendeten Figuren des ersten Protoyp wiegen 8 Gramm für die Bauern und 10 Gramm für die restlichen Figuren.
Der Test mit der Unterlegscheibe ergab das diese mit 5 Gramm zusätzlich, genug Gewicht hinzufügte um die magnetische Beeinflussung zu unterbinden.

Testweise wurden einige Figuren mittels 3D Drucker erstellt um so das Gewicht zu erhöhen.
Nach einem erfolgreichen Test wurde das (+cad) Modell wurde so angepasst, dass sich der Magnet direkt in den Boden der Figur einkleben lässt.
Des Weiteren wurden bei den Bauern die Magnete ausgetauscht. Die zuerst verwendeten 10x3mm Neodym-Magnete wurden bei diesen Figuren gegen 6x3mm Magnete getauscht.
Somit sind im Design zwei verschiedenen Arten von Magneten notwendig, jedoch traten in den anschliessend durchgeführten Testläufen keine Beeinflussungen mehr auf.

## Änderungen der Elektronik

Mit ein relevanter Kritikpunkt, welcher bereits während des Aufbaus des ersten Protoypen zu erkennen war, ist die Umsetzung der Elektronik.
Diese wurde im ersten Prototyp manuell Aufgebaut und enthielt viele verschiedene Komponenten.

Die verwendeten Motortreiber stellten sich während der Entwicklung als sehr flexibel heraus, stellten aber auch einen grossen Kostenfaktor dar.
Nach dem Aufbau und Erprobung des ersten Prototyps wurde ersichtlich, dass hier nicht alle zuerst angedachten Features der Treiber benötigt werden und so auch andere alternativen in Frage kommen.
Zusätzlich konnte die Elektronik nur beschränkt mit anderen System verbunden werden, welches insbesondere durch die verwendete (+spi) Schnittstelle geschuldet war.


All diese Faktoren erschweren einen einfachen Zusammenbau des autonomen Schachtischs. Die Lösung stellt die Verwendung von Standardhardware dar.
Nach dem Herunterbrechen der elektrischen Komponenten und des mechanischen Aufbaus ist zu erkennen, dass der autonome Schachtisch einer CNC-Fräse bzw eines 3D Drucker stark ähnelt.
Insbesondere die XY-Achsen Mechanik sowie die Ansteuerung von Schrittmotoren, wird in diesen Systemen verwendet.
Mit den Durchbruch von 3D Druckern im Consumerbereich, sind auch kleine und  preisgünstige Steuerungen erhältlich, welche 2-3 Schrittmotoren und einiges an zusätzlicher Hardware ansteuern können.

: Standardhardware 3D Drucker Steuerungen

|                	| SKR 1.4 Turbo 	| Ramps 1.4       	| Anet A8 Mainboard 	|
|----------------	|---------------	|-----------------	|-------------------	|
| Stepper Driver 	| TMC2209       	| A4988 / TMC2209 	| A4988             	|
| LED Strip Port 	| WS2811 / RGB  	| -               	| -                 	|
| Firmware       	| Marlin-FW 2.0 	| Marlin-FW 1.0   	| Proprietary       	|


Hierbei existiert eine grosse Auswahl dieser mit den verschiedensten Ausstattungen. Bei der Auswahl dieser wurde vor allem auf die Möglichkeit geachtet sogenannte Silent-Schrittmotortreiber verwenden zu können um die Geräuschimmissionen durch die Motoren soweit wie möglich zu minimieren. Im ersten Prototyp wurde unter anderem aus diesem Grund die TMC5160-BOB Treiber ausgewählt.
Hierzu wurde der Schrittmotor-Treiber TMC2209 gewählt, welcher diese Features ebenfalls unterstützt und in der Variante als Silent-Step-Stick direkt in die meisten 3D Drucker Steuerungen eingesetzt werden können. Hierbei ist es wichtig, dass auf der gewählten Steuerung die Treiber-ICs nicht fest verlötet sind, sondern getauscht werden können.
Ein weitere Punkt ist die Kommunikation der Steuerung mit dem Host-System. Hierbei setzten alle untersuchten Steuerungen auf die (+usb) Schnittstelle und somit ist eine einfache Kommunikation gewährleistet. Das verwendete eingebettete System im autonomen Schachtisch bietet vier freie (+usb) Anschlüsse, somit ist eine einfache Integration gewährleistet.

![Producation Hardware: Blockdiagramm \label{ATC_Hardware_Architecture_PROD}](images/ATC_Hardware_Architecture_PROD.png)

Nach einer gründlichen Evaluation der zur verfügung stehenden Steuerungen, wurde die SKR 1.4 Turbo Steuerung ausgewählt, da diese trotz des etwas höheren Marktpreises genug Ressourcen auch für spätere Erweiterung bietet und eine Unterstützung für die neuste Version der Marlin-FW[@marlinfw] bereitstellt.
Somit wurde die Elektronik durch die verwendete Plug&Play stark vereinfacht \ref{ATC_Hardware_Architecture_PROD}.


### HAL: Implementierung GCODE-Sender


Durch die durchgeführten Änderungen an der Elektronik insbesondere durch die Verwendung einer Marlin-FW[@marlinfw] fähigen Motorsteuerung, ist eine Anpassung der (+hal) notwendig.
Diese unterstütz die Ansteuerung der Motoren und anderen Komponenten (z.B. Spindeln, Heizelemente) mittels G-Code und wird typischerweise in 3D Druckern und CNC-Fräsen eingesetzt.
G-Code ist eine
Marlin-FW[@marlinfw] biete dabei einen großen Befehlssatz an G-Code Kommandos an. Bei diesem Projekt werden jedoch nur einige G-Code Kommandos verwendet, welche sich insbesondere auf die Ansteuerung der Motoren beschränken.

: Grundlegende verwendete G-Code Kommandos

|                          	| G-Code Command 	| Parameters                        	|
|--------------------------	|----------------	|-----------------------------------	|
| Move X Y                 	| G0             	| X<dest_pos_x_mm> Y<dest_pos_y_mm> 	|
| Move Home Position       	| G28            	|                                   	|
| Set Units to Millimeters 	| G21            	|                                   	|
| Set Servo Position       	| M280           	| P<servo_index> S<servo_position>  	|
| Disable Motors           	| M84            	| X Y                               	|


Die erforderlichen Kommandos wurden auf eine Minimum beschränk um eine maximale Kompatibilität bei verschiedenen G-Code fähigen Steuerungen zu gewährleisten.
Die Software unterstützt jedoch weitere Kommandos wie z.B. `M150` mit welchem spezielle Ausgänge für LEDs gesteuert werden können. Dieses Feature bietet die verwendete Marlin-FW[@marlinfw], als auch die verwendete Steuerung an. Sollte die verwendete Steuerung solch ein optionales Kommando nicht unterstützen, so werden diese ignoriert und somit können auch preisgünstige Steuerungen verwendet werden.

Die Kommunikation zwischen Steuerung und eingebetteten System geschieht durch eine (+usb) Verbinden. Die Steuerung meldet sich als virtuelle Serielle Schnittstelle im System an und kann über diese mit der Software kommunizieren. Auch werden so keine speziellen Treiber benötigt, da auf nahezu jedem System ein Treiber (USB CDC) für die gängigsten (+usb) zu Seriell Wandler bereits installiert ist. Die Software erkennt anhand der zur Verfügung stehenden USB Geräte, sowie deren Vendor und Product-ID Informationen die Steuerung automatisch und verwendet diese nach dem Start automatisch. Hierzu wurde zuvor eine Liste mit verschiedenen getesteten Steuerungen sowie deren USB Vendor und Product-ID angelegt.

: Hinterlegte G-Code Steuerungen

| Product                         | Vendor-ID | Product-ID | Board-Type         |
|---------------------------------|-----------|------------|--------------------|
| Bigtreetech SKR 1.4 Turbo       | 1d50      | 6029       | Stepper-Controller |
| Bigtreetech SKR 1.4             | 1d50      | 6029       | Stepper-Controller |
| Bigtreetech SKR 1.3             | 1d50      | 6029       | Stepper-Controller |


Damit die Software mit der Steuerung kommunizieren kann, wurde eine G-Code Sender Klasse implementiert, welche die gleichen Funktionen wie die HAl-Basisklasse bereitstellen.
Nach Aufruf einer Funktion zum Ansteuern der Motoren, wird aus den übergeben Parametern das passende G-Code Kommando in Form einer Zeichenkette zusammengesetzt und auf die Serielle Schnittstelle geschrieben.



```c++
//GCodeSender.cpp
bool GCodeSender::setServo(const int _index,const int _pos) {
	return write_gcode("M280 P" + std::to_string(_index) + " S" + std::to_string(_pos));     //MOVE SERVO
}

bool GCodeSender::write_gcode(std::string _gcode_line, bool _ack_check) {
    //...
    //...
    //FLUSH INPUT BUFFER
	port->flushReceiver();
	//APPEND NEW LINE CHARAKTER IF NEEDED
	if (_gcode_line.rfind('\n') == std::string::npos)
	{
		_gcode_line += '\n';
	}
	//WRITE COMMAND TO SERIAL LINE
	port->writeString(_gcode_line.c_str());
    //WAIT FOR ACK
    return wait_for_ack();
}

bool GCodeSender::wait_for_ack() {
	int wait_counter = 0;
	//...
    //...
	while (true) {
        //READ SERIAL REPONSE
		const std::string resp = read_string_from_serial();
        //...
        //...
		//PROCESS
		if (resp.rfind("ok") != std::string::npos)
		{
			break;
		}else if(resp.rfind("echo:Unknown") != std::string::npos) {
			break;
		}else if(resp.rfind("Error:") != std::string::npos) {
			break;			
		}else if (resp.rfind("echo:busy: processing") != std::string::npos) {
			wait_counter = 0;
			LOG_F("wait_for_ack: busy_processing");
		}else {
            //READ ERROR COUNTER AND HANDLING
			wait_counter++;
			if (wait_counter > 3)
			{
				break;
			}
		}
	}
    //...
    //...
	return true;
}
```
Die Steuerung verarbeitet diese und bestätigt die Ausführung mit einer Acknowledgement-Antwort. Hierbei gibt es verschiedenen Typen. Der einfachste Fall ist ein `ok`, welches ein erfolgreiche Abarbeitung des Kommandos signalisiert. Ein weitere Fall ist die Busy-Antwort `echo:busy`. Diese Signalisiert, dass das Kommando noch in der Bearbeitung ist und wird im falle des autonomen Schachtisch bei langen und langsamen Bewegungen der Mechanik ausgegeben. Das System wartet diese Antworten ab, bis eine finale `ok`-Antwort zurückgegeben wird, erst dann wird das nächste Kommando abgearbeitet.



### HAL: I2C Seriell Umsetzer

Durch den Wegfall der zuvor eingesetzten Elektronik und der Austausch durch die SKR 1.4 Turbo Steuerung, ist jedoch ein Anschluss des PN532 (+nfc) Moduls nicht mehr direkt möglich.
Da dieses mittels (+i2c) Interface direkt mit dem eingebetteten Systems verbunden war. Diese Möglichkeit besteht weiterhin, jedoch wurde auch hier auf eine (+usb) Schnittstelle gewechselt. So ist es möglich das System auch an einem anderen Host-System zu betreiben, wie z.B. an einem handelsüblichen Computer.

Dazu wurde ein Schnittstellenwandler hinzugefügt welcher die (+i2c) Schnittstelle zu einer (+usb) Seriell wandelt. Hierzu wurde ein Atmega328p Mikrokontroller eingesetzt, da dieser weit verbreitet und preisgünstig zu beschaffen ist.
Die Firmware des Mikrokontrollers stellt ein einfaches kommandobasiertes Interface bereit. Die Kommunikation ist mit der Kommunikation und der Implementierung des G-Code Senders vergleichbar und teilen sich die gleichen Funktionen zur Kommunikation mit der Seriellen Schnittstelle.

```c++
//userboardcontroller.cpp Atmega328p Firmware
//simplyfied version
char scan_nfc_tag(){
    //...
    if (nfc.tagPresent())
    {
        //READ TAG CONTENT
        NfcTag tag = nfc.read();
        //READ NDEF PAYLOAD
        NdefMessage msg = tag.getNdefMessage();
        if(msg.getRecordCount() > 0){
            //READ FIRST RECORD
            NdefRecord record = msg.getRecord(0);
            const int payloadLength = record.getPayloadLength();
            byte payload[payloadLength];
            //...
            record.getPayload(payload);
            //...
            //...
            //RETURN FIGURE ID
            if(payloadLength == 6){
                return payload[3];
            }
        }
    return 0; //VALID TAGS FROM 1-127
}
```

Hier wird nur ein Befehl zum auslesen des (+nfc) Tags benötigt. Das Host-System sendet die Zeichenkette `_readnfc_` zum Mikrokontroller und dieser versucht über das PN532 Modul ein (+nfc) Tag zu lesen. Wenn dieses erkannt wird und einen passenden Payload enthält, antwortet dieser mit dem String `_readnfc_res_FIGURE-ID_ok_` oder wenn kein Tag gefunden wurde mit `_readnfc_red__empty_`.
Auch hier wird wie bei der G-Code Sender Implementierung auf Fehler bei der Kommunikation bzw einem Abbruch durch einen Timeout reagiert. Das System initialisiert die Serielle Schnittstelle neu und resettet das System durch setzten des DTR GPIO am USB-Seriell Wandler ICs (falls vorhanden).

```c++
//UserBoardController.cpp HOST-SYSTEM
//simplyfied version
ChessPiece::FIGURE UserBoardController::read_chess_piece_nfc(){

    ChessPiece::FIGURE fig;
    fig.type = ChessPiece::TYPE::TYPE_INVALID;
    //...
    //READ SERIAL RESULT
    const std::string readres = send_command_blocking(UBC_COMMAND_READNFC);
    //...
    //SPLIT STRING _
    const std::vector<std::string> re = split(readres,UBC_CMD_SEPERATOR);
    //READ SECTIONS
    //...
    //...
    const std::string figure = re.at(3);
    const std::string errorcode = re.at(4);
    //CHECK READ RESULT
    if(errorcode == "ok"){
        if(figure.empty()){
            break;
        }
        //...
        //...
        //DETERM FINAL READ FIGURE
        const char figure_charakter = figure.at(0);
        fig = ChessPiece::getFigureByCharakter(figure_charakter);
    }
    //...
    return fig;
}
```
Das System erkennt den Anschluss der Hardware beim Start auf die gleiche Art und Weise wie der G-Code Sender. Hierbei wurden einige verschiedene Mikrokontroller im System hinterlegt, auf welchen die Firmware geteset wurde.

: Hinterlegte Mikrokontroller

| Product                         | Vendor-ID | Product-ID | Board-Type         |
|---------------------------------|-----------|------------|--------------------|
| Arduino Due [Programming Port]  | 2341      | 003d       | User-Move-Detector |
| Arduino Due [Native SAMX3 Port] | 2341      | 003e       | User-Move-Detector |
| CH340                           | 1a86      | 7523       | User-Move-Detector |
| HL-340                          | 1a86      | 7523       | User-Move-Detector |
| STM32F411                       | 0483      | 5740       | User-Move-Detector |



## Fazit zum finalen Prototypen

* modularer hardware aufbau
* einfach/gut verfügbare materialien verwendet
* geänderte Mechnik resultiert in nahezu Spielfreier Mechanik (+- 1mm), welches für diesen Zweck mehr als ausreicht
* 6h dauertest bestanden

: Eigenschaften des finalen Prototypen

|                                         	| (+atc) – autonomous Chessboard    	|
|-----------------------------------------	|--------------------------------------	|
| Feldabmessungen (LxBxH)                 	| 57x57mm                              	|
| Abmessungen (LxBxH)                     	| 620x620x170mm                        	|
| Gewicht                                 	| 5.7kg                                	|
| Konnektivität                           	| (+wlan), (+usb)                      	|
| Automatisches Bewegen der Figuren       	| ja                                   	|
| Erkennung Schachfigurstellung           	| ja                                   	|
| Spiel Livestream                        	| ja                                   	|
| Cloud anbindung (online Spiele)         	| ja                                   	|
| Parkposition für ausgeschiedene Figuren 	| ja                                   	|
| Stand-Alone Funktionalität              	| ja                                   	|
| Besonderheiten                          	| User-Port für Erweiterungen           |

* alle anforderungen erfüllt
* zulasten der geschwindigkeit insbesondere bei der erkennung des User-Move






















# Entwicklung der Cloud Infrastruktur

Die erste Phase der Entwicklung des Systems bestand in der Entwicklung der Cloud-Infrastruktur und der darauf laufenden Services.
Hierbei stellt die "Cloud", einen Server dar, welcher aus dem Internet über eine feste IPv4 und IPv6-Adresse verfügt und frei konfiguriert werden kann.
Auf diesem System ist der Schach-Cloud Stack \ref{ATC_Cloud_Architecture} installiert, welcher zum einen aus der Schach-Software besteht, welche in einem Docker-Stack ausgefphrt wird und zum anderen....

![Gesamtübersicht der verwendeten Cloud-Infrastruktur \label{ATC_Cloud_Architecture}](images/ATC_Cloud_Architecture.png)



## API Design

Das System soll so ausgelegt werden, dass es im späteren Zeitpunkt mit verschiedenen Client-Devices mit diesem kommunizieren können.
Dazu zählen zum einen der autonome Schachtisch, aber z.B. auch einen Web-Client, welcher die Funktionalität eines Schachtisch im Browser abbilden kann.
Hierzu muss das System eine einheitliche (+rest)-Schnittstelle bereitstellen.

Eine RESTful API bezeichnet eine API welche HTTP-Requests verwendet um auf Daten zugreifen zu können.

* grafik
* 5 requirements


Die RESTful API stellt verschiedene Ressourcen bereit, welche durch eine URI \ref{ATC_URI_SCHEMES} eindeutig identifizierbar sind.
Auf diese können mittels verschiedenster HTTP Anfragemethoden (GET, POST, PUT, DELETE) zugegriffen werden. Jeder dieser Methoden stellt einen anderen Zugriff auf die Ressource dar und beeinflusst somit das Verhalten und die Rückantwort dieser.


![Cloud-Infrastruktur: Aufbau einer URI \label{ATC_URI_SCHEMES}](images/ATC_URI_SCHEMES.png)

Eine URI besteht dabei aus mehreren Teilen. Das Schema  gibt an wie die nachfolgenden Teile interpretiert werden sollen.
Dabei wird bei einer RESTful Schnittstelle typischerweise das (+http) Protokoll, sowie (+https) verwendet. Dabei steht (+https) für eine verschlüsselte Verbindung. Desweiteren gibt es viele andere Schema, wie z.B (+ftp) welches


Somit stellt die RESTful API eine Interoperabilität zwischen verschiedenen Anwendungen und Systemen bereit, welche durch ein Netzwerk miteinander verbunden sind.
Dieser Ansatz ist somit geeignet um die verschiedenen Client Systeme (Schachtisch, Webclient) eine Kommunikation mit dem Server zu erlauben.



## Service Architektur

* was ist ein Service
* microservice ansatz
* Kapselung der Schach spiel spzifischen funktionaliutäten
* verwendung von NoSQL Datenbanken somit müssen Tabellen nicht spziell auf Schach spezifische Felder ausgelegt sein
* statelss
Diese stellen alle wichtigen Funktionen zum Betrieb des autonomen Schachtischs zur verfügung.

![Cloud-Infrastruktur: Aufbau der Service Architecture \label{ATC_Service_Architecture}](images/ATC_Service_Architecture.png)





### Vorüberlegungen



* welche funktionalitäten müssen abgedeckt werden
* client aktivitendiagram






### Backend

![Cloud-Infrastruktur: Backend Login-Requst und Response \label{ATC_request_example}](images/ATC_request_example.png)

* matchmaking schachlogik
* zentraler zugriffspunkt auf das System und stellt diese abi bereit
* stellt spielerprofile aus datenbanken bereit bereit
* authentifizierung der clients und deren sessions
* weiterleitung der von spielerinteraktionen an move validator
* spielfelder werden als string übermittelt = hier fen representation; einfach zu parsen; standart



### MoveValidator

Der MoveValidator-Service bildet im System die eigentliche Schachlogik ab.
Die Aufgabe ist es, die vom Benutzer eingegebenen Züge auf Richtigkeit zu überprüfen und auf daraufhin neuen Spiel-Status zurückzugeben.
Dazu zählen unter anderem das neue Schachbrett und ob ein Spieler gewonnen oder verloren hat.

Bevor ein Spiel begonnen wird, generiert der MoveValidator das initiale Spielfeld und bestimmt den Spieler, welcher als erstes am Zug ist.


![MoveValidator: Beispiel Request zur Ausführung eines Zuges auf einem gegebenen Schachbrett \label{ATC_movevalidator_execute_move}](images/ATC_movevalidator_execute_move.png)


Der Backend-Service fragt einen neues Spiel an oder übergibt einen Schachzug inkl. des Spielbretts an den Service.\ref{ATC_movevalidator_execute_move} Der Response wird dann vom Backend in der Datenbank gespeichert und weiter an die Client-Devices verteilt.




: MoveValidator-Service (+api) Overview

| AtomicChess Move-Validator Service (+api) | (+api)-Route            	| Method 	| Form-Data             |
|----------------------------------------	|----------------------	|--------	|-----------------------	|
| Check Move                             	| /rest/check_move     	| POST   	| * fen * move * player 	|
| Execute Move                           	| /rest/execute_move   	| POST   	| fen * move             	|
| Validate Board                         	| /rest/validate_board 	| POST   	| fen                   	|
| Init Board                             	| /rest/init_board     	| GET    	|                       	|


Allgemein geschieht die Kommunikation über vier (+api) Calls, welche vom MoveValidator-Service angeboten werden.
Als erstes wird vom Backend der `/rest/init_board` Request verwendet, welcher ein neues Spielbrett in der (+fen) Notation zurückgibt, welches zum Start der Partie verwendet wird.
Allgemein arbeitet wurde das komplette System so umgesetzt, dass dieses mit einem Spielfeld in einer Zeichenketten/String arbeitet.
Dies hat den Vorteil, dass die Spielfeld-Notation leicht angepasst werden kann.
Mit diesem Design ist es möglich, auch andere Spielarten im System zu implementieren, nur hier die initialen Spielfelder generiert werden und Züge der Spieler validiert werden müssen.

Die (+fen) Notation ist universal und kann jede Brettstellung darstellen. Auch enthält diese nicht nur die Figurstellungen, sondern auch weitere Informationen, wie die aktuelle Nummer des Zuges oder welcher Spieler gerade an der Reihe ist. Diese werden dann in der (+xfen) Notation angegeben, bei der zusätzlich zu der Brettstellung auch noch die weiteren Informationen angehängt werden.

: Vergleich (+fen) - (+xfen)

| FEN-TYPE                                          	| FEN-String                                                    |
|---------------------------------------------------	|---------------------------------------------------------------|
| FEN                                                	| rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R             |
| X-FEN                                             	| rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2|
| SCHEMA                                            	| Board Player-Color Rochade En-Passant Halfturn Turn-Number	  |


Alle gängigen Schachprogramme und Bibliotheken unterstützen das Laden von Spielbrettern in der (+fen) bzw (+xfen) Schreibweise, ebenso die für den MoveValidator Service verwendete Python-Chess Bibliothek [@pythonchesslib]. Diese unterstützt zusätzlich die Generierung der für den Benutzer möglichen Schachzügen, welche auf dem aktuellen Brett möglich sind. Diese List wird vom System dazu verwendet um sicherzustellen, das der Benutzer nur gültige Züge tätigen kann. Diese Funktion lässt sich zusätzliche abschalten, falls das Spiel nicht nach den allgemeinen Schachregeln ablaufen soll. Bei der Generierung der möglichen Schachzügen, muss zwischen den Legal-Moves und den Pseudo-Legal Schachzügen unterschieden werden. Die Legal-Moves beinhalten nur die nach den Schachregeln möglichen Zügen, welche von Figuren des Spielers ausgeführt werden können.
Die Pseudo-Legal Schachzüge, sind alle Schachzügen welche von den Figuren auf dem aktuellen Schachbrett möglich, so sind z.B. auch alle anderen Figur-Züge enthalten, wenn der König sich aktuell im Schach befindet.

Wenn ein Spieler an der Reihe ist, sein getätigter Zug mittels der `/rest/check_move` (+api) überprüft, ob dieser gemäss der Legal-Moves durchführbar ist. Ist dies der Fall, wird der Zug auf das Spielbrett angewendet, welches durch die `/rest/execute_move` (+api) geschieht. Diese führ den Zug aus und ermittelt somit das neue Spielbrett und überprüft zusätzlich, ob das Spiel gewonnen oder verloren wurde.

Hat der Benutzer jedoch einen ungültigen Zug ausgeführt, wird dieser vom System storniert und der Client des Benutzers stellt den Zustand des Spielbretts vor dem getätigten Zug wieder her. Danach hat der Benutzer die Möglichkeit einen alternativen Zug auszuführen.




### Entwicklung Webclient

![Webclient: Spielansicht \label{ATC_webclient}](images/ATC_webclient.png)

Der Webclient wurde primär dazu entwickelt um das System während der Entwicklung zu testen.
Dieser simuliert einen autonomen Schachtisch und verwendet dabei die gleichen (+http) Requests.
Dieser wurde dabei komplett in (+js) umgesetzt im Zusammenspiel mit (+html) und (+css) und ist somit komplett im Browser lauffähig.

Ausgeliefert werden die statischen Dateien zur Einfachheit durch den Backend-Service, es wurde kein gesonderter Frontend-Service angelegt.
Durch die Implementierung des Webclienten in (+js), ist dieser sogar lokal über einen Browser ausführbar, ohne dass die benötigten Dateien über einen Webserver ausgeliefert werden müssen.

Zusätzlich zu dem verwendeten Vanilla-(+js) wurde jQuery als zusätzliche (+js) Bibliothek verwendet, welches eine Manipulation der (+html) Elemente stark vereinfacht. Diese bietet insbesondere einfach zu nutzende HTTP-Request Funktionen bzw. (+ajax) an, welche für die Kommunikation mit dem Backen-Service verwendet werden. Diese werden im Hintergrund eingesetzt, sodass der Webclient automatisch den neuen Spielzustand dem Benutzer anzeigt. Dies geschieht mittels `polling`, bei dem der Webbrowser in zyklischen Abständen die aktuellen Spiel-Informationen vom Backen-Service abfragt. Diese Methode wurde verwendet, um eine maximale Kompatibilität mit verschiedensten ggf älteren Web-Browsern sicherzustellen. Eine moderne alternative ist die Verwendung von Web-Sockets, bei welcher der Web-Browser eine direkte TCP-Verbindung zum Webserver (in diesem Fall der Backend-Service) aufnehemen kann und so eine direkte Kommunikation stattfinden kann ohne Verwendung der `polling`-Methode.


Der Hauptanwendungsfall des Webclienten während der Entwicklung, ist es weitere Spieler zu simulieren und so ein Spiel mit nur einem autonomen Schachtisch test zu können.
Durch den Webclient ist zusätzliche möglich, gezielt Spiele und Spielzüge zu simulieren. Hierzu gehören vor allem Sonderzüge wie die Rochade oder der En-Passant Zug.
Auch können durch den Webclient ungültige Züge simuliert werden, welche durch die Verwendete Schach-AI nicht getätigt werden.


Während der Implementierung wurde der Webclient weiter ausgebaut und es wurde weitere Features ergänzt.
Dazu zählt zum einen eine Übersicht über vergangene und aktuell laufende Spiele. In dieser können Spiele Zug um Zug nachvollzogen werden und weitere Information über den Spielstatus angezeigt werden.\ref{ATC_statistics}
Auch ist es möglich aktuell laufende Spiele in Echtzeit anzeigen zu lassen, somit wurde eine Livestream-Funktionaliät implementiert.

![Webclient: Statistiken \label{}](images/ATC_statistics.png)


### AutoPlayer

Der AutoPlayer-Service stellt den Computerspieler bereit.
Jede Service-Instanz stellt einen virtuellen Spieler bereit, welcher die gleiche Schnittstellen wie der Webclient oder der autonome Schachtisch verwendet.
Die einzige Änderung an den verwendeten (+rest)-Calls ist der Login-Requst. Hier wird das `playertype` Flag gesetzt welches den Spieler als Computerspieler gegenüber des Systems authentifiziert.
Somit wird dieser während des matchmaking Prozesses erst für ein Match ausgewählt, wenn kein anderer Spieler mehr zur Verfügung steht, welcher vom Typ Webclient oder autonomer Schachtisch ist.
Somit ist gewährleistet, dass immer zuerst die menschlichen Spieler ein Spiel beginnen.

Eine weitere Modifikation ist die Verwendung einer Schach-AI, da dieser Service als Computerspieler agieren soll.
Hierzu kam die Open-Source Chess Engine Stockfish[@stockfish] in der Version 11 zum Einsatz.
Die Stockfish-Engine bietet noch weitere Features, als nur die nächst besten Züge zu einem gegebenen Schachbrett zu ermitteln.
Sie kann auch genutzt werden um Züge zu klassifizieren und bietet Analysemöglichkeiten für Spielstellungen.

Die AutoPlayer-Instanz kommuniziert über das (+uci) Protokoll[@uciprotocol] mit der Engine.
Dieses Protokoll wird in der Regel von Schach-Engines verwendet, um mit einer (+gui) zu Kommunizieren.

Um das aktuelle Spielbrett in der Engine zu setzten wird dieses in der (+xfen) Notation mit dem Prefix `position fen ` als Klartext an die Engine übergeben und gibt daraufhin eine List möglicher Züge zurück. Der erste Index dieser Liste, ist dabei der am besten bewertete Zug der Engine.

Im Kontext des AutoPlayer-Service wird der Engine nur das aktuelle Spielbrett übermittelt und der nächst beste Zug auslesen.
Dies wird Ausgeführt, wenn der AutoPlayer am Zug ist. Nachdem die Engine einen passenden Zug gefunden hat, wird das Ergebnis über den `make_move` (+rest)-API Call übermittelt.



Wenn das Match beendet wird, beendet sich auch die Service-Instanz.
Diese wird jedoch wieder gestartet wenn die Anzahl der zur Verfügung stehenden Computerspieler unter einen definierten Wert fallen.
Somit ist dafür gesorgt, dass das System nicht mit ungenutzten AutoPlayer-Instanzen gebremst wird.
Diese Anzahl \ref{ai_player_count} ist in der Konfiguration des Backend-Service frei wählbar und kann je nach zu erwarteten Aufkommen angepasst werden.

Allgemein skaliert das System durch diese Art der Ressourcenverwaltung auch auf kleinen Systemen sehr flexibel.
Durch die Art der Implementierung, dass sich der AutoPlayer-Service wie ein normaler Spieler verhält, sind auch andere Arten des Computerspieler möglich.
So ist es zum Beispiel möglich, die Spielstärke je Spieler anzupassen oder einen Computerspieler zu erstellen, welcher nur zufällige Züge zieht.

Ein weiterer Anwendungsfall für den AutoPlayer-Service, ist das Testen des weiteren Systems insbesondere des Backend-Service.
Durch das Erstellen eines Spiels mit zwei AutoPlayer-Instanzen, können automatisierte test Schachpartien ausgeführt werden um die Funktionsfähigkeit des restlichen Systems zu testen.
Diese Feature wurde insbesondere bei der Entwicklung des Webclienten und der Steuerungssoftware für den autonomen Schachtisch verwendet.








# Embedded System Software

* Hauptsoftware zur Steuerung der Elektrik/Mechanik
* Kommunikation mit dem Cloud-Server



## Ablaufdiagramm

* dummer/thin Client
* Synchronisierung von gegeben Schachfeld mit dem lokalen Schachfeld
* getätigte züfe werden direkt an den schachserver geschickt und dieser generiert darauf hin das neue schachbrett welches von beiden Partner sync



![Embedded System Software: Ablaufdiagramm \label{ATC_gameclient_statemachiene}](images/ATC_gameclient_statemachiene.png)


## Figur Bewegungspfadberechnung

* Algorithmus zur Umsetzung eines Schachzugs
* Auftrennung in current und target Board
* vier Schritte (enfernen, bewegen, hinzufügen, bewegen)
## Schachfeld Scan Algorithmus zur Erkennung von Schachzügen

![Embedded System Software: Schachfeld Scan Algorithmus Ablauf \label{ATC_ChessMoveAlgorithm}](images/ATC_ChessMoveAlgorithm.png)

* Benutzer bestätigt dass er Schachzug gemacht hat
* Ermittlung des getätigten Schachzug
* Scan der Schachfeld-Veränderungen, durch Vergleich des vorherigen Schachfelds und der möglichen Züge





### Inter Prozess Communication

Bei der Entwicklung des System wurde darauf geachtet, dass sich das User-Interface austauschen lässt. Somit ist es auch mögliche eine webbasiertes User-Interface zu integrieren. Dazu wurde eine zusätzliche (+ipc) Layer hinzugefügt, welches eine Abstraktion der von der User-Interface Software verwendeten Funktionen auf der Controller-Software Ebene bereitstellt.


* IPC Bibliothek zur Kommunikation mit der controller-Software Instanz
* JSON basiert => einfaches Debugging
* Steuerung über andere Endgeräte möglich z.B Handy-App welche im selben Netzwerk befindet.
* durch preprozessor define wird der master definiert
* eventbasiert seperater mittels socket nicht per shared memory damit das ui nicht zwangsläufig auf der tisch laufen muss


```c++
//IPC guicommunicator.cpp
//Simplyfied example calls

//INIT IPC SERVER
guicommunicator gui;
gui.start_recieve_thread();
//CHECK OTHER PROCESS REACHABLE
while (!gui.check_guicommunicator_reachable()){
    gui_wait_counter++;
    if (gui_wait_counter > GUI_WAIT_COUNTER_MAX){
        break;
    }
}
//...
//CHECK OTHER PROCESS VERSION NUMBER
if(gui.check_guicommunicator_version()){
    LOG_F(WARNING, "guicommunicator version check failed");
}

//SWITCH MENU ON SCREEN TO PLEASE WAIT SCREEN
gui.createEvent(guicommunicator::GUI_ELEMENT::SWITCH_MENU, guicommunicator::GUI_VALUE_TYPE::PROCESSING_SCREEN);
//FLIP SCREEN ORIENTATION
gui.createEvent(guicommunicator::GUI_ELEMENT::QT_UI_SET_ORIENTATION_180, guicommunicator::GUI_VALUE_TYPE::ENABLED);

//GET EVENT FROM OTHER PROCESSES STORED IN EVENT QUEUE
guicommunicator::GUI_EVENT ev = gui.get_gui_update_event();
if (!ev.is_event_valid){
    gui.debug_event(ev, true);
    continue;
}
//CHECK FOR USER INPUT
if(ev.event == guicommunicator::GUI_ELEMENT::BEGIN_BTN_SCAN && ev.type == guicommunicator::GUI_VALUE_TYPE::CLICKED) {}
```






## Userinterface



Das User-Interface ist mit das zentrale Element mit welchem der Benutzer interagiert.
Hierbei soll dieses nur die nötigsten Funktionen bereitstellen, welche zur Bedienung des Schachtisches nötig sind.
Durch die kleinen Abmessungen des Displays mit 4.3", wurde alle Bedienelemente in ihrer Größe angepasst, sodass der Benutzer auch von einer weiter entfernten Position den Zustand direkt erkennen kann. Auch wurden die maximale Anzahl an Bedienelementen in einer Ansicht auf drei begrenzt. Die Spielansicht stellt zudem nur die eigene Spielerfarbe, sowie welcher Spieler gerade am Zug ist dar, somit soll der Spieler nicht vom Spiel abgelenkt werden. Nach dem Spielstart findet keine weitere Interaktion mit dem User-Interface mehr statt.

Trotz der Einfachheit der Bedienung und der meist nur also Informationsquelle über den Spielstand dienenden User-Interface, bietet diese viele Möglichkeiten der Konfiguration des Systems. Somit kann auf ein weiteres Eingabegerät, wie z.B. einem Mobiltelefon verzichtet werden, da alle relevanten Einstellungen im Optionen-Menu vorgenommen werden können.

Als Framework wurde hier das Qt[@qtframework] verwendet, da dieses bereits im Buildroot-Framework in der Version 5.12 hinterlegt ist. Somit musste kein anderes derartiges Framework aufwändig in das Buildroot-Framrwork integriert werden.

Das User-Interface wurde gegen Ende der Entwicklung der Controller-Software begonnen, somit waren alle benötigteten Ansichten und Funktionen definiert, trotzdem wurden im Vorfeld bereits mögliche Ansichten und Menüstrukturen mittels Wireframing festgehalten und konnten anhand dieser schnell umgesetzt werden.

![Embedded System Software: User-Interface Mockup \label{ATC_Gui}](images/ATC_Gui.png)

Das Qt[@qtframework] bietet dazu einen separaten Editor `Qt Design Studio` an, in denen die zuvor erstellen Wireframe-Grafiken importiert wurden und anschliessen mit den Bedienelementen ersetzt werden könnten. Dieser Prozess gestaltete sich als sehr Effizient und so konnte das komplette UI mit moderaten Zeitaufwand fertiggestellt werden.

```QML
// WINDOW.qml User-Interface ATC
import QtQuick 2.15
import QtQuick.Controls 2.15
//...
Rectangle {
    id: window
    objectName: "window"
    width: 800
    height: 480
    //BACKEND LOGIC INIT => CREATES INSTANCE OF THE MenuManager CLASS
    MenuManager{
        id:main_menu
        objectName: "mainmenu"
    }
    //...
    // MAIN MENU CONTAINER
    Rectangle {
        id: mm_container
        objectName: "mm_container"
        property var headline_bar_name:"Main Menu"
        //START AI MATCH BUTTON
        Button {
                id: mm_start_random_btn
                x: 40
                y: 183
                width: 207
                height: 55
                text: qsTr("START AI MATCH")
                //CONNECT BUTTON EVENTS TO BACKEND LOGIC
                Connections {
                    target: mm_start_random_btn
                    function onClicked(_mouse){
                        //CALL A FUNCTION IN BACKEND LOGIC INSTANCE
                        main_menu.mm_search_for_players_toggled(true)
                    }
                    //...
                }
                //...
```

Die anschliessende Implementierung der Backend-Logik des Unter-Interface bestand in der Verbindung, der in QML erstellten Bedienelemente durch den `Qt Design Studio`-Editor und der User-Interface Backend Logik. Diese beschränkt sich auf die Initialisierung des Fenster und dem anschliessenden laden und darstellen des QML Codes. Die Backend-Logik Funktionalitäten in einem QML Typ `MenuManager` angelegt, welcher vor dem laden des eigentlichen User-Interface QML Codes registriert werden muss.


```c++
// main.cpp User-Interface ATC
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "menumanager.h" //BACKEND LOGIC
int main(int argc, char *argv[])
{
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  //...
  //CREATE WINDOW
  QWindow window;
  window.setBaseSize(QSize(800,480));

  //REGISTER MainMenu COMPONENT
  qmlRegisterType<MenuManager>("MenuManager",1,0,"MenuManager");
  //LOAD User-Interface QML
  QQuickView view;
  //...
  view.engine()->addImportPath("qrc:/qml/imports");
  view.setSource(QUrl("qrc:/qml/WINDOW.qml"));
  view.engine()->rootContext()->setContextProperty("app", &app);
  //...

  //IMPORTANT STEP: AFTER INIT THE MainMenu COMPONENT HAS NO PARENT
  //SO WE NEED TO SET IT MANUALLY TO MAKE C++ -> QML FUNCATION CALLS WORKING
  QObject *object = view.rootObject();
  QObject *rect = object->findChild<QObject*>("mainmenu");
  if (rect){
         rect->setParent(object);
  }
  //FINALLY SHOW MENU ON SCREEN
  view.show();
```


Da das User-Interface ein separates Programm, welches auf dem System ausgeführt wird, muss dieses in der Lage sein mit der Controller-Software zu kommunizieren. Hierzu wurde die zuvor erstellte IPC Bibliothek in das Projekt importiert, jedoch wurde in der Makefile das `USES_QT` Define-Flag gesetzt. Wenn dieses gesetzt ist, wird die Bibliothek in den Client-Modus versetzt und stellt somit das Gegenstück zu der Instanz dar, welche in der Controller-Software läuft. Somit werden auch die Funktionen zum senden von `gui.createEvent()` umgekehrt, sodass ein Event in der Controller-Software ausgelöst wird. Dies kann z.B. durch eine Benutzereingabe oder wenn das User-Interface die von der Controller-Software geforderten Zustand angenommen hat.

* c++ example code

```c++
// menumanager.cpp User-Interface ATC
#include "menumanager.h"

MenuManager::MenuManager()
{
    //...
    //
    guiconnection.start_recieve_thread();
    //...
}

//METHOD CALLED FROM QML ELEMENT ss_calboard_btn
void MenuManager::ss_calboard_btn(){
    //SEND EVENT TO CONTROLLER SOFTWARE
    guiconnection.createEvent(guicommunicator::GUI_VALUE_TYPE::START_CALBOARD_PROC);
}

//PROCESSES EVENTS COMMING FROM THE INTER PROCESS COMMUNICATION AND SHOWS MENUS OR SET IMAGES/LABES
// MenuManager::updateProgress() CALLED BY SPERATE THREAD
void MenuManager::updateProgress()
{
    //GET LATEST EVENT
    const guicommunicator::GUI_EVENT ev =  guiconnection.get_gui_update_event();
    if(!ev.is_event_valid){return;}
    //PROCESS EVENTS
    //SWITCH MAIN MENU REQUEST
    if(ev.event == guicommunicator::GUI_ELEMENT::SWITCH_MENU){
        switch_menu(ev.type);
    }
    //...
}
//...
```






















# Fazit

Zusammenfassend lässt sich feststellen, dass das Ziel der Arbeit erreicht wurde.
Es wurde ein Prototyp eines autonomen Schachtischs entwickelt.

* mit am weitesten forgeschrittener open-source autonomes Schachtisch Projekt
* vom versierten Benutzer selbstädig aufbaubar
* leichte bedienung
* lässt spiel für erweiterungen
*

## Ausblick

* Einbindung in existeirende Schach-Clouds  z.B. https://lichess.org/
* user-port für Erweiterungen (z.B. DGT Schachur)
