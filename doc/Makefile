all: user devel

LATEX=latex
DVIPS=dvips
PS2PDF=ps2pdf

user: dibbler-user.pdf

devel: dibbler-devel.pdf

dibbler-user.pdf: dibbler-user.tex dibbler-user-config.tex dibbler-user-epilogue.tex \
	  dibbler-user-intro.tex dibbler-user-usage.tex
	$(LATEX) dibbler-user.tex
	$(LATEX) dibbler-user.tex
	$(LATEX) dibbler-user.tex
	$(DVIPS) -z dibbler-user.dvi -o
	$(PS2PDF) dibbler-user.ps

dibbler-devel.pdf: dibbler-devel.tex dibbler-devel-compile.tex \
                   dibbler-devel-common.tex dibbler-devel-compile.tex \
		   dibbler-devel-arch-clnt.tex dibbler-devel-arch-srv.tex \
                   dibbler-devel-intro.tex dibbler-devel-misc.tex
	$(LATEX) dibbler-devel.tex
	$(DVIPS) -z dibbler-devel.dvi -o
	$(PS2PDF) dibbler-devel.ps

version.tex:
	@echo "[GREP  ] $(SUBDIR)/$@"
	@echo "\newcommand{\dibblerVersion}{" > $@
	@../test/xtract_version >> $@
	@echo "}" >> $@

clean:
	@echo "[CLEAN  ] $(SUBDIR)"
	@rm -f *.aux *.idx *.log *.toc *.out *~ *.pdf *.ps *.dvi

.PHONY: version.tex