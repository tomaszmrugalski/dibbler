all:

all-am:

cppcheck:
	mkdir -p html
	cd .. && cppcheck --enable=all --inline-suppr \
	  --suppressions-list=doc/cppcheck-skip.txt \
          --template '{file}:{line}: {message} ({severity},{id})' \
	  -f -v -j 4 -i bison++/ -i tests/ -i dibbler-*/ -i poslib/examples \
	  . 1> doc/html/cppcheck.log 2> doc/html/cppcheck-error.log

man8_MANS = man/dibbler-server.8 man/dibbler-client.8 man/dibbler-relay.8

nobase_dist_doc_DATA = dibbler-user.pdf
nobase_dist_doc_DATA += examples/client-addrparams.conf examples/client-auth.conf
nobase_dist_doc_DATA += examples/client-autodetect.conf examples/client.conf examples/client-fqdn.conf
nobase_dist_doc_DATA += examples/client-prefix-delegation.conf examples/client-stateless.conf
nobase_dist_doc_DATA += examples/client-ta.conf examples/client-win32.conf
nobase_dist_doc_DATA += examples/client-custom.conf
nobase_dist_doc_DATA += examples/relay-1interface.conf examples/relay.conf
nobase_dist_doc_DATA += examples/relay-echo-remoteid.conf
nobase_dist_doc_DATA += examples/server-3classes.conf examples/server-addrparams.conf
nobase_dist_doc_DATA += examples/server-auth.conf examples/server-bulk-lq.conf examples/server.conf
nobase_dist_doc_DATA += examples/server-extraopts.conf examples/server-fqdn.conf
nobase_dist_doc_DATA += examples/server-guess-mode.conf examples/server-leasequery.conf
nobase_dist_doc_DATA += examples/server-per-client.conf examples/server-prefix-delegation.conf
nobase_dist_doc_DATA += examples/server-relay.conf examples/server-relay-interface-id.conf
nobase_dist_doc_DATA += examples/server-script.conf examples/server-stateless.conf
nobase_dist_doc_DATA += examples/server-ta.conf examples/server-win32.conf examples/server-route.conf
nobase_dist_doc_DATA += examples/server-client-classification.conf examples/server-subnet.conf

dist_noinst_DATA = RELEASE-METHOD TEST-METHOD doxygen.cfg $(DOXYGEN) $(TEX_SOURCES) $(man8_MANS)

TEX_SOURCES = dibbler-aaa.png dibbler-cascade-relays.png dibbler-fqdn-cli-update.png
TEX_SOURCES += dibbler-fqdn-srv-update.png dibbler-multiple-cli.png dibbler-multiple-srv.png
TEX_SOURCES += dibbler-prefixes-host.png dibbler-prefixes-router.png dibbler-relay.png
TEX_SOURCES += dibbler-srv-cli.png dibbler-user-bibliography.tex dibbler-user-config-client.tex
TEX_SOURCES += dibbler-user-config-relay.tex dibbler-user-config-server.tex dibbler-user-epilogue.tex
TEX_SOURCES += dibbler-user-faq.tex dibbler-user-features.tex dibbler-user-intro.tex dibbler-user.tex
TEX_SOURCES += dibbler-user-usage.tex dibbler-worldmap.png logo-eti.png logo-eu.jpg logo-iip.png
TEX_SOURCES += logo-kti.png logo-nss.png logo-pg.png

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
DOXYGEN += doxygen.cfg

LATEX=pdflatex
LATEXOPTS=-file-line-error -halt-on-error

user: $(TEX_SOURCES)
	$(LATEX) $(LATEXOPTS) dibbler-user.tex > dibbler-user.log
	if grep "undefined" dibbler-user.log; then \
	      echo "There are undefined references";\
	fi

devel: $(DOXYGEN)
	mkdir -p html/
	doxygen doxygen.cfg > html/doxygen.log 2>html/doxygen-error.log
	cp logo* html/
	echo "Encountered `cat html/doxygen-error.log | wc -l` errors and warnings."

compile.log:
	mkdir -p html
	cd .. && ./configure &> doc/html/configure.log
	cd .. && $(MAKE) clean
	cd .. && $(MAKE) > doc/html/compile.log 2> doc/html/compile-warnings.log

check.log:
	mkdir -p html
	cd .. && ./configure --with-gtest=/home/thomson/devel/gtest-1.6.0 &> doc/html/configure-gtest.log
	cd .. && $(MAKE) -j2 check &> doc/html/make-check.log

distcheck.log:
	mkdir -p html
	cd .. && ./configure --with-gtest=/home/thomson/devel/gtest-1.6.0 &> doc/html/configure-gtest.log
	cd .. && $(MAKE) -j2 distcheck &> doc/html/make-distcheck.log

summary.log:
	mkdir -p html/
	wc -l html/compile-warnings.log html/cppcheck-error.log html/doxygen-error.log > html/summary.txt

clean:
	@echo "[CLEAN  ] $(SUBDIR)"
	@rm -f *.aux *.idx *.log *.toc *.out *~ *.ps *.dvi *.tmp

clobber: clean
	@rm -f *.pdf

.PHONY: devel cppcheck compile.log check.log distcheck.log
