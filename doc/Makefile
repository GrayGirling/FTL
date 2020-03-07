CMD_SOFFICE:=soffice
CMD_PANDOC:=pandoc


all: pdf/FTL-user-guide.pdf

pdf/FTL-user-guide.pdf: FTL-user-guide.odt
	$(CMD_SOFFICE) --convert-to pdf --outdir pdf --headless $< 

markdown/FTL-spec.md: FTL-spec.html
	$(CMD_PANDOC) -w markdown_github -o $@ $<


