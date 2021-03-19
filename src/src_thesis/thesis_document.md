# Einleitung

## Motivation

Der Boom in der Spieleindustrie ist ungebrochen. Allein das Online-Rollenspiel World of Warcraft erreichte Ende Oktober 2008 eine Spielerzahl von 11 Millionen [15]. Der Spieler bewegt seinen Charakter meist durch eine dreidimensionale Fantasiewelt – durch verschiedene St¨adte und D¨orfer, durch W¨alder und W¨usten bis hin zu Dschungeln und speziellen Dungeons. Er sammelt dort Erfahrungspunkte und Belohnungen in Form von virtuellem Geld, Ausr¨ustungsgegenst¨anden oder anderen Items. Hierbei ist das Medium Computer, als auch der Stil der Interaktion durch die Steuerung der Figur vom Programm vorgegeben. Alles in Allem ist die Trennung der Welten innerhalb des Spiels und der realen Welt zu jeder Zeit gegeben.

Pervasive Spiele dahingegen zielen auf eine neue Art des Spielens ab. Sie integrieren Informations- und Kommunikationstechnik, um die Grenzen herk¨ommlicher Spiele aufzubrechen und erweiterte Spielwelten zu schaﬀen. Mit Hilfe von mobilen Endger¨aten und deren speziﬁschen Eigenschaften soll somit eine neue Art von Spielerlebnis geschaﬀen werden. Die Nutzung von ortsabh¨angigen Diensten und Umgebungsinformationen der Benutzer wird zu einem entscheidenden Merkmal des Spiels. Die Schl¨usseltechnologien daf¨ur ﬁnden sich in den Bereichen der drahtlosen Kommunikation und der Positionsbestimmung von Benutzern bzw. mobilen Ger¨aten. Im Gegensatz zu klassischen Computerspielen, die in der virtuellen Welt stattﬁnden, sind bei pervasiven Spielen beispielsweise physikalische Bewegungen und soziale Interaktionen mit anderen Benutzern erforderlich. Der Spieler nutzt die physische Welt als Spielfeld und kann dennoch die Vorteile und M¨oglichkeiten der technischen Ger¨ate und der virtuellen Welt nutzen.

Auf diese Weise k¨onnen g¨anzlich neue Spielkonzepte realisiert werden. Aber auch eine Er- 1 Einleitung

2

weiterung tradtionioneller Spiele um Komponenten des Pervasive Computing ist denkbar.

Solche Spiele er¨oﬀnen ein breites Feld von Anwendungen. Es k¨onnen neue Lernmethoden im Schulunterricht oder die unterhaltsame Vermittlung kultureller Aspekte angestrebt werden. Die Konzeption dessen gestaltet sich jedoch problematisch und wirft Fragen auf, wie die Schl¨usseltechnologien in das Spiel zu integrieren sind.

## Zielsetzung

Im Rahmen dieser Arbeit soll eine verteilte Anwendung entworfen und prototypisch implementiert werden, die es erm¨oglicht anhand unterschiedlicher Sensordaten eines mobilen Endger¨ates die Situation und Position des Benutzers zu bestimmen. In Abh¨angigkeit dieser Daten sollen von einer zentralen Instanz die Daten zur Anreicherung des Spiels abgerufen werden. Auf diese Weise sollen dem Spieler Elemente der Story, neue Aufgaben oder Informationen ¨uber andere Spieler n¨aher gebracht werden.

Ziel dieser Arbeit wird es sein, die Vorgehensweise bei der Positions- und Situationsbestimmung im Detail auszuarbeiten und die unterschiedlichen Technologien und Algorithmen zu untersuchen und den Anforderungen entsprechend auszuw¨ahlen. Diese werden schließlich in einem Software-Entwurf zu einem Gesamtsystem kombiniert, das alle Anforderungen an ein unterhaltsames Spiel f¨ur Pervasive Gaming erf¨ullt.

## Aufbau der Arbeit

In dieser Arbeit werden theoretische Grundlagen und gegebene Randbedingungen beleuchtet. Zun¨achst werden in Abschnitt 2.1 die Konzepte der Positionsbestimmung und einsetzbare Technologien beschrieben. Anschließend werden in Abschnitt 2.2 die Grundlagen des Ubiquitous und Pervasive Computing und die damit verbundenen Konﬂikte in den Bereichen Sicherheit und Datenschutz beleuchtet. In Abschnitt 2.3 wird auf die Notwendigkeit der Kontextsensitivit¨at eingegangen. Und schließlich wird in Abschnitt 2.4 auf die aktuellen Trends und m¨ogliche Endger¨ate f¨ur die Umsetzung des Systems eingegangen.

