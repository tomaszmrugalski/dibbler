#ifndef _SRVLEXER_H
#define _SRVLEXER_H

#include <clex.h>

#define COMMENT 2
#define ADDR 4
/////////////////////////////////////////////////////////////////////////////
// SrvLexer

#ifndef YYDECLSPEC
#define YYDECLSPEC
#endif

class YYFAR YYDECLSPEC SrvLexer : public yyclexer {
public:
	SrvLexer();

protected:
	void yytables();
	virtual int yyaction(int action);

public:
#line 32 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.l"

  unsigned ComBeg; //line, in which comment begins
  unsigned LftCnt; //how many signs : on the left side of 
                                    //'::' sign was interpreted
  unsigned RgtCnt; //the same as above, but on the right side of '::'
  char Address[16]; //address, which is analizing right now
  char AddrPart[16];
  unsigned intpos,pos;
  unsigned analyzeBigPart(char* bigPart, unsigned length, char *dst);
  int decodeSmallPart(char *src, char* dst,int length);
  void decodeIntPart(char *src, char *dst);
  void resetAddress(char *tab);

#line 39 "C:\\Dyplom\\sources\\SrvParser\\srvLexer.h"
};

#ifndef YYLEXNAME
#define YYLEXNAME SrvLexer
#endif

#ifndef INITIAL
#define INITIAL 0
#endif

#endif
