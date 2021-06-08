#!/bin/bash
cd .
# REMOVE TEMP FILES
rm -f *.acn
rm -f *.acr
rm -f *.alg
rm -f *.ist
rm -f *.glg
rm -f *.glo
rm -f *.gls
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

# REMOVE GENERATED TEX
rm -f ./thesis_declaration.tex
rm -f ./thesis_document.tex
rm -f ./thesis_abstract.tex
rm -f ./thesis_attachments.tex
# REMOVE GENERATED PDF
rm -f ./thesis.pdf


echo "-- STARTING BUILDING THESIS DOCUMENT --"
pandoc --version




pandoc ./thesis_document.md -o ./thesis_document.tex --from markdown --biblatex --template ./pandoc_template.tex --listings --top-level-division=chapter --lua-filter ./pandoc_filters/pandoc-gls.lua
# NOW THE HACKY PART WE WANT TO USE THE STANDART cite command instead the from pandoc used cite to we use sed to hard replace the stuff
sed -i 's/\\autocite{/\\cite{/g' ./thesis_document.tex
# python3 ./fix-table-color-bleed.py ./thesis_document.tex > ./thesis_document.tex

pandoc ./thesis_declaration.md -o ./thesis_declaration.tex --from markdown --top-level-division=chapter --listings
pandoc ./thesis_abstract.md -o ./thesis_abstract.tex --from markdown --top-level-division=chapter --listings
pandoc ./thesis_attachments.md -o ./thesis_attachments.tex --from markdown --top-level-division=chapter --listings

echo "------------- PANDOC GENERATION FINISHED -----------"

# BUILD THESIS FIRST TIME GENERATE .AUX and .TOC FILE
pdflatex ./thesis.tex ./thesis.pdf
# GENERATE BIBTEX INDEX
makeglossaries thesis # ACRONYM
bibtex thesis # REFERENCES
pdflatex ./thesis.tex ./thesis.pdf
# BUILD FINAL DOCUMENT
pdflatex ./thesis.tex ./thesis.pdf

echo "------------- PDF EXPORT FINISHED -----------"



# EXPORT AS HTML

#pandoc -s thesis.tex -o thesis.md
#pandoc thesis.tex -f latex -t html -s -o thesis.html --bibliography thesis_references.bib

# REMOVE TEMP FILES
rm -f *.acn
rm -f *.acr
rm -f *.alg
rm -f *.ist
rm -f *.glg
rm -f *.glo
rm -f *.gls
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
