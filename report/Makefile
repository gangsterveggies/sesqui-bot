all:
	pdflatex -shell-escape lab_report_2.tex
	bibtex lab_report_2
	pdflatex lab_report_2.tex > /dev/null
	pdflatex lab_report_2.tex > /dev/null
	@make tidy

tidy:
	rm -f *.log *.aux *.bbl *.blg *~

clean: tidy
	rm -f *.pdf
