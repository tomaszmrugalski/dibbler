all: user devel

LATEX=pdflatex

user: dibbler-user.pdf

devel: dibbler-devel.pdf

dibbler-user.pdf: dibbler-user.tex dibbler-user-config.tex dibbler-user-epilogue.tex \
	  dibbler-user-intro.tex dibbler-user-usage.tex
	$(LATEX) dibbler-user.tex

dibbler-devel.pdf: dibbler-devel.tex
	$(LATEX) dibbler-devel.tex