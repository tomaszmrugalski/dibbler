MAKEFLAGS   += -s
SUBDIR=$(subst $(TOPDIR),,$(CURDIR))
ifndef TOPDIR
TOPDIR=$(CURDIR)
endif 

PASS3=YES

all: user devel

LATEX=latex
DVIPS=dvips
PS2PDF=ps2pdf

user: dibbler-user.pdf

devel: dibbler-devel.pdf

dibbler-user.pdf: dibbler-user.tex dibbler-user-config.tex dibbler-user-epilogue.tex \
	  dibbler-user-intro.tex dibbler-user-usage.tex version.tex
	@echo "[LATEX  ] $(SUBDIR)/$@"
	$(LATEX) dibbler-user.tex &>dibbler-user-1st_pass.log
	if [ $(PASS3) == "YES" ]; then                          \
	  echo "[LATEX  ] $(SUBDIR)/$@";                        \
	  $(LATEX) dibbler-user.tex &>dibbler-user-2nd_pass.log;\
	  echo "[LATEX  ] $(SUBDIR)/$@";                        \
	  $(LATEX) dibbler-user.tex &>dibbler-user-3rd_pass.log;\
	fi
	@echo "[DVIPS  ] $(SUBDIR)/$@"
	$(DVIPS) -z dibbler-user.dvi -o &>dibbler-user-dvips.log
	@echo "[PS2PDF ] $(SUBDIR)/$@"
	$(PS2PDF) dibbler-user.ps &>dibbler-user-ps2pdf.log

dibbler-devel.pdf: dibbler-devel.tex        dibbler-devel-compile.tex \
                   dibbler-devel-common.tex dibbler-devel-compile.tex \
		   dibbler-devel-arch.tex   dibbler-devel-intro.tex \
                   dibbler-devel-misc.tex   version.tex
	@echo "[LATEX  ] $(SUBDIR)/$@"
	$(LATEX) dibbler-devel.tex &>dibbler-devel-1st_pass.log
	if [ $(PASS3) == "YES" ]; then                            \
	  echo "[LATEX  ] $(SUBDIR)/$@";                          \
	  $(LATEX) dibbler-devel.tex &>dibbler-devel-2nd_pass.log;\
	  echo "[LATEX  ] $(SUBDIR)/$@";                          \
	  $(LATEX) dibbler-devel.tex &>dibbler-devel-3rd_pass.log;\
	fi
	@echo "[DVIPS  ] $(SUBDIR)/$@"
	$(DVIPS) -z dibbler-devel.dvi -o &>dibbler-devel-dvips.log
	@echo "[PS2PDF ] $(SUBDIR)/$@"
	$(PS2PDF) dibbler-devel.ps

version.tex:
	@echo "[GREP   ] $(SUBDIR)/$@"
	@../test/xtract_version > $@

clean:
	@echo "[CLEAN  ] $(SUBDIR)"
	@rm -f *.aux *.idx *.log *.toc *.out *~ *.pdf *.ps *.dvi *.tmp version.tex

.PHONY: version.tex