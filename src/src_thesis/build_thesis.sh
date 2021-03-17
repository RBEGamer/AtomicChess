#!/bin/bash

echo "-- STARTING BUILDING THESIS DOCUMENT --"

pandoc "thesis_document.md" -o "thesis_document.tex" --from markdown --template "pandoc_template.tex" --listings --biblio=thesis_references.bib
pandoc "declaration.md" -o "declaration.tex" --from markdown
pandoc "abstract.md" -o "abstract.tex" --from markdown 
pandoc "attachments.md" -o "attachments.tex" --from markdown 


pdflatex thesis.tex thesis.pdf
pdflatex thesis.tex thesis.pdf
pdflatex thesis.tex thesis.pdf
exit 0
