
PASS3=NO

man8_MANS = man/dibbler-server.8 man/dibbler-client.8 man/dibbler-relay.8

nobase_dist_doc_DATA = dibbler-user.pdf
nobase_dist_doc_DATA += examples/client-addrparams.conf examples/client-auth.conf examples/client-autodetect.conf examples/client.conf examples/client-fqdn.conf examples/client-prefix-delegation.conf examples/client-stateless.conf examples/client-ta.conf examples/client-win32.conf 
nobase_dist_doc_DATA += examples/relay-1interface.conf examples/relay.conf examples/relay-echo-remoteid.conf 
nobase_dist_doc_DATA += examples/server-3classes.conf examples/server-addrparams.conf examples/server-auth.conf examples/server-bulk-lq.conf examples/server.conf examples/server-extraopts.conf examples/server-fqdn.conf examples/server-guess-mode.conf examples/server-leasequery.conf examples/server-per-client.conf examples/server-prefix-delegation.conf examples/server-relay.conf examples/server-relay-interface-id.conf examples/server-script.conf examples/server-stateless.conf examples/server-ta.conf examples/server-win32.conf examples/server-route.conf

dist_noinst_DATA = RELEASE-METHOD TEST-METHOD doxygen.cfg $(DOXYGEN) $(TEX_SOURCES) $(man8_MANS)

LATEX=pdflatex
DVIPS=dvips
PS2PDF=ps2pdf

LATEXOPTS=-file-line-error -halt-on-error

user: dibbler-user.pdf

TEX_SOURCES = dibbler-aaa.png dibbler-cascade-relays.png dibbler-fqdn-cli-update.png dibbler-fqdn-srv-update.png dibbler-multiple-cli.png dibbler-multiple-srv.png dibbler-prefixes-host.png dibbler-prefixes-router.png dibbler-relay.png dibbler-srv-cli.png dibbler-user-bibliography.tex dibbler-user-config-client.tex dibbler-user-config-relay.tex dibbler-user-config-server.tex dibbler-user-epilogue.tex dibbler-user-faq.tex dibbler-user-features.tex dibbler-user-intro.tex dibbler-user.tex dibbler-user-usage.tex dibbler-worldmap.png logo-eti.png logo-eu.jpg logo-iip.png logo-kti.png logo-nss.png logo-pg.png

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

all:

all-am:

user: dibbler-user.tex dibbler-user-config-client.tex \
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
	  -grep "undefined" dibbler-user-3rd_pass.log;\
	fi
	if grep "undefined" dibbler-user-1st_pass.log; then \
		echo "There are undefined references";\
	fi

devel: $(DOXYGEN)
	doxygen doxygen.cfg > html/doxygen.log 2>html/doxygen-error.log
	cp logo* html/
	echo "Encountered `wc -l html/doxygen-error.log` errors and warnings."

version.tex:
	../scripts/xtract_version > version.tex

clean:
	@echo "[CLEAN  ] $(SUBDIR)"
	@rm -f *.aux *.idx *.log *.toc *.out *~ *.ps *.dvi *.tmp version.tex

clobber: clean
	@rm -f *.pdf

.PHONY: version.tex devel
