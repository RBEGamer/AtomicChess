# AtomicChessThesis

Das eigentliche Dokument ist die `thesis_document.md`. Die finale PDF `thesis.pdf` ist soweit aktuell, wenn ich diese erstelle, dies ist hier im Repo noch nicht automatisiert.
Ein bauen per Dockerimage ist möglich mittels ` build_thesis_docker.sh` Script und erzeugt die `thesis.pdf` neu.

# CHANGELOG

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
