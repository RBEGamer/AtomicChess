#!/bin/bash

echo "-- STARTING BUILDING THESIS DOCUMENT --"

pandoc "thesis_document.md" -o "thesis_document.tex" --from markdown --template "pandoc_template.tex" --listings
pandoc "declaration.md" -o "declaration.tex" --from markdown
pandoc "abstract.md" -o "abstract.tex" --from markdown 
pandoc "attachments.md" -o "attachments.tex" --from markdown 

# BUILD THESIS FIRST TIME GENERATE .AUX and .TOC FILE
pdflatex thesis.tex thesis.pdf
# GENERATE BIBTEX INDEX
makeglossaries thesis #ACRONYM
bibtex thesis # REFERENCES

# BUILD FINAL DOCUMENT
pdflatex thesis.tex thesis.pdf
pdflatex thesis.tex thesis.pdf

echo "------------- PDF EXPORT FINISHED -----------"
# EXPORT AS HTML
#pandoc thesis.tex -f latex -t html -s -o thesis.html --bibliography thesis_references.bib

rm -f *.aux
rm -f *.lof
rm -f *.lot
rm -f *.aux
rm -f *.out
rm -f *.toc
rm -f *.log
rm -f *.synctex.gz
rm -f *.bbl
rm -f *.blg
rm -f *.fls
rm -f *.fdb_latexmk
rm -f *.run.xml
exit 0
