MAKEFLAGS   += -s
SUBDIR=$(subst $(TOPDIR),,$(CURDIR))
ifndef TOPDIR
TOPDIR=$(CURDIR)
endif 

PASS3=YES

all: user devel

LATEX=pdflatex
DVIPS=dvips
PS2PDF=ps2pdf

LATEXOPTS=-file-line-error -halt-on-error

user: dibbler-user.pdf

dibbler-user.pdf: dibbler-user.tex dibbler-user-config.tex dibbler-user-epilogue.tex \
	  dibbler-user-intro.tex dibbler-user-usage.tex dibbler-user-bibliography.tex version.tex
	@echo "[LATEX  ] $(SUBDIR)/$@"
	$(LATEX) $(LATEXOPTS) dibbler-user.tex >dibbler-user-1st_pass.log
	if [ $(PASS3)=="YES" ]; then                                       \
	  echo "[LATEX  ] $(SUBDIR)/$@";                                     \
	  $(LATEX) $(LATEXOPTS) dibbler-user.tex >dibbler-user-2nd_pass.log; \
	  echo "[LATEX  ] $(SUBDIR)/$@";                                     \
	  $(LATEX) $(LATEXOPTS) dibbler-user.tex >dibbler-user-3rd_pass.log; \
	fi
#	@echo "[DVIPS  ] $(SUBDIR)/$@"
#	$(DVIPS) -z dibbler-user.dvi -o &>dibbler-user-dvips.log
#	@echo "[PS2PDF ] $(SUBDIR)/$@"
#	$(PS2PDF) dibbler-user.ps &>dibbler-user-ps2pdf.log

devel: dibbler-devel-arch.dox dibbler-devel-bibl.dox dibbler-devel-common.dox \
	dibbler-devel-compile.dox dibbler-devel-debug.dox dibbler-devel-intro.dox \
        dibbler-devel-mainpage.dox dibbler-devel-misc.dox dibbler-devel-port.dox
	doxygen doxygen.cfg
	cp logo* html/

version.tex:
	@echo "[GREP   ] $(SUBDIR)/$@"
	@../scripts/xtract_version > $@
	@echo "[PERL   ] $(SUBDIR)/$@"
	perl -i -pe 's/_/\\_/g' version.tex
	perl -i -pe 's/^/\\newcommand{\\version}{/' version.tex
	perl -i -pe 's/$$/ }/' version.tex

clean:
	@echo "[CLEAN  ] $(SUBDIR)"
	@rm -f *.aux *.idx *.log *.toc *.out *~ *.ps *.dvi *.tmp version.tex

clobber: clean
	@rm -f *.pdf

.PHONY: version.tex devel