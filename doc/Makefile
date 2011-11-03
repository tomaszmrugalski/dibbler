MAKEFLAGS   += -s
SUBDIR=$(subst $(TOPDIR),,$(CURDIR))
ifndef TOPDIR
TOPDIR=$(CURDIR)
endif

PASS3=NO

all: user devel

LATEX=pdflatex
DVIPS=dvips
PS2PDF=ps2pdf

LATEXOPTS=-file-line-error -halt-on-error

user: dibbler-user.pdf

DOXYGEN  = dibbler-devel-00-mainpage.dox 
DOXYGEN += dibbler-devel-01-intro.dox
DOXYGEN += dibbler-devel-02-compile.dox
DOXYGEN += dibbler-devel-03-port.dox
DOXYGEN += dibbler-devel-04-common.dox
DOXYGEN += dibbler-devel-05-sources.dox
DOXYGEN += dibbler-devel-06-arch.dox
DOXYGEN += dibbler-devel-07-debug.dox
DOXYGEN += dibbler-devel-08-contrib.dox
DOXYGEN += dibbler-devel-09-misc.dox
DOXYGEN += dibbler-devel-bibl.dox

dibbler-user.pdf: dibbler-user.tex dibbler-user-config-client.tex \
	  dibbler-user-config-server.tex dibbler-user-epilogue.tex \
	  dibbler-user-intro.tex dibbler-user-usage.tex \
          dibbler-user-bibliography.tex version.tex

	@echo "[LATEX  ] $(SUBDIR)/$@"
	$(LATEX) $(LATEXOPTS) dibbler-user.tex >dibbler-user-1st_pass.log
	if [ "$(PASS3)" = "YES" ]; then                                        \
	  echo "[LATEX  ] $(SUBDIR)/$@";                                       \
	  $(LATEX) $(LATEXOPTS) dibbler-user.tex >dibbler-user-2nd_pass.log;   \
	  echo "[LATEX  ] $(SUBDIR)/$@";                                       \
	  $(LATEX) $(LATEXOPTS) dibbler-user.tex >dibbler-user-3rd_pass.log;   \
	fi
	-grep "undefined" dibbler-user-3rd_pass.log;
	-grep "undefined" dibbler-user-1st_pass.log;

devel: $(DOXYGEN)
	doxygen doxygen.cfg
# > doxygen.log 2>doxygen-error.log
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