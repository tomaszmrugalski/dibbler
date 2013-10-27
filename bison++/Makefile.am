bin_PROGRAMS = bison++
AUTOMAKE_OPTIONS = foreign 

bison___SOURCES = closure.cc alloca.c\
			derives.cc \
			getargs.cc \
			getopt1.cc \
			lalr.cc  \
			lr0.cc \
			nullable.cc \
			print.cc \
			reduce.cc \
			version.cc \
			warshall.cc \
			allocate.cc \
			conflict.cc \
			files.cc \
			getopt.cc \
			gram.cc \
			lex.cc \
			main.cc \
			output.cc \
			reader.cc \
			symtab.cc\
			old.c \
	files.h FlexLexer.h getopt.h gram.h lex.h machine.h new.h \
	state.h symtab.h system.h types.h

dist_noinst_DATA = bison.cc bison.hairy bison.h
dist_noinst_DATA += bison++.1 bison++.1.dman bison.cld
dist_noinst_DATA += bison.info bison.info-1 bison.info-2
dist_noinst_DATA += bison.info-3 bison.info-4 bison.info-5
dist_noinst_DATA += bison_pp.mak bison.ps.gz
dist_noinst_DATA += bison.rnh bison.texinfo
dist_noinst_DATA += bison++.yacc bison++.yacc.1
dist_noinst_DATA += README++ REFERENCES version.texi

#info_TEXINFOS = bison.texinfo
#man_MANS = bison++.1 bison.1 bison++.yacc.1
pkgdata_DATA = bison.cc bison.hairy bison.h Example
CXX=g++
PFILE = bison.cc
PFILE1 = bison.hairy
HFILE = bison.h
AM_CPPFLAGS = -DXPFILE=\"$(datadir)/bison++/$(PFILE)\" \
	 -DXHFILE=\"$(datadir)/bison++/$(HFILE)\" \
	 -DXPFILE1=\"$(datadir)/bison++/$(PFILE1)\"

install-exec-hook:
	cp bison $(bindir)
	cp bison++.yacc $(bindir)

uninstall-hook:
	rm $(bindir)/bison++.yacc
	rm $(bindir)/bison