Gegenstand der Diskussion in Kapitel 3 ist die Beleuchtung bereits vorhandener pervasiver Spiele und die Identiﬁzierung vorhandener und fehlender Funktionalit¨aten. Die Analyse wird außerdem m¨ogliche Anwendungsf¨alle aufzeigen, um die Komponenten auf 1 Einleitung

3

konzeptioneller Ebene zu erarbeiten. Aufbauend auf der Analyse wird das umzusetzende Spielkonzept erarbeitet und f¨ur den Prototypen deﬁniert.

Der Systementwurf in Kapitel 4 wird die Ergebnisse der vorangegangenen Untersuchungen in einem Software-System zusammenf¨uhren. Zudem wird die Datenhaltung modelliert, die Architektur in Komponenten zerlegt und deren Funktionsumfang festgelegt.

Die Implementierung der Anwendung behandelt die Schritte, Probleme und L¨osungen der Umsetzung. Außerdem wird auf die eingesetzten Implementierungs-Technologien eingegangen.

Eine Demonstration zeigt die zuvor in der Anforderungsdeﬁnition erstellte Funktionalit¨at der Anwendung. Außerdem testet eine Evaluation unter verschiedenen Gesichtspunkten die Flexibilit¨at und Korrektheit der Anwendung.

Abschließend wird eine Zusammenfassung ¨uber den Verlauf der Arbeit und der erzielten Ergebnisse sowie ein Ausblick auf m¨ogliche Erweiterungen des Systems aufgezeigt.






# Analyse bestehender Systeme

## Existierende Systeme im Vergleich

|                                         	| Square Off - Kingdom              	| Square Off - Grand Kingdom        	| DGT Smart Board      	| DGT Bluetooth Wenge  	|
|-----------------------------------------	|-----------------------------------	|-----------------------------------	|----------------------	|----------------------	|
| Erkennung Schachfigurstellung           	| ja (Manuell per Ausgangsposition) 	| ja (Manuell per Ausgangsposition) 	| ja (Resonanzspulen)  	| ja (RFID)            	|
| Tischabmessungen (LxBxH)                	| 486mm x 486mm x 75mm              	| 671mm x 486mm x 75mm              	| 540mm x 540mm x 20mm 	| 540mm x 540mm x 20mm 	|
| Konektivität                            	| BLE                               	| BLE                               	| USB / Seriell        	| Bluetooth 2.0        	|
| Automatisches Bewegen der Figuren       	| ja                                	| ja                                	| nein                 	| nein                 	|
| Spiel Livestream                        	| ja                                	| ja                                	| ja                   	| ja                   	|
| Cloud anbindung (online Spiele)         	| ja (über Mobiltelefon + App)      	| ja (über Mobiltelefon + App)      	| ja (über PC + App)   	| ja (über PC + App)   	|
| Parkposition für ausgeschiedene Figuren 	| nein                              	| ja                                	| nein                 	| nein                 	|
|                                         	|                                   	|                                   	|                      	|                      	|
|           

## Zielgruppe

## User Experience

### Software-Aufbau

### Hardware-Aufbau



# Grundlagen

## Anforderungsanalyse




## Machbarkeitsanalyse

## Technologien im Makerspace

##













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



## LaTeX Table with Caption

At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr.


| ALGORITHM_V1_TRAVEL_TIME [s] 	| ALGORITHM_V2_TRAVEL_TIME [s] 	| TRAVEL_DISTANCE [FIELDS_DIAGONAL] 	|
|------------------------------	|------------------------------	|-----------------------------------	|
| 7.20                         	| 2.56                         	| 1                                 	|
| 11.56                        	| 6,20                         	| 3                                 	|
| 12,27                        	| 7,06                         	| 5                                 	|
| 14,39                        	| 6,56                         	| 8                                 	|


Table: Verschiedene Bewegungsalgorithmen im Vergleich


## Image with Caption

![Kalibrierungeschema der Mechanik zeigt welche Abstände in der Konfiguration eigetragen werden müssen](images/ATC_Calibration_Guide.png)
