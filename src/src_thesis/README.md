# AtomicChessThesis

Das eigentliche Dokument ist die `thesis_document.md`. Die finale PDF `thesis.pdf` ist soweit aktuell, wenn ich diese erstelle, dies ist hier im Repo noch nicht automatisiert.
Ein bauen per Dockerimage ist möglich mittels ` build_thesis_docker.sh` Script und erzeugt die `thesis.pdf` neu.

# CHANGELOG

# 05.06.2021

* 7.3 Service: Backend - Matchmaking+Beispiel (bearbeitet)
* 7.3 Service: Backend - Docker-Compose Code (bearbeitet)

# 04.06.2021

* Abstract - thesis_abstract.md (neu)
* 9.0 Anhang entfernt (neu)


# 02.06.2021

* einarbeitung Kommentare (neu)
* 4.0 Machbarkeitsanalyse und Verifikation ausgewählter Technologien (neu)

# 31.05.2021

* anpassungen Bilder (überarbeitet)
* abstract (neu)
* Zusammenfassung Kapitel 4 +5 => Machbarkeitsanalyse und Verifikation ausgewählter Technologien
* 4 Machbarkeitsanalyse und Verifikation ausgewählter Technologien (neu)
* 4.1 ErprobungBuildroot-Framework (neu)
* 4.2 Verifikation NFC Technologie (neu)
* 4.3 Schrittmotor/Schrittmotorsteuerung (neu)
* 8.0 Embedded System Software (neu)

# 30.05.2021

* 10 - Fazit (neu)
* 10.1 - persoenliches Fazit (neu)
* 10.2 - Ausblick (neu)



# 28.05.2021

* 8.2 - Service Architektur (neu)
* added Haskel to Dockerfile
* added csv2table pandoc filter
* added bib ref for docker, redis, mongo, docker-compose

# 27.05.2021 

* anpassungen Bilder (überarbeitet)

# 26.05.2021 II

* 8.4 - Backend (neu)

## 26.05.2021 I

* 1.1 - Motivation (überarbeitet)
* 6.1 - Mechanik (Bild eingefügt)
* 7.2 - Optimierungen der Spielfiguren (Bild eingefügt)
* 7.5 - Fazit bezüglich des finalen Prototypens (neu)

## 25.05.2021 II

* einarbeitung Kommentare 11:49
* einfügen Referenzen für Tabellen

## 25.05.2021 I

* 6.5 - Fazit des ersten Prototypens
* 7.1.1 - Gehäuse und Design
* 7.1.2 - 3D-Komponenten
* 7.1.3 Positions-Mechanik
* 9.1 - Ablaufdiagramm
* 9.2 - Figur Bewegungspfadberechnung

## 24.05.2021

* 6.3 - Schaltungsentwurf
* 6.4 - Implementierung HAL
* 7.1 - Modifikation der Mechanik

## 23.05.2021

* pandoc chapter numbering fix
* delaration, abstract toc fix
* pcb, schematic img added

## 21.05.2021 - INITIALE VERSION

* 3 - Einleitung, Motivation, Methodik
* 4.1 - 4.3, Analyse
* 5.4 - 3D Druck
* 6.1 - 6.2, Mechanik
* 7.2 - 7.3 - Optimierungen HWRev 2, HAL
* 8.5 - 8.7 - AutoPlayer WebClient
* 9.3 - 9.4 IPC User-Interface


## BUILD THESIS thesis_document

`$ bash ./build_thesis_docker.sh`
