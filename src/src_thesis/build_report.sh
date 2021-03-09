#!/bin/bash

echo "-- STARTING BUILDING REPORT DOCUMENT --"

pandoc "report_document.md" -o "report_document.tex" --from markdown --template "template.tex" --listings

pdflatex report_document.tex report_document.pdf

exit 0