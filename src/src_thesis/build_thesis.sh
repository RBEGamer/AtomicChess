#!/bin/bash

echo "-- STARTING BUILDING THESIS DOCUMENT --"

#pandoc "thesis_document.md" -o "thesis_document.tex" --from markdown --template "template.tex" --listings
pandoc "thesis_document.md" -o "thesis_document.tex" --from markdown --listings

pdflatex thesis_document.tex thesis_document.pdf

exit 0