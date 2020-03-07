# Makefile to create copies of documentation held in this directory in
# different formats held in subdirectories
# (The documents in this directory are the masters, those in the subdirectories
# are derivative)

CMD_SOFFICE:=soffice
# (LibreOffice is assumed)
CMD_PANDOC:=pandoc
# note - pandoc 1.x is too primitive, pandoc 2.9.2 is OK -
# default linux version was 1.x so may need to find a version on-line

all: pdf-docs md-docs

pdf-docs: pdf/FTL-user-guide.pdf

md-docs: markdown/FTL-spec.md \
	 markdown/FTL-commandline-tutorial.md \
	 markdown/FTL-build-setup.md \
	 markdown/FTL-client-tool-creation.md \
	 markdown/FTL-user-guide.md

pdf/%.pdf: %.odt
	$(CMD_SOFFICE) --convert-to pdf --outdir pdf --headless $< 

markdown/%.md: %.html
	$(CMD_PANDOC) -w gfm -o $@ $<

markdown/%.md: %.odt
	$(CMD_PANDOC) -w gfm -o $@ $<

