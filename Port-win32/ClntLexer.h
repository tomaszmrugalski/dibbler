#ifndef _CLNTLEXER_H
#define _CLNTLEXER_H

#include <clex.h>

#define COMMENT 2
#define ADDR 4
/////////////////////////////////////////////////////////////////////////////
// clntLexer

#ifndef YYDECLSPEC
#define YYDECLSPEC
#endif

class YYFAR YYDECLSPEC clntLexer : public yyflexer {
public:
	clntLexer();

protected:
	void yytables();
	virtual int yyaction(int action);

public:
#line 32 "C:\\Dyplom\\sources\\ClntParser\\ClntLexer.l"

  unsigned ComBeg; //line, in which comment begins
  unsigned LftCnt; //how many signs : on the left side of :: sign was interpreted
  unsigned RgtCnt; //the same as above, but on the right side of ::
  char Address[16]; //address, which is analizying right now
  char AddrPart[16];
  unsigned intpos,pos;
  unsigned analyzeBigPart(char* bigPart, unsigned length, char *dst);
  int decodeSmallPart(char *src, char* dst,int length);
  void decodeIntPart(char *src, char *dst);
  void resetAddress(char *tab);

#line 38 "C:\\Dyplom\\sources\\ClntParser\\ClntLexer.h"
};

#ifndef YYLEXNAME
#define YYLEXNAME clntLexer
#endif

#ifndef INITIAL
#define INITIAL 0
#endif

#endif
