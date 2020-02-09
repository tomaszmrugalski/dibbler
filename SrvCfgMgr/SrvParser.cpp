// A Bison parser, made by GNU Bison 3.4.1.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2019 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// Undocumented macros, especially those whose name start with YY_,
// are private implementation details.  Do not rely on them.





#include "SrvParser.h"


// Unqualified %code blocks.
#line 21 "SrvParser.y"

#include <iostream>
#include <string>
#include <cstdint>
#include <sstream>
#include "Portable.h"
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Container.h"
#include "SrvParser.h"
// #include "SrvParsGlobalOpt.h"
#include "SrvParsClassOpt.h"
#include "SrvParsIfaceOpt.h"
#include "OptAddr.h"
//#include "OptAddrLst.h"
#include "OptDomainLst.h"
#include "OptString.h"
#include "OptVendorSpecInfo.h"
#include "OptRtPrefix.h"
#include "SrvOptAddrParams.h"
//#include "SrvCfgMgr.h"
//#include "SrvCfgTA.h"
//#include "SrvCfgPD.h"
#include "SrvCfgClientClass.h"
#include "SrvCfgAddrClass.h"
#include "SrvCfgIface.h"
#include "SrvCfgOptions.h"
#include "DUID.h"
#include "Logger.h"
#include "FQDN.h"
//#include "Key.h"
#include "Node.h"
#include "NodeConstant.h"
#include "NodeClientSpecific.h"
#include "NodeOperator.h"


using namespace std;

//#define YY_USE_CLASS


//#include <FlexLexer.h>

#line 76 "SrvParser.y"

#include "SrvParserContext.h"

#line 94 "SrvParser.cpp"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 5 "SrvParser.y"
namespace dibbler {
#line 189 "SrvParser.cpp"


  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  SrvParser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }


  /// Build a parser object.
  SrvParser::SrvParser (SrvParserContext& ctx_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      ctx (ctx_yyarg)
  {}

  SrvParser::~SrvParser ()
  {}

  SrvParser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | Symbol types.  |
  `---------------*/



  // by_state.
  SrvParser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  SrvParser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  SrvParser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  SrvParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  SrvParser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  SrvParser::symbol_number_type
  SrvParser::by_state::type_get () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  SrvParser::stack_symbol_type::stack_symbol_type ()
  {}

  SrvParser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.type_get ())
    {
      case 109: // STRING_
      case 112: // IPV6ADDR_
      case 113: // DUID_
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      case 110: // HEXNUMBER_
      case 111: // INTNUMBER_
      case 172: // Number
        value.YY_MOVE_OR_COPY< uint32_t > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  SrvParser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.type_get ())
    {
      case 109: // STRING_
      case 112: // IPV6ADDR_
      case 113: // DUID_
        value.move< std::string > (YY_MOVE (that.value));
        break;

      case 110: // HEXNUMBER_
      case 111: // INTNUMBER_
      case 172: // Number
        value.move< uint32_t > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.type = empty_symbol;
  }

#if YY_CPLUSPLUS < 201103L
  SrvParser::stack_symbol_type&
  SrvParser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.type_get ())
    {
      case 109: // STRING_
      case 112: // IPV6ADDR_
      case 113: // DUID_
        value.move< std::string > (that.value);
        break;

      case 110: // HEXNUMBER_
      case 111: // INTNUMBER_
      case 172: // Number
        value.move< uint32_t > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  SrvParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  SrvParser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
#if defined __GNUC__ && ! defined __clang__ && ! defined __ICC && __GNUC__ * 100 + __GNUC_MINOR__ <= 408
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
#endif
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  void
  SrvParser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  SrvParser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  SrvParser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  SrvParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  SrvParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  SrvParser::debug_level_type
  SrvParser::debug_level () const
  {
    return yydebug_;
  }

  void
  SrvParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  SrvParser::state_type
  SrvParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  bool
  SrvParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  SrvParser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  SrvParser::operator() ()
  {
    return parse ();
  }

  int
  SrvParser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << '\n';

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex (ctx));
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case 109: // STRING_
      case 112: // IPV6ADDR_
      case 113: // DUID_
        yylhs.value.emplace< std::string > ();
        break;

      case 110: // HEXNUMBER_
      case 111: // INTNUMBER_
      case 172: // Number
        yylhs.value.emplace< uint32_t > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 68:
#line 306 "SrvParser.y"
    {
    if (!ctx.StartIfaceDeclaration(yystack_[1].value.as < std::string > ())) {
	YYABORT;
    }
}
#line 652 "SrvParser.cpp"
    break;

  case 69:
#line 312 "SrvParser.y"
    {
    //Information about new interface has been read
    //Add it to list of read interfaces
    ctx.EndIfaceDeclaration();
}
#line 662 "SrvParser.cpp"
    break;

  case 70:
#line 319 "SrvParser.y"
    {
    if (!ctx.StartIfaceDeclaration(yystack_[1].value.as < uint32_t > ())) {
	YYABORT;
    }
}
#line 672 "SrvParser.cpp"
    break;

  case 71:
#line 325 "SrvParser.y"
    {
    ctx.EndIfaceDeclaration();
}
#line 680 "SrvParser.cpp"
    break;

  case 82:
#line 344 "SrvParser.y"
    {
    /// this is key object initialization part
    ctx.CurrentKey_ = new TSIGKey(string(yystack_[1].value.as < std::string > ()));
}
#line 689 "SrvParser.cpp"
    break;

  case 83:
#line 349 "SrvParser.y"
    {
    /// check that both secret and algorithm keywords were defined.
    Log(Debug) << "Loaded key '" << ctx.CurrentKey_->Name_ << "', base64len is "
	       << ctx.CurrentKey_->getBase64Data().length() << ", rawlen is "
	       << ctx.CurrentKey_->getPackedData().length() << "." << LogEnd;
    if (ctx.CurrentKey_->getPackedData().length() == 0) {
	Log(Crit) << "Key " << ctx.CurrentKey_->Name_ << " does not have secret specified." << LogEnd;
	YYABORT;
    }

    if ( (ctx.CurrentKey_->Digest_ != DIGEST_HMAC_MD5) &&
	 (ctx.CurrentKey_->Digest_ != DIGEST_HMAC_SHA1) &&
	 (ctx.CurrentKey_->Digest_ != DIGEST_HMAC_SHA256) ) {
	Log(Crit) << "Invalid key type specified: only hmac-md5, hmac-sha1 and "
                  << "hmac-sha256 are supported." << LogEnd;
	YYABORT;
    }
#if !defined(MOD_SRV_DISABLE_DNSUPDATE) && !defined(MOD_CLNT_DISABLE_DNSUPDATE)
    ctx.CfgMgr_->addKey( ctx.CurrentKey_ );
#else
    Log(Crit) << "DNS Update disabled at compilation time. Can't specify TSIG key."
              << LogEnd;
#endif
}
#line 718 "SrvParser.cpp"
    break;

  case 90:
#line 388 "SrvParser.y"
    {
    // store the key in base64 encoded form
    ctx.CurrentKey_->setData(string(yystack_[1].value.as < std::string > ()));
}
#line 727 "SrvParser.cpp"
    break;

  case 91:
#line 395 "SrvParser.y"
    {
    ctx.CurrentKey_->Fudge_ = yystack_[1].value.as < uint32_t > ();
}
#line 735 "SrvParser.cpp"
    break;

  case 92:
#line 400 "SrvParser.y"
    { ctx.CurrentKey_->Digest_ = DIGEST_HMAC_SHA256; }
#line 741 "SrvParser.cpp"
    break;

  case 93:
#line 401 "SrvParser.y"
    { ctx.CurrentKey_->Digest_ = DIGEST_HMAC_SHA1;  }
#line 747 "SrvParser.cpp"
    break;

  case 94:
#line 402 "SrvParser.y"
    { ctx.CurrentKey_->Digest_ = DIGEST_HMAC_MD5;  }
#line 753 "SrvParser.cpp"
    break;

  case 95:
#line 408 "SrvParser.y"
    {
    ctx.ParserOptStack_.append(new TSrvParsGlobalOpt());
    SPtr<TDUID> duid = new TDUID(yystack_[1].value.as < std::string > ());
    ctx.ClientLst_.append(new TSrvCfgOptions(duid));
}
#line 763 "SrvParser.cpp"
    break;

  case 96:
#line 414 "SrvParser.y"
    {
    Log(Debug) << "Exception: DUID-based exception specified." << LogEnd;
    // copy all defined options
    ctx.ClientLst_.getLast()->setOptions(ctx.ParserOptStack_.getLast());
    ctx.ParserOptStack_.delLast();
}
#line 774 "SrvParser.cpp"
    break;

  case 97:
#line 422 "SrvParser.y"
    {
    ctx.ParserOptStack_.append(new TSrvParsGlobalOpt());
    SPtr<TOptVendorData> remoteid = new TOptVendorData(OPTION_REMOTE_ID, yystack_[3].value.as < uint32_t > (), yystack_[1].value.as < std::string > ().c_str(), yystack_[1].value.as < std::string > ().length(), 0);
    ctx.ClientLst_.append(new TSrvCfgOptions(remoteid));
}
#line 784 "SrvParser.cpp"
    break;

  case 98:
#line 428 "SrvParser.y"
    {
    Log(Debug) << "Exception: RemoteID-based exception specified." << LogEnd;
    // copy all defined options
    ctx.ClientLst_.getLast()->setOptions(ctx.ParserOptStack_.getLast());
    ctx.ParserOptStack_.delLast();
}
#line 795 "SrvParser.cpp"
    break;

  case 99:
#line 436 "SrvParser.y"
    {
		ctx.ParserOptStack_.append(new TSrvParsGlobalOpt());
		SPtr<TIPv6Addr> clntaddr = new TIPv6Addr(yystack_[1].value.as < std::string > ());
		ctx.ClientLst_.append(new TSrvCfgOptions(clntaddr));
}
#line 805 "SrvParser.cpp"
    break;

  case 100:
#line 442 "SrvParser.y"
    {
		Log(Debug) << "Exception: Link-local-based exception specified." << LogEnd;
		// copy all defined options
		ctx.ClientLst_.getLast()->setOptions(ctx.ParserOptStack_.getLast());
		ctx.ParserOptStack_.delLast();
}
#line 816 "SrvParser.cpp"
    break;

  case 119:
#line 475 "SrvParser.y"
    {
    ctx.addr_ = new TIPv6Addr(yystack_[0].value.as < std::string > ());
    Log(Info) << "Exception: Address " << ctx.addr_->getPlain() << " reserved." << LogEnd;
    ctx.ClientLst_.getLast()->setAddr(ctx.addr_);
}
#line 826 "SrvParser.cpp"
    break;

  case 120:
#line 483 "SrvParser.y"
    {
    ctx.addr_ = new TIPv6Addr(yystack_[2].value.as < std::string > ());
    Log(Info) << "Exception: Prefix " << ctx.addr_->getPlain() << "/" << yystack_[0].value.as < uint32_t > () << " reserved." << LogEnd;
    ctx.ClientLst_.getLast()->setPrefix(ctx.addr_, yystack_[0].value.as < uint32_t > ());
}
#line 836 "SrvParser.cpp"
    break;

  case 121:
#line 492 "SrvParser.y"
    {
    ctx.StartClassDeclaration();
}
#line 844 "SrvParser.cpp"
    break;

  case 122:
#line 496 "SrvParser.y"
    {
    if (!ctx.EndClassDeclaration()) {
	YYABORT;
    }
}
#line 854 "SrvParser.cpp"
    break;

  case 125:
#line 511 "SrvParser.y"
    {
    ctx.StartTAClassDeclaration();
}
#line 862 "SrvParser.cpp"
    break;

  case 126:
#line 514 "SrvParser.y"
    {
    if (!ctx.EndTAClassDeclaration()) {
	YYABORT;
    }
}
#line 872 "SrvParser.cpp"
    break;

  case 137:
#line 539 "SrvParser.y"
    {
    ctx.StartPDDeclaration();
}
#line 880 "SrvParser.cpp"
    break;

  case 138:
#line 542 "SrvParser.y"
    {
    if (!ctx.EndPDDeclaration()) {
	YYABORT;
    }
}
#line 890 "SrvParser.cpp"
    break;

  case 149:
#line 570 "SrvParser.y"
    {
    SPtr<TIPv6Addr> routerAddr = new TIPv6Addr(yystack_[1].value.as < std::string > ());
    SPtr<TOpt> myNextHop = new TOptAddr(OPTION_NEXT_HOP, routerAddr, NULL);
    ctx.nextHop = myNextHop; 
}
#line 900 "SrvParser.cpp"
    break;

  case 150:
#line 576 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->addExtraOption(ctx.nextHop, false);
    ctx.nextHop.reset();
}
#line 909 "SrvParser.cpp"
    break;

  case 151:
#line 581 "SrvParser.y"
    {
    SPtr<TIPv6Addr> routerAddr = new TIPv6Addr(yystack_[0].value.as < std::string > ());
    SPtr<TOpt> myNextHop = new TOptAddr(OPTION_NEXT_HOP, routerAddr, NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(myNextHop, false);
}
#line 919 "SrvParser.cpp"
    break;

  case 154:
#line 595 "SrvParser.y"
    {
    SPtr<TIPv6Addr> prefix = new TIPv6Addr(yystack_[4].value.as < std::string > ());
    SPtr<TOpt> rtPrefix = new TOptRtPrefix(yystack_[0].value.as < uint32_t > (), yystack_[2].value.as < uint32_t > (), 42, prefix, NULL);
    if (ctx.nextHop)
        ctx.nextHop->addOption(rtPrefix);
    else
        ctx.ParserOptStack_.getLast()->addExtraOption(rtPrefix, false);
}
#line 932 "SrvParser.cpp"
    break;

  case 155:
#line 604 "SrvParser.y"
    {
    SPtr<TIPv6Addr> prefix = new TIPv6Addr(yystack_[2].value.as < std::string > ());
    SPtr<TOpt> rtPrefix = new TOptRtPrefix(DHCPV6_INFINITY, yystack_[0].value.as < uint32_t > (), 42, prefix, NULL);
    if (ctx.nextHop)
        ctx.nextHop->addOption(rtPrefix);
    else
        ctx.ParserOptStack_.getLast()->addExtraOption(rtPrefix, false);
}
#line 945 "SrvParser.cpp"
    break;

  case 156:
#line 613 "SrvParser.y"
    {
    SPtr<TIPv6Addr> prefix = new TIPv6Addr(yystack_[4].value.as < std::string > ());
    SPtr<TOpt> rtPrefix = new TOptRtPrefix(DHCPV6_INFINITY, yystack_[2].value.as < uint32_t > (), 42, prefix, NULL);
    if (ctx.nextHop)
        ctx.nextHop->addOption(rtPrefix);
    else
        ctx.ParserOptStack_.getLast()->addExtraOption(rtPrefix, false);
}
#line 958 "SrvParser.cpp"
    break;

  case 157:
#line 623 "SrvParser.y"
    {

#ifndef MOD_DISABLE_AUTH
    if (yystack_[0].value.as < std::string > () == "none") {
        ctx.CfgMgr_->setAuthProtocol(AUTH_PROTO_NONE);
        ctx.CfgMgr_->setAuthAlgorithm(AUTH_ALGORITHM_NONE);
    } else if (yystack_[0].value.as < std::string > () == "delayed") {
        ctx.CfgMgr_->setAuthProtocol(AUTH_PROTO_DELAYED);
    } else if (yystack_[0].value.as < std::string > () == "reconfigure-key") {
        ctx.CfgMgr_->setAuthProtocol(AUTH_PROTO_RECONFIGURE_KEY);
        ctx.CfgMgr_->setAuthAlgorithm(AUTH_ALGORITHM_RECONFIGURE_KEY);
    } else if (yystack_[0].value.as < std::string > () == "dibbler") {
        ctx.CfgMgr_->setAuthProtocol(AUTH_PROTO_DIBBLER);
    } else {
        Log(Crit) << "Invalid auth-protocol parameter: " << string(yystack_[0].value.as < std::string > ()) << LogEnd;
        YYABORT;
    }
#else
    Log(Crit) << "Auth support disabled at compilation time." << LogEnd;
#endif
}
#line 984 "SrvParser.cpp"
    break;

  case 158:
#line 646 "SrvParser.y"
    {
    Log(Crit) << "auth-algorithm secification is not supported yet." << LogEnd;
    YYABORT;
}
#line 993 "SrvParser.cpp"
    break;

  case 159:
#line 652 "SrvParser.y"
    {

#ifndef MOD_DISABLE_AUTH
    if (yystack_[0].value.as < std::string > () == "none") {
        ctx.CfgMgr_->setAuthReplay(AUTH_REPLAY_NONE);
    } else if (yystack_[0].value.as < std::string > () == "monotonic") {
        ctx.CfgMgr_->setAuthReplay(AUTH_REPLAY_MONOTONIC);
    } else {
        Log(Crit) << "Invalid auth-replay parameter: " << string(yystack_[0].value.as < std::string > ()) << LogEnd;
        YYABORT;
    }
#else
    Log(Crit) << "Auth support disabled at compilation time." << LogEnd;
#endif

}
#line 1014 "SrvParser.cpp"
    break;

  case 160:
#line 670 "SrvParser.y"
    {
#ifndef MOD_DISABLE_AUTH
    ctx.CfgMgr_->setAuthRealm(string(yystack_[0].value.as < std::string > ()));
#else
    Log(Crit) << "Auth support disabled at compilation time." << LogEnd;
#endif
}
#line 1026 "SrvParser.cpp"
    break;

  case 161:
#line 680 "SrvParser.y"
    {
    ctx.DigestLst_.clear();
}
#line 1034 "SrvParser.cpp"
    break;

  case 162:
#line 682 "SrvParser.y"
    {
#ifndef MOD_DISABLE_AUTH
    ctx.CfgMgr_->setAuthDigests(ctx.DigestLst_);
    ctx.CfgMgr_->setAuthDropUnauthenticated(true);
    ctx.DigestLst_.clear();
#else
    Log(Crit) << "Auth support disabled at compilation time." << LogEnd;
#endif
}
#line 1048 "SrvParser.cpp"
    break;

  case 165:
#line 698 "SrvParser.y"
    { ctx.DigestLst_.push_back(DIGEST_NONE); }
#line 1054 "SrvParser.cpp"
    break;

  case 166:
#line 699 "SrvParser.y"
    { ctx.DigestLst_.push_back(DIGEST_PLAIN); }
#line 1060 "SrvParser.cpp"
    break;

  case 167:
#line 700 "SrvParser.y"
    { ctx.DigestLst_.push_back(DIGEST_HMAC_MD5); }
#line 1066 "SrvParser.cpp"
    break;

  case 168:
#line 701 "SrvParser.y"
    { ctx.DigestLst_.push_back(DIGEST_HMAC_SHA1); }
#line 1072 "SrvParser.cpp"
    break;

  case 169:
#line 702 "SrvParser.y"
    { ctx.DigestLst_.push_back(DIGEST_HMAC_SHA224); }
#line 1078 "SrvParser.cpp"
    break;

  case 170:
#line 703 "SrvParser.y"
    { ctx.DigestLst_.push_back(DIGEST_HMAC_SHA256); }
#line 1084 "SrvParser.cpp"
    break;

  case 171:
#line 704 "SrvParser.y"
    { ctx.DigestLst_.push_back(DIGEST_HMAC_SHA384); }
#line 1090 "SrvParser.cpp"
    break;

  case 172:
#line 705 "SrvParser.y"
    { ctx.DigestLst_.push_back(DIGEST_HMAC_SHA512); }
#line 1096 "SrvParser.cpp"
    break;

  case 173:
#line 710 "SrvParser.y"
    {
#ifndef MOD_DISABLE_AUTH
    ctx.CfgMgr_->setAuthDropUnauthenticated(yystack_[0].value.as < uint32_t > ());
#else
    Log(Crit) << "Auth support disabled at compilation time." << LogEnd;
#endif
}
#line 1108 "SrvParser.cpp"
    break;

  case 174:
#line 728 "SrvParser.y"
    {
    Log(Notice)<< "FQDN: The client "<<yystack_[0].value.as < std::string > ()<<" has no address nor DUID"<<LogEnd;
    ctx.PresentFQDNLst_.append(new TFQDN(yystack_[0].value.as < std::string > (),false));
}
#line 1117 "SrvParser.cpp"
    break;

  case 175:
#line 733 "SrvParser.y"
    {
    /// @todo: Use SPtr()
    TDUID* duidNew = new TDUID(yystack_[0].value.as < std::string > ());
    Log(Debug)<< "FQDN:" << yystack_[2].value.as < std::string > () <<" reserved for DUID " << duidNew->getPlain()<<LogEnd;
    ctx.PresentFQDNLst_.append(new TFQDN(duidNew, yystack_[2].value.as < std::string > (),false));
}
#line 1128 "SrvParser.cpp"
    break;

  case 176:
#line 740 "SrvParser.y"
    {
    ctx.addr_= new TIPv6Addr(yystack_[0].value.as < std::string > ());
    Log(Debug)<< "FQDN:" << yystack_[2].value.as < std::string > () << " reserved for address " << ctx.addr_->getPlain() << LogEnd;
    ctx.PresentFQDNLst_.append(new TFQDN(new TIPv6Addr(yystack_[0].value.as < std::string > ()), yystack_[2].value.as < std::string > (),false));
}
#line 1138 "SrvParser.cpp"
    break;

  case 177:
#line 746 "SrvParser.y"
    {
	Log(Debug) << "FQDN:"<<yystack_[0].value.as < std::string > ()<<" has no reservations (is available to everyone)."<<LogEnd;
    ctx.PresentFQDNLst_.append(new TFQDN(yystack_[0].value.as < std::string > (),false));
}
#line 1147 "SrvParser.cpp"
    break;

  case 178:
#line 751 "SrvParser.y"
    {
    TDUID* duidNew = new TDUID(yystack_[0].value.as < std::string > ());
    Log(Debug) << "FQDN:" << yystack_[2].value.as < std::string > () << " reserved for DUID "<< duidNew->getPlain() << LogEnd;
    ctx.PresentFQDNLst_.append(new TFQDN( duidNew, yystack_[2].value.as < std::string > (),false));
}
#line 1157 "SrvParser.cpp"
    break;

  case 179:
#line 757 "SrvParser.y"
    {
    ctx.addr_ = new TIPv6Addr(yystack_[0].value.as < std::string > ());
    Log(Debug) << "FQDN:" << yystack_[2].value.as < std::string > () << " reserved for address " << ctx.addr_->getPlain() << LogEnd;
    ctx.PresentFQDNLst_.append(new TFQDN(new TIPv6Addr(yystack_[0].value.as < std::string > ()), yystack_[2].value.as < std::string > (),false));
}
#line 1167 "SrvParser.cpp"
    break;

  case 180:
#line 765 "SrvParser.y"
    {yylhs.value.as < uint32_t > ()=yystack_[0].value.as < uint32_t > ();}
#line 1173 "SrvParser.cpp"
    break;

  case 181:
#line 766 "SrvParser.y"
    {yylhs.value.as < uint32_t > ()=yystack_[0].value.as < uint32_t > ();}
#line 1179 "SrvParser.cpp"
    break;

  case 182:
#line 771 "SrvParser.y"
    {
    ctx.PresentAddrLst_.append(new TIPv6Addr(yystack_[0].value.as < std::string > ()));
}
#line 1187 "SrvParser.cpp"
    break;

  case 183:
#line 775 "SrvParser.y"
    {
    ctx.PresentAddrLst_.append(new TIPv6Addr(yystack_[0].value.as < std::string > ()));
}
#line 1195 "SrvParser.cpp"
    break;

  case 184:
#line 782 "SrvParser.y"
    {
    Log(Debug) << "Vendor-spec defined: Enterprise: " << yystack_[4].value.as < uint32_t > () << ", optionCode: "
	       << yystack_[2].value.as < uint32_t > () << ", valuelen=" << yystack_[0].value.as < std::string > ().length() << LogEnd;

    ctx.ParserOptStack_.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, yystack_[4].value.as < uint32_t > (), yystack_[2].value.as < uint32_t > (), yystack_[0].value.as < std::string > (), 0), false);
}
#line 1206 "SrvParser.cpp"
    break;

  case 185:
#line 789 "SrvParser.y"
    {
    SPtr<TIPv6Addr> addr(new TIPv6Addr(yystack_[0].value.as < std::string > ()));
    Log(Debug) << "Vendor-spec defined: Enterprise: " << yystack_[4].value.as < uint32_t > () << ", optionCode: "
               << yystack_[2].value.as < uint32_t > () << ", value=" << addr->getPlain() << LogEnd;
    ctx.ParserOptStack_.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, yystack_[4].value.as < uint32_t > (), yystack_[2].value.as < uint32_t > (),
								    new TIPv6Addr(yystack_[0].value.as < std::string > ()), 0), false);
}
#line 1218 "SrvParser.cpp"
    break;

  case 186:
#line 797 "SrvParser.y"
    {
    Log(Debug) << "Vendor-spec defined: Enterprise: " << yystack_[4].value.as < uint32_t > () << ", optionCode: "
	       << yystack_[2].value.as < uint32_t > () << ", valuelen=" << yystack_[0].value.as < std::string > ().length() << LogEnd;

    ctx.ParserOptStack_.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, yystack_[4].value.as < uint32_t > (), yystack_[2].value.as < uint32_t > (),
								    yystack_[0].value.as < std::string > (), 0), false);
}
#line 1230 "SrvParser.cpp"
    break;

  case 187:
#line 805 "SrvParser.y"
    {
    Log(Debug) << "Vendor-spec defined: Enterprise: " << yystack_[4].value.as < uint32_t > () << ", optionCode: "
	       << yystack_[2].value.as < uint32_t > () << ", valuelen=" << yystack_[0].value.as < std::string > ().length() << LogEnd;
    ctx.ParserOptStack_.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, yystack_[4].value.as < uint32_t > (), yystack_[2].value.as < uint32_t > (), yystack_[0].value.as < std::string > (), 0), false);
}
#line 1240 "SrvParser.cpp"
    break;

  case 188:
#line 811 "SrvParser.y"
    {
    SPtr<TIPv6Addr> addr(new TIPv6Addr(yystack_[0].value.as < std::string > ()));
    Log(Debug) << "Vendor-spec defined: Enterprise: " << yystack_[4].value.as < uint32_t > () << ", optionCode: "
               << yystack_[2].value.as < uint32_t > () << ", value=" << addr->getPlain() << LogEnd;
    ctx.ParserOptStack_.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, yystack_[4].value.as < uint32_t > (), yystack_[2].value.as < uint32_t > (),
								    addr, 0), false);
}
#line 1252 "SrvParser.cpp"
    break;

  case 189:
#line 819 "SrvParser.y"
    {
    Log(Debug) << "Vendor-spec defined: Enterprise: " << yystack_[4].value.as < uint32_t > () << ", optionCode: "
	       << yystack_[2].value.as < uint32_t > () << ", valuelen=" << yystack_[0].value.as < std::string > ().length() << LogEnd;
    ctx.ParserOptStack_.getLast()->addExtraOption(new TOptVendorSpecInfo(OPTION_VENDOR_OPTS, yystack_[4].value.as < uint32_t > (), yystack_[2].value.as < uint32_t > (),
								    yystack_[0].value.as < std::string > (), 0), false);
}
#line 1263 "SrvParser.cpp"
    break;

  case 190:
#line 828 "SrvParser.y"
    { ctx.PresentStringLst_.append(SPtr<string> (new string(yystack_[0].value.as < std::string > ()))); }
#line 1269 "SrvParser.cpp"
    break;

  case 191:
#line 829 "SrvParser.y"
    { ctx.PresentStringLst_.append(SPtr<string> (new string(yystack_[0].value.as < std::string > ()))); }
#line 1275 "SrvParser.cpp"
    break;

  case 192:
#line 834 "SrvParser.y"
    {
	ctx.PresentRangeLst_.append(new THostRange(new TIPv6Addr(yystack_[0].value.as < std::string > ()),new TIPv6Addr(yystack_[0].value.as < std::string > ())));
    }
#line 1283 "SrvParser.cpp"
    break;

  case 193:
#line 838 "SrvParser.y"
    {
	SPtr<TIPv6Addr> addr1(new TIPv6Addr(yystack_[2].value.as < std::string > ()));
	SPtr<TIPv6Addr> addr2(new TIPv6Addr(yystack_[0].value.as < std::string > ()));
	if (*addr1<=*addr2)
	    ctx.PresentRangeLst_.append(new THostRange(addr1,addr2));
	else
	    ctx.PresentRangeLst_.append(new THostRange(addr2,addr1));
    }
#line 1296 "SrvParser.cpp"
    break;

  case 194:
#line 847 "SrvParser.y"
    {
	SPtr<TIPv6Addr> addr(new TIPv6Addr(yystack_[2].value.as < std::string > ()));
	int prefix = yystack_[0].value.as < uint32_t > ();
	if ( (prefix<1) || (prefix>128) ) {
	    error(yystack_[0].location, "Invalid prefix defined: " + to_string(prefix) + ". Allowed range: 1..128.");
	    YYABORT;
	}
	SPtr<TIPv6Addr> addr1 = ctx.getRangeMin(yystack_[2].value.as < std::string > ().c_str(), prefix);
	SPtr<TIPv6Addr> addr2 = ctx.getRangeMax(yystack_[2].value.as < std::string > ().c_str(), prefix);
	if (*addr1<=*addr2)
	    ctx.PresentRangeLst_.append(new THostRange(addr1,addr2));
	else
	    ctx.PresentRangeLst_.append(new THostRange(addr2,addr1));
    }
#line 1315 "SrvParser.cpp"
    break;

  case 195:
#line 862 "SrvParser.y"
    {
	ctx.PresentRangeLst_.append(new THostRange(new TIPv6Addr(yystack_[0].value.as < std::string > ()),new TIPv6Addr(yystack_[0].value.as < std::string > ())));
    }
#line 1323 "SrvParser.cpp"
    break;

  case 196:
#line 866 "SrvParser.y"
    {
	SPtr<TIPv6Addr> addr1(new TIPv6Addr(yystack_[2].value.as < std::string > ()));
	SPtr<TIPv6Addr> addr2(new TIPv6Addr(yystack_[0].value.as < std::string > ()));
	if (*addr1<=*addr2)
	    ctx.PresentRangeLst_.append(new THostRange(addr1,addr2));
	else
	    ctx.PresentRangeLst_.append(new THostRange(addr2,addr1));
    }
#line 1336 "SrvParser.cpp"
    break;

  case 197:
#line 878 "SrvParser.y"
    {
	SPtr<TIPv6Addr> addr(new TIPv6Addr(yystack_[2].value.as < std::string > ()));
	int prefix = yystack_[0].value.as < uint32_t > ();
	if ( (prefix<1) || (prefix>128)) {
	    error(yystack_[0].location, "Invalid prefix defined: " + to_string(prefix) + ". Allowed range: 1..128.");
            YYABORT;
	}

	SPtr<TIPv6Addr> addr1 = ctx.getRangeMin(yystack_[2].value.as < std::string > ().c_str(), prefix);
	SPtr<TIPv6Addr> addr2 = ctx.getRangeMax(yystack_[2].value.as < std::string > ().c_str(), prefix);
	SPtr<THostRange> range;
	if (*addr1<=*addr2)
	    range = new THostRange(addr1,addr2);
	else
	    range = new THostRange(addr2,addr1);
	range->setPrefixLength(prefix);
	ctx.PDLst_.append(range);
    }
#line 1359 "SrvParser.cpp"
    break;

  case 198:
#line 900 "SrvParser.y"
    {
    ctx.PresentRangeLst_.append(new THostRange(new TIPv6Addr(yystack_[0].value.as < std::string > ()),new TIPv6Addr(yystack_[0].value.as < std::string > ())));
}
#line 1367 "SrvParser.cpp"
    break;

  case 199:
#line 904 "SrvParser.y"
    {
    SPtr<TIPv6Addr> addr1(new TIPv6Addr(yystack_[2].value.as < std::string > ()));
    SPtr<TIPv6Addr> addr2(new TIPv6Addr(yystack_[0].value.as < std::string > ()));
    if (*addr1<=*addr2)
	ctx.PresentRangeLst_.append(new THostRange(addr1,addr2));
    else
	ctx.PresentRangeLst_.append(new THostRange(addr2,addr1));
}
#line 1380 "SrvParser.cpp"
    break;

  case 200:
#line 913 "SrvParser.y"
    {
    ctx.PresentRangeLst_.append(new THostRange(new TIPv6Addr(yystack_[0].value.as < std::string > ()),new TIPv6Addr(yystack_[0].value.as < std::string > ())));
}
#line 1388 "SrvParser.cpp"
    break;

  case 201:
#line 917 "SrvParser.y"
    {
    SPtr<TIPv6Addr> addr1(new TIPv6Addr(yystack_[2].value.as < std::string > ()));
    SPtr<TIPv6Addr> addr2(new TIPv6Addr(yystack_[0].value.as < std::string > ()));
    if (*addr1<=*addr2)
	ctx.PresentRangeLst_.append(new THostRange(addr1,addr2));
    else
	ctx.PresentRangeLst_.append(new THostRange(addr2,addr1));
}
#line 1401 "SrvParser.cpp"
    break;

  case 202:
#line 926 "SrvParser.y"
    {
    SPtr<TDUID> duid(new TDUID(yystack_[0].value.as < std::string > ()));
    ctx.PresentRangeLst_.append(new THostRange(duid, duid));
}
#line 1410 "SrvParser.cpp"
    break;

  case 203:
#line 931 "SrvParser.y"
    {
    SPtr<TDUID> duid1(new TDUID(yystack_[2].value.as < std::string > ()));
    SPtr<TDUID> duid2(new TDUID(yystack_[0].value.as < std::string > ()));

    if (*duid1<=*duid2)
	ctx.PresentRangeLst_.append(new THostRange(duid1,duid2));
    else
	ctx.PresentRangeLst_.append(new THostRange(duid2,duid1));
}
#line 1424 "SrvParser.cpp"
    break;

  case 204:
#line 941 "SrvParser.y"
    {
    SPtr<TDUID> duid(new TDUID(yystack_[0].value.as < std::string > ()));
    ctx.PresentRangeLst_.append(new THostRange(duid, duid));
}
#line 1433 "SrvParser.cpp"
    break;

  case 205:
#line 946 "SrvParser.y"
    {
    SPtr<TDUID> duid2(new TDUID(yystack_[2].value.as < std::string > ()));
    SPtr<TDUID> duid1(new TDUID(yystack_[0].value.as < std::string > ()));
    if (*duid1<=*duid2)
	ctx.PresentRangeLst_.append(new THostRange(duid1,duid2));
    else
	ctx.PresentRangeLst_.append(new THostRange(duid2,duid1));
}
#line 1446 "SrvParser.cpp"
    break;

  case 206:
#line 958 "SrvParser.y"
    {
    ctx.PresentRangeLst_.clear();
}
#line 1454 "SrvParser.cpp"
    break;

  case 207:
#line 961 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setRejedClnt(&ctx.PresentRangeLst_);
}
#line 1462 "SrvParser.cpp"
    break;

  case 208:
#line 968 "SrvParser.y"
    {
    ctx.PresentRangeLst_.clear();
}
#line 1470 "SrvParser.cpp"
    break;

  case 209:
#line 971 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setAcceptClnt(&ctx.PresentRangeLst_);
}
#line 1478 "SrvParser.cpp"
    break;

  case 210:
#line 978 "SrvParser.y"
    {
    ctx.PresentRangeLst_.clear();
}
#line 1486 "SrvParser.cpp"
    break;

  case 211:
#line 981 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setPool(&ctx.PresentRangeLst_);
}
#line 1494 "SrvParser.cpp"
    break;

  case 212:
#line 988 "SrvParser.y"
    {
}
#line 1501 "SrvParser.cpp"
    break;

  case 213:
#line 990 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setPool(&ctx.PresentRangeLst_/*PDList*/);
}
#line 1509 "SrvParser.cpp"
    break;

  case 214:
#line 996 "SrvParser.y"
    {
    if ( ((yystack_[0].value.as < uint32_t > ()) > 128) || ((yystack_[0].value.as < uint32_t > ()) < 1) ) {
        Log(Crit) << "Invalid pd-length:" << yystack_[0].value.as < uint32_t > () << ", allowed range is 1..128."
                  << LogEnd;
        YYABORT;
    }
   ctx.PDPrefix_ = yystack_[0].value.as < uint32_t > ();
}
#line 1522 "SrvParser.cpp"
    break;

  case 215:
#line 1008 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setPrefBeg(yystack_[0].value.as < uint32_t > ());
    ctx.ParserOptStack_.getLast()->setPrefEnd(yystack_[0].value.as < uint32_t > ());
}
#line 1531 "SrvParser.cpp"
    break;

  case 216:
#line 1013 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setPrefBeg(yystack_[2].value.as < uint32_t > ());
    ctx.ParserOptStack_.getLast()->setPrefEnd(yystack_[0].value.as < uint32_t > ());
}
#line 1540 "SrvParser.cpp"
    break;

  case 217:
#line 1021 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setValidBeg(yystack_[0].value.as < uint32_t > ());
    ctx.ParserOptStack_.getLast()->setValidEnd(yystack_[0].value.as < uint32_t > ());
}
#line 1549 "SrvParser.cpp"
    break;

  case 218:
#line 1026 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setValidBeg(yystack_[2].value.as < uint32_t > ());
    ctx.ParserOptStack_.getLast()->setValidEnd(yystack_[0].value.as < uint32_t > ());
}
#line 1558 "SrvParser.cpp"
    break;

  case 219:
#line 1034 "SrvParser.y"
    {
    int x=yystack_[0].value.as < uint32_t > ();
    if ( (x<1) || (x>1000)) {
	error(yystack_[0].location, "Invalid share value: " + to_string(x) + ". Allowed range: 1..1000.");
        YYABORT;
    }
    ctx.ParserOptStack_.getLast()->setShare(x);
}
#line 1571 "SrvParser.cpp"
    break;

  case 220:
#line 1045 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setT1Beg(yystack_[0].value.as < uint32_t > ());
    ctx.ParserOptStack_.getLast()->setT1End(yystack_[0].value.as < uint32_t > ());
}
#line 1580 "SrvParser.cpp"
    break;

  case 221:
#line 1050 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setT1Beg(yystack_[2].value.as < uint32_t > ());
    ctx.ParserOptStack_.getLast()->setT1End(yystack_[0].value.as < uint32_t > ());
}
#line 1589 "SrvParser.cpp"
    break;

  case 222:
#line 1058 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setT2Beg(yystack_[0].value.as < uint32_t > ());
    ctx.ParserOptStack_.getLast()->setT2End(yystack_[0].value.as < uint32_t > ());
}
#line 1598 "SrvParser.cpp"
    break;

  case 223:
#line 1063 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setT2Beg(yystack_[2].value.as < uint32_t > ());
    ctx.ParserOptStack_.getLast()->setT2End(yystack_[0].value.as < uint32_t > ());
}
#line 1607 "SrvParser.cpp"
    break;

  case 224:
#line 1071 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setClntMaxLease(yystack_[0].value.as < uint32_t > ());
}
#line 1615 "SrvParser.cpp"
    break;

  case 225:
#line 1078 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setClassMaxLease(yystack_[0].value.as < uint32_t > ());
}
#line 1623 "SrvParser.cpp"
    break;

  case 226:
#line 1085 "SrvParser.y"
    {
    if (!ctx.ParserOptStack_.getLast()->getExperimental()) {
	Log(Crit) << "Experimental 'addr-params' defined, but experimental "
                  << "features are disabled. Add 'experimental' "
		  << "in global section of server.conf to enable it." << LogEnd;
	YYABORT;
    }
    int bitfield = ADDRPARAMS_MASK_PREFIX;
    Log(Warning) << "Experimental addr-params added (prefix=" << yystack_[0].value.as < uint32_t > ()
                 << ", bitfield=" << bitfield << ")." << LogEnd;
    ctx.ParserOptStack_.getLast()->setAddrParams(yystack_[0].value.as < uint32_t > (),bitfield);
}
#line 1640 "SrvParser.cpp"
    break;

  case 227:
#line 1100 "SrvParser.y"
    {
    SPtr<TOpt> tunnelName = new TOptDomainLst(OPTION_AFTR_NAME, yystack_[0].value.as < std::string > (), 0);
    Log(Debug) << "Enabling DS-Lite tunnel option, AFTR name=" << yystack_[0].value.as < std::string > () << LogEnd;
    ctx.ParserOptStack_.getLast()->addExtraOption(tunnelName, false);
}
#line 1650 "SrvParser.cpp"
    break;

  case 228:
#line 1108 "SrvParser.y"
    {
    SPtr<TOpt> opt = new TOptGeneric(yystack_[2].value.as < uint32_t > (), yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length(), 0);
    ctx.ParserOptStack_.getLast()->addExtraOption(opt, false);
    Log(Debug) << "Extra option defined: code=" << yystack_[2].value.as < uint32_t > () << ", length=" << yystack_[0].value.as < std::string > ().length() << LogEnd;
}
#line 1660 "SrvParser.cpp"
    break;

  case 229:
#line 1114 "SrvParser.y"
    {
    SPtr<TIPv6Addr> addr(new TIPv6Addr(yystack_[0].value.as < std::string > ()));

    SPtr<TOpt> opt = new TOptAddr(yystack_[2].value.as < uint32_t > (), addr, 0);
    ctx.ParserOptStack_.getLast()->addExtraOption(opt, false);
    Log(Debug) << "Extra option defined: code=" << yystack_[2].value.as < uint32_t > () << ", address=" << addr->getPlain() << LogEnd;
}
#line 1672 "SrvParser.cpp"
    break;

  case 230:
#line 1122 "SrvParser.y"
    {
    ctx.PresentAddrLst_.clear();
}
#line 1680 "SrvParser.cpp"
    break;

  case 231:
#line 1125 "SrvParser.y"
    {
    SPtr<TOpt> opt = new TOptAddrLst(yystack_[3].value.as < uint32_t > (), ctx.PresentAddrLst_, 0);
    ctx.ParserOptStack_.getLast()->addExtraOption(opt, false);
    Log(Debug) << "Extra option defined: code=" << yystack_[3].value.as < uint32_t > () << ", address count="
               << ctx.PresentAddrLst_.count() << LogEnd;
}
#line 1691 "SrvParser.cpp"
    break;

  case 232:
#line 1132 "SrvParser.y"
    {
    SPtr<TOpt> opt = new TOptString(yystack_[2].value.as < uint32_t > (), string(yystack_[0].value.as < std::string > ()), 0);
    ctx.ParserOptStack_.getLast()->addExtraOption(opt, false);
    Log(Debug) << "Extra option defined: code=" << yystack_[2].value.as < uint32_t > () << ", string=" << yystack_[0].value.as < std::string > () << LogEnd;
}
#line 1701 "SrvParser.cpp"
    break;

  case 233:
#line 1140 "SrvParser.y"
    {
    if (!ctx.ParserOptStack_.getLast()->getExperimental()) {
	Log(Crit) << "Experimental 'remote autoconf neighbors' defined, but "
		  << "experimental features are disabled. Add 'experimental' "
		  << "in global section of server.conf to enable it." << LogEnd;
	YYABORT;
    }

    ctx.PresentAddrLst_.clear();
}
#line 1716 "SrvParser.cpp"
    break;

  case 234:
#line 1150 "SrvParser.y"
    {
    SPtr<TOpt> opt = new TOptAddrLst(OPTION_NEIGHBORS, ctx.PresentAddrLst_, 0);
    ctx.ParserOptStack_.getLast()->addExtraOption(opt, false);
    Log(Debug) << "Remote autoconf neighbors enabled (" << ctx.PresentAddrLst_.count()
	       << " neighbors defined.)" << LogEnd;
}
#line 1727 "SrvParser.cpp"
    break;

  case 235:
#line 1160 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setIfaceMaxLease(yystack_[0].value.as < uint32_t > ());
}
#line 1735 "SrvParser.cpp"
    break;

  case 236:
#line 1167 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setUnicast(new TIPv6Addr(yystack_[0].value.as < std::string > ()));
}
#line 1743 "SrvParser.cpp"
    break;

  case 237:
#line 1174 "SrvParser.y"
    {
    ctx.CfgMgr_->dropUnicast(true);
}
#line 1751 "SrvParser.cpp"
    break;

  case 238:
#line 1180 "SrvParser.y"
    {
    if ( (yystack_[0].value.as < uint32_t > ()!=0) && (yystack_[0].value.as < uint32_t > ()!=1)) {
	error(yystack_[0].location, "RAPID-COMMIT parameter specified " + to_string(yystack_[0].value.as < uint32_t > ()) + " must have 0 or 1 value.");
        YYABORT;
    }
    ctx.ParserOptStack_.getLast()->setRapidCommit(yystack_[0].value.as < uint32_t > () == 1);
}
#line 1763 "SrvParser.cpp"
    break;

  case 239:
#line 1191 "SrvParser.y"
    {
    int x = yystack_[0].value.as < uint32_t > ();
    if ( (x<0) || (x>255) ) {
	error(yystack_[0].location, "Preference value (" + to_string(x) + " is out of range [0..255].");
	YYABORT;
    }
    ctx.ParserOptStack_.getLast()->setPreference(x);
}
#line 1776 "SrvParser.cpp"
    break;

  case 240:
#line 1202 "SrvParser.y"
    {
    logger::setLogLevel(yystack_[0].value.as < uint32_t > ());
}
#line 1784 "SrvParser.cpp"
    break;

  case 241:
#line 1208 "SrvParser.y"
    {
    logger::setLogMode(yystack_[0].value.as < std::string > ());
}
#line 1792 "SrvParser.cpp"
    break;

  case 242:
#line 1214 "SrvParser.y"
    {
    logger::setLogName(yystack_[0].value.as < std::string > ());
}
#line 1800 "SrvParser.cpp"
    break;

  case 243:
#line 1221 "SrvParser.y"
    {
    logger::setColors(yystack_[0].value.as < uint32_t > ()==1);
}
#line 1808 "SrvParser.cpp"
    break;

  case 244:
#line 1227 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setWorkDir(yystack_[0].value.as < std::string > ());
}
#line 1816 "SrvParser.cpp"
    break;

  case 245:
#line 1234 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setStateless(true);
}
#line 1824 "SrvParser.cpp"
    break;

  case 246:
#line 1241 "SrvParser.y"
    {
    Log(Info) << "Guess-mode enabled: relay interfaces may be loosely "
              << "defined (matching interface-id is not mandatory)." << LogEnd;
    ctx.ParserOptStack_.getLast()->setGuessMode(true);
}
#line 1834 "SrvParser.cpp"
    break;

  case 247:
#line 1249 "SrvParser.y"
    {
    ctx.CfgMgr_->setScriptName(yystack_[0].value.as < std::string > ());
}
#line 1842 "SrvParser.cpp"
    break;

  case 248:
#line 1255 "SrvParser.y"
    {
    if (!ctx.ParserOptStack_.getLast()->getExperimental()) {
	Log(Crit) << "Experimental 'performance-mode' defined, but experimental "
                  << "features are disabled. Add 'experimental' "
		  << "in global section of server.conf to enable it." << LogEnd;
	YYABORT;
    }

    ctx.CfgMgr_->setPerformanceMode(yystack_[0].value.as < uint32_t > ());
}
#line 1857 "SrvParser.cpp"
    break;

  case 249:
#line 1268 "SrvParser.y"
    {
    switch (yystack_[0].value.as < uint32_t > ()) {
    case 0:
    case 1:
        ctx.CfgMgr_->setReconfigureSupport(yystack_[0].value.as < uint32_t > ());
        break;
    default:
        Log(Crit) << "Invalid reconfigure-enabled value " << yystack_[0].value.as < uint32_t > ()
                  << ", only 0 and 1 are supported." << LogEnd;
        YYABORT;
    }
}
#line 1874 "SrvParser.cpp"
    break;

  case 250:
#line 1284 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setInactiveMode(true);
}
#line 1882 "SrvParser.cpp"
    break;

  case 251:
#line 1290 "SrvParser.y"
    {
    Log(Crit) << "Experimental features are allowed." << LogEnd;
    ctx.ParserOptStack_.getLast()->setExperimental(true);
}
#line 1891 "SrvParser.cpp"
    break;

  case 252:
#line 1297 "SrvParser.y"
    {
    if (yystack_[0].value.as < std::string > () == "before")
    {
		ctx.ParserOptStack_.getLast()->setInterfaceIDOrder(SRV_IFACE_ID_ORDER_BEFORE);
    } else
    if (yystack_[0].value.as < std::string > () == "after")
    {
		ctx.ParserOptStack_.getLast()->setInterfaceIDOrder(SRV_IFACE_ID_ORDER_AFTER);
    } else
    if (yystack_[0].value.as < std::string > () == "omit")
    {
		ctx.ParserOptStack_.getLast()->setInterfaceIDOrder(SRV_IFACE_ID_ORDER_NONE);
    } else {
		Log(Crit) << "Invalid interface-id-order specified. Allowed "
                          << "values: before, after, omit" << LogEnd;
		YYABORT;
    }
}
#line 1914 "SrvParser.cpp"
    break;

  case 253:
#line 1318 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setCacheSize(yystack_[0].value.as < uint32_t > ());
}
#line 1922 "SrvParser.cpp"
    break;

  case 254:
#line 1329 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setLeaseQuerySupport(true);

}
#line 1931 "SrvParser.cpp"
    break;

  case 255:
#line 1334 "SrvParser.y"
    {
    switch (yystack_[0].value.as < uint32_t > ()) {
    case 0:
		ctx.ParserOptStack_.getLast()->setLeaseQuerySupport(false);
		break;
    case 1:
		ctx.ParserOptStack_.getLast()->setLeaseQuerySupport(true);
		break;
    default:
		Log(Crit) << "Invalid value of accept-leasequery specifed. Allowed "
                          << "values: 0, 1, yes, no, true, false" << LogEnd;
		YYABORT;
    }
}
#line 1950 "SrvParser.cpp"
    break;

  case 256:
#line 1351 "SrvParser.y"
    {
    if (yystack_[0].value.as < uint32_t > ()!=0 && yystack_[0].value.as < uint32_t > ()!=1) {
	Log(Error) << "Invalid bulk-leasequery-accept value: " << (yystack_[0].value.as < uint32_t > ())
		   << ", 0 or 1 expected." << LogEnd;
	YYABORT;
    }
    ctx.CfgMgr_->bulkLQAccept( (bool) yystack_[0].value.as < uint32_t > ());
}
#line 1963 "SrvParser.cpp"
    break;

  case 257:
#line 1362 "SrvParser.y"
    {
    ctx.CfgMgr_->bulkLQTcpPort( yystack_[0].value.as < uint32_t > () );
}
#line 1971 "SrvParser.cpp"
    break;

  case 258:
#line 1368 "SrvParser.y"
    {
    ctx.CfgMgr_->bulkLQMaxConns( yystack_[0].value.as < uint32_t > () );
}
#line 1979 "SrvParser.cpp"
    break;

  case 259:
#line 1374 "SrvParser.y"
    {
    ctx.CfgMgr_->bulkLQTimeout( yystack_[0].value.as < uint32_t > () );
}
#line 1987 "SrvParser.cpp"
    break;

  case 260:
#line 1383 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setRelayName(yystack_[0].value.as < std::string > ());
}
#line 1995 "SrvParser.cpp"
    break;

  case 261:
#line 1387 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setRelayID(yystack_[0].value.as < uint32_t > ());
}
#line 2003 "SrvParser.cpp"
    break;

  case 262:
#line 1394 "SrvParser.y"
    {
    SPtr<TSrvOptInterfaceID> id = new TSrvOptInterfaceID(yystack_[0].value.as < uint32_t > (), 0);
    ctx.ParserOptStack_.getLast()->setRelayInterfaceID(id);
}
#line 2012 "SrvParser.cpp"
    break;

  case 263:
#line 1399 "SrvParser.y"
    {
    SPtr<TSrvOptInterfaceID> id = new TSrvOptInterfaceID(yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length(), 0);
    ctx.ParserOptStack_.getLast()->setRelayInterfaceID(id);
}
#line 2021 "SrvParser.cpp"
    break;

  case 264:
#line 1404 "SrvParser.y"
    {
    SPtr<TSrvOptInterfaceID> id = new TSrvOptInterfaceID(yystack_[0].value.as < std::string > ().c_str(), yystack_[0].value.as < std::string > ().length(), 0);
    ctx.ParserOptStack_.getLast()->setRelayInterfaceID(id);
}
#line 2030 "SrvParser.cpp"
    break;

  case 265:
#line 1412 "SrvParser.y"
    {
    int prefix = yystack_[0].value.as < uint32_t > ();
    if ( (prefix<1) || (prefix>128) ) {
        error(yystack_[0].location, "Invalid (1..128 allowed) prefix used: "+ to_string(prefix)
              + " in subnet definition.");
        YYABORT;
    }
    SPtr<TIPv6Addr> min = ctx.getRangeMin(yystack_[2].value.as < std::string > ().c_str(), prefix);
    SPtr<TIPv6Addr> max = ctx.getRangeMax(yystack_[2].value.as < std::string > ().c_str(), prefix);
    ctx.SrvCfgIfaceLst_.getLast()->addSubnet(min, max);
    Log(Debug) << "Defined subnet " << min->getPlain() << "/" << yystack_[0].value.as < uint32_t > ()
               << " on " << ctx.SrvCfgIfaceLst_.getLast()->getFullName() << LogEnd;
}
#line 2048 "SrvParser.cpp"
    break;

  case 266:
#line 1425 "SrvParser.y"
    {
    SPtr<TIPv6Addr> min = new TIPv6Addr(yystack_[2].value.as < std::string > ().c_str());
    SPtr<TIPv6Addr> max = new TIPv6Addr(yystack_[0].value.as < std::string > ().c_str());
    ctx.SrvCfgIfaceLst_.getLast()->addSubnet(min, max);
    Log(Debug) << "Defined subnet " << min->getPlain() << "-" << max->getPlain()
               << "on " << ctx.SrvCfgIfaceLst_.getLast()->getFullName() << LogEnd;
}
#line 2060 "SrvParser.cpp"
    break;

  case 279:
#line 1450 "SrvParser.y"
    {
    SPtr<TSrvCfgClientClass> clntClass;
    bool found = false;
    ctx.SrvCfgClientClassLst_.first();
    while ((clntClass = ctx.SrvCfgClientClassLst_.get()))
    {
	if (clntClass->getClassName() == string(yystack_[0].value.as < std::string > ()))
	    found = true;
    }
    if (!found)
    {
	error(yystack_[0].location, ": Unable to use class '" + string(yystack_[0].value.as < std::string > ()) + "', no such class defined.");
	YYABORT;
    }
    ctx.ParserOptStack_.getLast()->setAllowClientClass(string(yystack_[0].value.as < std::string > ()));

    int deny = ctx.ParserOptStack_.getLast()->getDenyClientClassString().count();

    if (deny)
    {
	error(yystack_[1].location, "Unable to define both allow and deny lists for this client class.");
	YYABORT;
    }

}
#line 2090 "SrvParser.cpp"
    break;

  case 280:
#line 1478 "SrvParser.y"
    {
    SPtr<TSrvCfgClientClass> clntClass;
    bool found = false;
    ctx.SrvCfgClientClassLst_.first();
    while ((clntClass = ctx.SrvCfgClientClassLst_.get()))
    {
	if (clntClass->getClassName() == string(yystack_[0].value.as < std::string > ()))
	    found = true;
    }
    if (!found)
    {
	error(yystack_[0].location, " Unable to use class " + string(yystack_[0].value.as < std::string > ()) + ", no such class defined.");
	YYABORT;
    }
    ctx.ParserOptStack_.getLast()->setDenyClientClass(string(yystack_[0].value.as < std::string > ()));

    int allow = ctx.ParserOptStack_.getLast()->getAllowClientClassString().count();

    if (allow)
    {
	error(yystack_[1].location, "Unable to define both allow and deny lists for this client class.");
	YYABORT;
    }

}
#line 2120 "SrvParser.cpp"
    break;

  case 281:
#line 1510 "SrvParser.y"
    {
    ctx.PresentAddrLst_.clear();
}
#line 2128 "SrvParser.cpp"
    break;

  case 282:
#line 1513 "SrvParser.y"
    {
    SPtr<TOpt> nis_servers = new TOptAddrLst(OPTION_DNS_SERVERS, ctx.PresentAddrLst_, NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(nis_servers, false);
}
#line 2137 "SrvParser.cpp"
    break;

  case 283:
#line 1523 "SrvParser.y"
    {
    ctx.PresentStringLst_.clear();
}
#line 2145 "SrvParser.cpp"
    break;

  case 284:
#line 1526 "SrvParser.y"
    {
    SPtr<TOpt> domains = new TOptDomainLst(OPTION_DOMAIN_LIST, ctx.PresentStringLst_, NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(domains, false);
}
#line 2154 "SrvParser.cpp"
    break;

  case 285:
#line 1537 "SrvParser.y"
    {
    ctx.PresentAddrLst_.clear();
}
#line 2162 "SrvParser.cpp"
    break;

  case 286:
#line 1540 "SrvParser.y"
    {
    SPtr<TOpt> ntp_servers = new TOptAddrLst(OPTION_SNTP_SERVERS, ctx.PresentAddrLst_, NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(ntp_servers, false);
    // ctx.ParserOptStack_.getLast()->setNTPServerLst(&ctx.PresentAddrLst);
}
#line 2172 "SrvParser.cpp"
    break;

  case 287:
#line 1552 "SrvParser.y"
    {
    SPtr<TOpt> timezone = new TOptString(OPTION_NEW_TZDB_TIMEZONE, string(yystack_[0].value.as < std::string > ()), NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(timezone, false);
    // ctx.ParserOptStack_.getLast()->setTimezone($3);
}
#line 2182 "SrvParser.cpp"
    break;

  case 288:
#line 1563 "SrvParser.y"
    {
    ctx.PresentAddrLst_.clear();
}
#line 2190 "SrvParser.cpp"
    break;

  case 289:
#line 1566 "SrvParser.y"
    {
    SPtr<TOpt> sip_servers = new TOptAddrLst(OPTION_SIP_SERVER_A, ctx.PresentAddrLst_, NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(sip_servers, false);
    // ctx.ParserOptStack_.getLast()->setSIPServerLst(&ctx.PresentAddrLst);
}
#line 2200 "SrvParser.cpp"
    break;

  case 290:
#line 1577 "SrvParser.y"
    {
    ctx.PresentStringLst_.clear();
}
#line 2208 "SrvParser.cpp"
    break;

  case 291:
#line 1580 "SrvParser.y"
    {
    SPtr<TOpt> sip_domains = new TOptDomainLst(OPTION_SIP_SERVER_D, ctx.PresentStringLst_, NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(sip_domains, false);
    //ctx.ParserOptStack_.getLast()->setSIPDomainLst(&ctx.PresentStringLst_);
}
#line 2218 "SrvParser.cpp"
    break;

  case 292:
#line 1593 "SrvParser.y"
    {
    ctx.PresentFQDNLst_.clear();
    Log(Debug)   << "No FQDNMode found, setting default mode 2 (all updates "
                 "executed by server)." << LogEnd;
    Log(Warning) << "revDNS zoneroot lenght not found, dynamic revDNS update "
                 "will not be possible." << LogEnd;
    ctx.ParserOptStack_.getLast()->setFQDNMode(2);
    ctx.ParserOptStack_.getLast()->setRevDNSZoneRootLength(0);
}
#line 2232 "SrvParser.cpp"
    break;

  case 293:
#line 1602 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setFQDNLst(&ctx.PresentFQDNLst_);
}
#line 2240 "SrvParser.cpp"
    break;

  case 294:
#line 1606 "SrvParser.y"
    {
    ctx.PresentFQDNLst_.clear();
    Log(Debug)  << "FQDN: Setting update mode to " << yystack_[0].value.as < uint32_t > ();
    switch (yystack_[0].value.as < uint32_t > ()) {
    case 0:
	Log(Cont) << "(no updates)" << LogEnd;
	break;
    case 1:
	Log(Cont) << "(client will update AAAA, server will update PTR)" << LogEnd;
	break;
    case 2:
	Log(Cont) << "(server will update both AAAA and PTR)" << LogEnd;
	break;
    default:
	Log(Cont) << LogEnd;
	Log(Crit) << "FQDN: Invalid mode. Only 0-2 are supported." << LogEnd;
        YYABORT;
    }
    Log(Warning)<< "FQDN: RevDNS zoneroot lenght not specified, dynamic revDNS update will not be possible." << LogEnd;
    ctx.ParserOptStack_.getLast()->setFQDNMode(yystack_[0].value.as < uint32_t > ());
    ctx.ParserOptStack_.getLast()->setRevDNSZoneRootLength(0);
}
#line 2267 "SrvParser.cpp"
    break;

  case 295:
#line 1628 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setFQDNLst(&ctx.PresentFQDNLst_);

}
#line 2276 "SrvParser.cpp"
    break;

  case 296:
#line 1633 "SrvParser.y"
    {
    ctx.PresentFQDNLst_.clear();
    Log(Debug) << "FQDN: Setting update mode to " << yystack_[1].value.as < uint32_t > ();
    switch (yystack_[1].value.as < uint32_t > ()) {
    case 0:
	Log(Cont) << "(no updates)" << LogEnd;
	break;
    case 1:
	Log(Cont) << "(client will update AAAA, server will update PTR)" << LogEnd;
	break;
    case 2:
	Log(Cont) << "(server will update both AAAA and PTR)" << LogEnd;
	break;
    default:
	Log(Cont) << LogEnd;
	Log(Crit) << "FQDN: Invalid mode. Only 0-2 are supported." << LogEnd;
        YYABORT;
    }

    Log(Debug) << "FQDN: RevDNS zoneroot lenght set to " << yystack_[0].value.as < uint32_t > () <<LogEnd;
    if ( (yystack_[0].value.as < uint32_t > () < 0) || (yystack_[0].value.as < uint32_t > () > 128) ) {
	Log(Crit) << "FQDN: Invalid zoneroot length specified:" << yystack_[0].value.as < uint32_t > ()
                  << ". Value 0-128 expected." << LogEnd;
	YYABORT;
    }
    ctx.ParserOptStack_.getLast()->setFQDNMode(yystack_[1].value.as < uint32_t > ());
    ctx.ParserOptStack_.getLast()->setRevDNSZoneRootLength(yystack_[0].value.as < uint32_t > ());
}
#line 2309 "SrvParser.cpp"
    break;

  case 297:
#line 1661 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setFQDNLst(&ctx.PresentFQDNLst_);

}
#line 2318 "SrvParser.cpp"
    break;

  case 298:
#line 1669 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setUnknownFQDN(EUnknownFQDNMode(yystack_[1].value.as < uint32_t > ()), string(yystack_[0].value.as < std::string > ()) );
    Log(Debug) << "FQDN: Unknown fqdn names processing set to " << yystack_[1].value.as < uint32_t > ()
               << ", domain=" << yystack_[0].value.as < std::string > () << "." << LogEnd;
}
#line 2328 "SrvParser.cpp"
    break;

  case 299:
#line 1675 "SrvParser.y"
    {
    ctx.ParserOptStack_.getLast()->setUnknownFQDN(EUnknownFQDNMode(yystack_[0].value.as < uint32_t > ()), string("") );
    Log(Debug) << "FQDN: Unknown fqdn names processing set to " << yystack_[0].value.as < uint32_t > ()
               << ", no domain." << LogEnd;
}
#line 2338 "SrvParser.cpp"
    break;

  case 300:
#line 1684 "SrvParser.y"
    {
    ctx.addr_ = new TIPv6Addr(yystack_[0].value.as < std::string > ());
    ctx.CfgMgr_->setDDNSAddress(ctx.addr_);
    Log(Info) << "FQDN: DDNS updates will be performed to " << ctx.addr_->getPlain() << "." << LogEnd;
}
#line 2348 "SrvParser.cpp"
    break;

  case 301:
#line 1692 "SrvParser.y"
    {
    if (yystack_[0].value.as < std::string > () == "tcp")
	ctx.CfgMgr_->setDDNSProtocol(TSrvCfgMgr::DNSUPDATE_TCP);
    else if (yystack_[0].value.as < std::string > () == "udp")
	ctx.CfgMgr_->setDDNSProtocol(TSrvCfgMgr::DNSUPDATE_UDP);
    else if (yystack_[0].value.as < std::string > () == "any")
	ctx.CfgMgr_->setDDNSProtocol(TSrvCfgMgr::DNSUPDATE_ANY);
    else {
        Log(Crit) << "Invalid ddns-protocol specifed:" << (yystack_[0].value.as < std::string > ()) 
                  << ", supported values are tcp, udp, any." << LogEnd;
        YYABORT;
    }
    Log(Debug) << "DDNS: Setting protocol to " << (yystack_[0].value.as < std::string > ()) << LogEnd;
}
#line 2367 "SrvParser.cpp"
    break;

  case 302:
#line 1709 "SrvParser.y"
    {
    Log(Debug) << "DDNS: Setting timeout to " << yystack_[0].value.as < uint32_t > () << "ms." << LogEnd;
    ctx.CfgMgr_->setDDNSTimeout(yystack_[0].value.as < uint32_t > ());
}
#line 2376 "SrvParser.cpp"
    break;

  case 303:
#line 1718 "SrvParser.y"
    {
    ctx.PresentAddrLst_.clear();
}
#line 2384 "SrvParser.cpp"
    break;

  case 304:
#line 1721 "SrvParser.y"
    {
    SPtr<TOpt> nis_servers = new TOptAddrLst(OPTION_NIS_SERVERS, ctx.PresentAddrLst_, NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(nis_servers, false);
    ///ctx.ParserOptStack_.getLast()->setNISServerLst(&ctx.PresentAddrLst);
}
#line 2394 "SrvParser.cpp"
    break;

  case 305:
#line 1732 "SrvParser.y"
    {
    ctx.PresentAddrLst_.clear();
}
#line 2402 "SrvParser.cpp"
    break;

  case 306:
#line 1735 "SrvParser.y"
    {
    SPtr<TOpt> nisp_servers = new TOptAddrLst(OPTION_NISP_SERVERS, ctx.PresentAddrLst_, NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(nisp_servers, false);
    // ctx.ParserOptStack_.getLast()->setNISPServerLst(&ctx.PresentAddrLst);
}
#line 2412 "SrvParser.cpp"
    break;

  case 307:
#line 1747 "SrvParser.y"
    {
    SPtr<TOpt> nis_domain = new TOptDomainLst(OPTION_NIS_DOMAIN_NAME, string(yystack_[0].value.as < std::string > ()), NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(nis_domain, false);
    // ctx.ParserOptStack_.getLast()->setNISDomain($3);
}
#line 2422 "SrvParser.cpp"
    break;

  case 308:
#line 1759 "SrvParser.y"
    {
    SPtr<TOpt> nispdomain = new TOptDomainLst(OPTION_NISP_DOMAIN_NAME, string(yystack_[0].value.as < std::string > ()), NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(nispdomain, false);
}
#line 2431 "SrvParser.cpp"
    break;

  case 309:
#line 1770 "SrvParser.y"
    {
    SPtr<TOpt> lifetime = new TOptInteger(OPTION_INFORMATION_REFRESH_TIME,
                                          OPTION_INFORMATION_REFRESH_TIME_LEN, 
                                          (uint32_t)(yystack_[0].value.as < uint32_t > ()), NULL);
    ctx.ParserOptStack_.getLast()->addExtraOption(lifetime, false);
    //ctx.ParserOptStack_.getLast()->setLifetime($3);
}
#line 2443 "SrvParser.cpp"
    break;

  case 310:
#line 1780 "SrvParser.y"
    {
}
#line 2450 "SrvParser.cpp"
    break;

  case 311:
#line 1782 "SrvParser.y"
    {
    // ctx.ParserOptStack_.getLast()->setVendorSpec(VendorSpec);
    // Log(Debug) << "Vendor-spec parsing finished" << LogEnd;
}
#line 2459 "SrvParser.cpp"
    break;

  case 312:
#line 1790 "SrvParser.y"
    {
    Log(Notice) << "ClientClass found, name: " << string(yystack_[1].value.as < std::string > ()) << LogEnd;
}
#line 2467 "SrvParser.cpp"
    break;

  case 313:
#line 1793 "SrvParser.y"
    {
    SPtr<Node> cond =  ctx.NodeClientClassLst_.getLast();
    ctx.SrvCfgClientClassLst_.append( new TSrvCfgClientClass(string(yystack_[4].value.as < std::string > ()),cond));
    ctx.NodeClientClassLst_.delLast();
}
#line 2477 "SrvParser.cpp"
    break;

  case 314:
#line 1803 "SrvParser.y"
    {
}
#line 2484 "SrvParser.cpp"
    break;

  case 316:
#line 1809 "SrvParser.y"
    {
    SPtr<Node> r =  ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();
    SPtr<Node> l = ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();
    ctx.NodeClientClassLst_.append(new NodeOperator(NodeOperator::OPERATOR_CONTAIN,l,r));
}
#line 2496 "SrvParser.cpp"
    break;

  case 317:
#line 1817 "SrvParser.y"
    {
    SPtr<Node> l =  ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();
    SPtr<Node> r = ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();

    ctx.NodeClientClassLst_.append(new NodeOperator(NodeOperator::OPERATOR_EQUAL,l,r));
}
#line 2509 "SrvParser.cpp"
    break;

  case 318:
#line 1826 "SrvParser.y"
    {
    SPtr<Node> l =  ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();
    SPtr<Node> r = ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();
    ctx.NodeClientClassLst_.append(new NodeOperator(NodeOperator::OPERATOR_AND,l,r));

}
#line 2522 "SrvParser.cpp"
    break;

  case 319:
#line 1835 "SrvParser.y"
    {
    SPtr<Node> l =  ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();
    SPtr<Node> r = ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();
    ctx.NodeClientClassLst_.append(new NodeOperator(NodeOperator::OPERATOR_OR,l,r));
}
#line 2534 "SrvParser.cpp"
    break;

  case 320:
#line 1846 "SrvParser.y"
    {
    ctx.NodeClientClassLst_.append(new NodeClientSpecific(NodeClientSpecific::CLIENT_VENDOR_SPEC_ENTERPRISE_NUM));
}
#line 2542 "SrvParser.cpp"
    break;

  case 321:
#line 1850 "SrvParser.y"
    {
    ctx.NodeClientClassLst_.append(new NodeClientSpecific(NodeClientSpecific::CLIENT_VENDOR_SPEC_DATA));
}
#line 2550 "SrvParser.cpp"
    break;

  case 322:
#line 1854 "SrvParser.y"
    {
    ctx.NodeClientClassLst_.append(new NodeClientSpecific(NodeClientSpecific::CLIENT_VENDOR_CLASS_ENTERPRISE_NUM));
}
#line 2558 "SrvParser.cpp"
    break;

  case 323:
#line 1858 "SrvParser.y"
    {
    ctx.NodeClientClassLst_.append(new NodeClientSpecific(NodeClientSpecific::CLIENT_VENDOR_CLASS_DATA));
}
#line 2566 "SrvParser.cpp"
    break;

  case 324:
#line 1862 "SrvParser.y"
    {
    // Log(Info) << "Constant expression found:" <<string($1)<<LogEnd;
    ctx.NodeClientClassLst_.append(new NodeConstant(string(yystack_[0].value.as < std::string > ())));
}
#line 2575 "SrvParser.cpp"
    break;

  case 325:
#line 1867 "SrvParser.y"
    {
    //Log(Info) << "Constant expression found:" <<string($1)<<LogEnd;
    stringstream convert;
    string snum;
    convert<<yystack_[0].value.as < uint32_t > ();
    convert>>snum;
    ctx.NodeClientClassLst_.append(new NodeConstant(snum));
}
#line 2588 "SrvParser.cpp"
    break;

  case 326:
#line 1876 "SrvParser.y"
    {
    SPtr<Node> l =  ctx.NodeClientClassLst_.getLast();
    ctx.NodeClientClassLst_.delLast();
    ctx.NodeClientClassLst_.append(new NodeOperator(NodeOperator::OPERATOR_SUBSTRING,l, yystack_[3].value.as < uint32_t > (),yystack_[1].value.as < uint32_t > ()));
}
#line 2598 "SrvParser.cpp"
    break;


#line 2602 "SrvParser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  SrvParser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  // Generate an error message.
  std::string
  SrvParser::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (!yyla.empty ())
      {
        int yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const short SrvParser::yypact_ninf_ = -393;

  const signed char SrvParser::yytable_ninf_ = -1;

  const short
  SrvParser::yypact_[] =
  {
     445,   184,   205,    73,   -94,   -64,    92,   -38,    92,   -32,
     287,    92,   -28,   -23,    92,  -393,  -393,  -393,    92,    92,
      92,    92,    92,   -10,  -393,    92,    92,    92,    92,    92,
    -393,    92,    -5,    29,   263,  -393,  -393,  -393,    92,    92,
      35,    37,    52,  -393,    92,    54,    59,    92,    92,    92,
      92,    92,    69,    92,    71,    86,   101,   218,   445,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,   106,  -393,  -393,   108,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,   123,  -393,  -393,  -393,   125,  -393,   135,    92,   104,
    -393,  -393,   155,    64,   169,  -393,  -393,  -393,   115,   115,
     176,  -393,   173,   182,   216,   219,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,   188,    92,   228,  -393,  -393,
    -393,  -393,  -393,   442,  -393,  -393,   231,  -393,  -393,  -393,
    -393,  -393,   253,  -393,  -393,  -393,   144,  -393,  -393,  -393,
    -393,  -393,   230,   243,   230,  -393,   230,   243,   230,  -393,
     230,  -393,  -393,   262,   265,    92,   230,  -393,   269,   264,
     276,  -393,  -393,   271,   274,   267,   267,   203,   273,    92,
      92,    92,    92,   369,   285,   284,   288,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,   297,  -393,  -393,  -393,   291,
      92,   532,   532,  -393,   298,  -393,   302,   298,   298,   302,
     298,   298,  -393,   265,   305,   304,   307,   306,   298,  -393,
    -393,  -393,   230,   316,   317,   172,   319,   318,   322,  -393,
    -393,  -393,  -393,    92,  -393,   321,   369,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,   324,  -393,   442,   282,
     348,  -393,  -393,   325,   326,   329,   331,  -393,   238,  -393,
    -393,  -393,  -393,   327,   332,   338,   265,   304,   215,   343,
      92,    92,   298,  -393,  -393,   336,   345,  -393,  -393,   347,
    -393,   351,  -393,  -393,    77,   352,    77,  -393,   356,   179,
      92,   174,  -393,  -393,  -393,  -393,   340,   355,  -393,  -393,
     357,   376,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,   304,  -393,  -393,   378,   382,   383,   389,   395,   390,
     391,  -393,   581,   414,   415,    30,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,    82,   412,   419,   422,   425,   426,
    -393,  -393,   567,  -393,  -393,   622,   167,  -393,   436,   235,
     117,    92,  -393,  -393,  -393,   437,  -393,   431,  -393,  -393,
      77,  -393,  -393,  -393,  -393,  -393,  -393,   434,  -393,  -393,
    -393,  -393,   432,  -393,  -393,   204,   -25,   590,  -393,   346,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,   448,
     526,  -393,  -393,  -393,  -393,  -393,   438,  -393,    92,   136,
    -393,   299,   340,   340,   299,   299,  -393,  -393,  -393,  -393,
     -33,  -393,    -6,   124,  -393,  -393,   439,   435,   440,   441,
     451,  -393,  -393,  -393,  -393,  -393,  -393,  -393,    92,  -393,
    -393,  -393,  -393,   459,    92,   458,  -393
  };

  const unsigned short
  SrvParser::yydefact_[] =
  {
       3,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   208,   206,   210,     0,     0,
       0,     0,     0,     0,   237,     0,     0,     0,     0,     0,
     245,     0,     0,     0,     0,   246,   250,   251,     0,     0,
       0,     0,     0,   161,     0,     0,     0,   254,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     2,     4,
       8,     5,    29,    65,    63,    16,    17,    18,    19,    20,
      21,   273,   274,   269,   267,   268,   270,   271,   272,    46,
     275,   276,    59,    61,    62,    45,    42,    33,    44,    43,
      10,     9,    11,    12,    13,    14,    27,    30,    31,    32,
      66,    22,    23,    15,    37,    38,    39,    40,    41,    35,
      36,    67,    34,   277,   278,    47,    48,    49,    50,    51,
      52,    53,    54,    24,    25,    26,    55,    57,    56,    58,
      60,    64,    28,     0,   180,   181,     0,   260,   261,   264,
     263,   262,   252,   242,   240,   241,   243,   244,   281,   283,
     285,     0,   288,   290,   303,     0,   305,     0,     0,   292,
     310,   233,     0,     0,   299,   300,   301,   302,     0,     0,
       0,   219,   220,   222,   215,   217,   236,   239,   238,   235,
     225,   224,   253,   137,   247,     0,     0,     0,   226,   248,
     157,   158,   159,     0,   173,   160,     0,   255,   256,   257,
     258,   259,     0,   249,   279,   280,     0,     1,     6,     7,
      68,    70,     0,     0,     0,   287,     0,     0,     0,   307,
       0,   308,   309,   294,     0,     0,     0,   227,     0,     0,
       0,   230,   298,   198,   202,   209,   207,   192,   211,     0,
       0,     0,     0,     0,     0,     0,     0,   165,   166,   167,
     168,   169,   170,   171,   172,   162,   163,    82,   312,     0,
       0,     0,     0,   182,   282,   190,   284,   286,   289,   291,
     304,   306,   296,     0,   174,   293,     0,   311,   234,   228,
     229,   232,     0,     0,     0,     0,     0,     0,     0,   221,
     223,   216,   218,     0,   212,     0,   139,   142,   141,   144,
     143,   145,   146,   147,   148,    95,     0,    99,     0,     0,
       0,   266,   265,     0,     0,     0,     0,    72,     0,    74,
      75,    76,    77,     0,     0,     0,     0,   295,     0,     0,
       0,     0,   231,   199,   203,   200,   204,   193,   194,   195,
     214,     0,   138,   140,     0,     0,     0,   164,     0,     0,
       0,     0,    85,    88,    89,    87,   315,     0,   121,   125,
     151,     0,    69,    73,    79,    78,    80,    81,    71,   183,
     191,   297,   176,   175,   177,     0,     0,     0,     0,     0,
       0,   213,     0,     0,     0,     0,   101,   117,   118,   116,
     115,   103,   104,   105,   106,   107,   108,   109,   111,   110,
     112,   113,   114,    97,     0,     0,     0,     0,     0,     0,
      83,    86,   315,   314,   313,     0,     0,   149,     0,     0,
       0,     0,   201,   205,   196,     0,   119,     0,    96,   102,
       0,   100,    90,    94,    93,    92,    91,     0,   320,   321,
     322,   323,     0,   324,   325,     0,     0,     0,   123,     0,
     127,   133,   134,   131,   129,   130,   132,   135,   136,     0,
     155,   179,   178,   186,   185,   184,     0,   197,     0,     0,
      84,     0,   315,   315,     0,     0,   122,   124,   126,   128,
       0,   152,     0,     0,   120,    98,     0,     0,     0,     0,
       0,   150,   153,   156,   154,   189,   188,   187,     0,   318,
     319,   317,   316,     0,     0,     0,   326
  };

  const short
  SrvParser::yypgoto_[] =
  {
    -393,  -393,  -393,   499,   -55,   502,  -393,  -393,   320,  -393,
    -393,  -393,  -393,   232,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -324,  -353,  -393,  -393,  -228,  -393,  -393,  -226,  -393,
    -393,   132,  -393,  -393,   290,  -393,  -163,  -393,  -393,  -307,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,   279,  -393,  -219,
      -1,     5,  -393,   367,  -393,  -393,   420,  -302,  -393,  -275,
    -393,  -274,  -393,  -393,  -393,  -393,  -240,  -239,  -393,  -185,
    -183,  -393,  -260,  -393,  -332,  -315,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -392,  -237,  -235,  -311,
    -393,  -310,  -393,  -305,  -393,  -304,  -282,  -393,  -281,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -277,  -393,
    -268,  -393,  -261,  -234,  -214,  -213,  -393,  -393,  -393,  -393,
    -344,  -221
  };

  const short
  SrvParser::yydefgoto_[] =
  {
      -1,    57,    58,    59,    60,    61,   261,   262,   318,    62,
     309,   437,   351,   352,   353,   354,   355,    63,   344,   430,
     346,   385,   386,   387,   388,   319,   415,   447,   320,   416,
     449,   450,    64,   243,   295,   296,   321,   459,   480,   322,
      65,    66,    67,    68,    69,   193,   255,   256,    70,   275,
     444,   264,   277,   266,   238,   381,   235,    71,   169,    72,
     168,    73,   170,   297,   341,   298,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,   282,    84,   226,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     212,   116,   213,   117,   214,   118,   119,   216,   120,   217,
     121,   224,   273,   326,   122,   123,   124,   125,   126,   218,
     127,   220,   128,   129,   130,   131,   225,   132,   310,   357,
     413,   446
  };

  const unsigned short
  SrvParser::yytable_[] =
  {
     136,   138,   141,   299,   300,   144,   303,   146,   304,   163,
     164,   367,   389,   167,   389,   142,   367,   171,   172,   173,
     174,   175,   404,   448,   177,   178,   179,   180,   181,   390,
     182,   390,   429,   391,   392,   391,   392,   188,   189,   393,
     394,   393,   394,   194,   382,   143,   197,   198,   199,   200,
     201,   429,   203,   389,   327,   477,   299,   300,   301,   303,
     302,   304,   395,   396,   395,   396,   474,   397,   445,   397,
     390,   145,   389,   316,   391,   392,   398,   147,   398,   475,
     393,   394,   491,   399,   165,   399,   166,   383,   384,   390,
     364,   382,   365,   391,   392,   364,   382,   365,   389,   393,
     394,   493,   176,   395,   396,   494,   469,   371,   397,   183,
     400,   301,   400,   302,   451,   390,   429,   398,   228,   391,
     392,   229,   395,   396,   399,   393,   394,   397,   487,   488,
     401,   402,   401,   402,   383,   384,   398,   389,   184,   383,
     384,   452,   453,   399,   190,   428,   191,   451,   395,   396,
     382,   400,   481,   397,   390,   366,   456,   222,   391,   392,
     366,   192,   398,   195,   393,   394,   230,   231,   196,   399,
     400,   401,   402,   492,   452,   453,   454,   455,   202,   457,
     204,   458,   139,   134,   135,   245,   140,   395,   396,   456,
     401,   402,   397,   383,   384,   205,   400,   431,    15,    16,
      17,   398,   134,   135,    21,    22,   317,   317,   399,   454,
     455,    28,   457,   206,   458,   223,   401,   402,   207,   267,
     210,   268,   211,   270,   276,   271,   463,   233,   234,   464,
     465,   278,   215,   495,   219,   400,   496,   497,   289,   290,
     291,   292,     2,     3,   221,   313,   314,   348,   349,   350,
     486,   485,    10,   489,   490,   401,   402,   406,   407,   312,
     408,   259,   260,   363,   227,    11,    54,    55,   363,    15,
      16,    17,    18,    19,    20,    21,    22,    23,   232,    25,
      26,    27,    28,    29,   335,   336,    32,   332,   237,   410,
     239,    34,   340,   133,   134,   135,   472,   473,    36,   240,
      38,   244,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   137,   134,   135,   185,   186,   187,
     286,   287,    47,    48,    49,    50,    51,   372,   373,   375,
     376,     2,     3,   241,   313,   314,   242,    54,    55,   160,
     246,    10,   263,   315,   316,   257,    56,   461,   462,   409,
     161,   162,   265,   362,    11,   348,   349,   350,    15,    16,
      17,    18,    19,    20,    21,    22,    23,   258,    25,    26,
      27,    28,    29,   272,   274,    32,   280,    15,    16,    17,
      34,   163,   279,    21,    22,   281,   285,    36,   283,    38,
      28,   284,   288,   438,   439,   440,   441,   134,   135,   305,
     442,   306,   307,   311,    19,    20,    21,    22,   443,   134,
     135,    47,    48,    49,    50,    51,   308,   324,   293,   294,
     466,   325,   328,   329,   330,   331,    54,    55,   333,   338,
     334,   337,   315,   316,   339,    56,   342,   345,   356,   358,
     359,   360,   368,   361,   369,    54,    55,   370,     1,     2,
       3,     4,   374,   377,     5,     6,     7,     8,     9,    10,
     412,   478,   378,   380,   379,   405,   403,   484,    54,    55,
     414,   417,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,   418,   419,    33,   503,    34,   420,
     421,   422,   424,   505,    35,    36,    37,    38,   423,   425,
      39,    40,    41,    42,    43,    44,    45,    46,   247,   248,
     249,   250,   251,   252,   253,   254,   426,   427,   432,    47,
      48,    49,    50,    51,    52,   433,     2,     3,   434,   313,
     314,   435,   436,    53,    54,    55,    10,   460,   467,   468,
     470,   482,   471,    56,   316,   483,   499,   208,   498,    11,
     209,   500,   501,    15,    16,    17,    18,    19,    20,    21,
      22,    23,   502,    25,    26,    27,    28,    29,   504,   506,
      32,   479,   323,   411,   269,    34,   343,   347,     0,   236,
       0,     0,    36,     0,    38,     0,   148,   149,   150,   151,
     152,   153,   154,   155,   156,   157,   158,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    47,    48,    49,    50,
      51,    15,    16,    17,    18,    19,    20,    21,    22,     0,
       0,    54,    55,   160,    28,     0,     0,   315,   316,     0,
      56,     0,     0,     0,     0,   162,     0,     0,     0,     0,
       0,     0,    38,    15,    16,    17,    18,    19,    20,    21,
      22,   438,   439,   440,   441,     0,    28,     0,   442,     0,
       0,     0,     0,     0,     0,     0,   443,   134,   135,     0,
       0,     0,     0,     0,    38,     0,     0,   412,     0,    54,
      55,   134,   135,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   476,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    54,    55
  };

  const short
  SrvParser::yycheck_[] =
  {
       1,     2,     3,   243,   243,     6,   243,     8,   243,    10,
      11,   318,   344,    14,   346,   109,   323,    18,    19,    20,
      21,    22,   346,   415,    25,    26,    27,    28,    29,   344,
      31,   346,   385,   344,   344,   346,   346,    38,    39,   344,
     344,   346,   346,    44,    14,   109,    47,    48,    49,    50,
      51,   404,    53,   385,   273,   447,   296,   296,   243,   296,
     243,   296,   344,   344,   346,   346,    91,   344,   412,   346,
     385,   109,   404,   106,   385,   385,   344,   109,   346,   104,
     385,   385,   115,   344,   112,   346,   109,    57,    58,   404,
     318,    14,   318,   404,   404,   323,    14,   323,   430,   404,
     404,   107,   112,   385,   385,   111,   430,   326,   385,   114,
     344,   296,   346,   296,   416,   430,   469,   385,    54,   430,
     430,    57,   404,   404,   385,   430,   430,   404,   472,   473,
     344,   344,   346,   346,    57,    58,   404,   469,   109,    57,
      58,   416,   416,   404,   109,   115,   109,   449,   430,   430,
      14,   385,   459,   430,   469,   318,   416,   158,   469,   469,
     323,   109,   430,   109,   469,   469,   102,   103,   109,   430,
     404,   385,   385,   480,   449,   449,   416,   416,   109,   416,
     109,   416,   109,   110,   111,   186,   113,   469,   469,   449,
     404,   404,   469,    57,    58,   109,   430,   115,    31,    32,
      33,   469,   110,   111,    37,    38,   261,   262,   469,   449,
     449,    44,   449,   112,   449,   111,   430,   430,     0,   214,
     114,   216,   114,   218,   225,   220,   109,   112,   113,   112,
     113,   226,   109,   109,   109,   469,   112,   113,   239,   240,
     241,   242,     4,     5,   109,     7,     8,    73,    74,    75,
     471,   115,    14,   474,   475,   469,   469,    78,    79,   260,
      81,   117,   118,   318,   109,    27,    99,   100,   323,    31,
      32,    33,    34,    35,    36,    37,    38,    39,   109,    41,
      42,    43,    44,    45,   112,   113,    48,   282,   112,   115,
     117,    53,   293,   109,   110,   111,    92,    93,    60,   117,
      62,   113,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,   109,   110,   111,    54,    55,    56,
     117,   118,    84,    85,    86,    87,    88,   112,   113,   330,
     331,     4,     5,   117,     7,     8,   117,    99,   100,    52,
     112,    14,   112,   105,   106,   114,   108,   112,   113,   350,
      63,    64,   109,   115,    27,    73,    74,    75,    31,    32,
      33,    34,    35,    36,    37,    38,    39,   114,    41,    42,
      43,    44,    45,   111,   109,    48,   112,    31,    32,    33,
      53,   382,   113,    37,    38,   109,   119,    60,   117,    62,
      44,   117,   119,    94,    95,    96,    97,   110,   111,   114,
     101,   117,   114,   112,    35,    36,    37,    38,   109,   110,
     111,    84,    85,    86,    87,    88,   119,   119,    49,    50,
     421,   119,   117,   119,   117,   119,    99,   100,   112,   111,
     113,   112,   105,   106,   112,   108,   115,   113,    90,   114,
     114,   112,   115,   112,   112,    99,   100,   109,     3,     4,
       5,     6,   109,   117,     9,    10,    11,    12,    13,    14,
     120,   115,   117,   112,   117,   109,   114,   468,    99,   100,
     115,   114,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,   118,   117,    51,   498,    53,   117,
     117,   112,   112,   504,    59,    60,    61,    62,   113,   118,
      65,    66,    67,    68,    69,    70,    71,    72,    76,    77,
      78,    79,    80,    81,    82,    83,   112,   112,   116,    84,
      85,    86,    87,    88,    89,   116,     4,     5,   116,     7,
       8,   116,   116,    98,    99,   100,    14,   111,   111,   118,
     116,    25,   120,   108,   106,   117,   121,    58,   119,    27,
      58,   121,   121,    31,    32,    33,    34,    35,    36,    37,
      38,    39,   121,    41,    42,    43,    44,    45,   119,   121,
      48,   449,   262,   351,   217,    53,   296,   308,    -1,   169,
      -1,    -1,    60,    -1,    62,    -1,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    85,    86,    87,
      88,    31,    32,    33,    34,    35,    36,    37,    38,    -1,
      -1,    99,   100,    52,    44,    -1,    -1,   105,   106,    -1,
     108,    -1,    -1,    -1,    -1,    64,    -1,    -1,    -1,    -1,
      -1,    -1,    62,    31,    32,    33,    34,    35,    36,    37,
      38,    94,    95,    96,    97,    -1,    44,    -1,   101,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   109,   110,   111,    -1,
      -1,    -1,    -1,    -1,    62,    -1,    -1,   120,    -1,    99,
     100,   110,   111,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   115,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    99,   100
  };

  const unsigned short
  SrvParser::yystos_[] =
  {
       0,     3,     4,     5,     6,     9,    10,    11,    12,    13,
      14,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    51,    53,    59,    60,    61,    62,    65,
      66,    67,    68,    69,    70,    71,    72,    84,    85,    86,
      87,    88,    89,    98,    99,   100,   108,   123,   124,   125,
     126,   127,   131,   139,   154,   162,   163,   164,   165,   166,
     170,   179,   181,   183,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   199,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   233,   235,   237,   238,
     240,   242,   246,   247,   248,   249,   250,   252,   254,   255,
     256,   257,   259,   109,   110,   111,   172,   109,   172,   109,
     113,   172,   109,   109,   172,   109,   172,   109,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      52,    63,    64,   172,   172,   112,   109,   172,   182,   180,
     184,   172,   172,   172,   172,   172,   112,   172,   172,   172,
     172,   172,   172,   114,   109,    54,    55,    56,   172,   172,
     109,   109,   109,   167,   172,   109,   109,   172,   172,   172,
     172,   172,   109,   172,   109,   109,   112,     0,   125,   127,
     114,   114,   232,   234,   236,   109,   239,   241,   251,   109,
     253,   109,   172,   111,   243,   258,   200,   109,    54,    57,
     102,   103,   109,   112,   113,   178,   178,   112,   176,   117,
     117,   117,   117,   155,   113,   172,   112,    76,    77,    78,
      79,    80,    81,    82,    83,   168,   169,   114,   114,   117,
     118,   128,   129,   112,   173,   109,   175,   173,   173,   175,
     173,   173,   111,   244,   109,   171,   172,   174,   173,   113,
     112,   109,   198,   117,   117,   119,   117,   118,   119,   172,
     172,   172,   172,    49,    50,   156,   157,   185,   187,   188,
     189,   191,   192,   229,   230,   114,   117,   114,   119,   132,
     260,   112,   172,     7,     8,   105,   106,   126,   130,   147,
     150,   158,   161,   130,   119,   119,   245,   171,   117,   119,
     117,   119,   173,   112,   113,   112,   113,   112,   111,   112,
     172,   186,   115,   156,   140,   113,   142,   169,    73,    74,
      75,   134,   135,   136,   137,   138,    90,   261,   114,   114,
     112,   112,   115,   126,   147,   150,   158,   161,   115,   112,
     109,   171,   112,   113,   109,   172,   172,   117,   117,   117,
     112,   177,    14,    57,    58,   143,   144,   145,   146,   196,
     197,   231,   233,   235,   237,   238,   240,   250,   252,   254,
     255,   256,   257,   114,   143,   109,    78,    79,    81,   172,
     115,   135,   120,   262,   115,   148,   151,   114,   118,   117,
     117,   117,   112,   113,   112,   118,   112,   112,   115,   144,
     141,   115,   116,   116,   116,   116,   116,   133,    94,    95,
      96,    97,   101,   109,   172,   262,   263,   149,   228,   152,
     153,   179,   181,   183,   188,   189,   194,   229,   230,   159,
     111,   112,   113,   109,   112,   113,   172,   111,   118,   143,
     116,   120,    92,    93,    91,   104,   115,   228,   115,   153,
     160,   161,    25,   117,   172,   115,   263,   262,   262,   263,
     263,   115,   161,   107,   111,   109,   112,   113,   119,   121,
     121,   121,   121,   172,   119,   172,   121
  };

  const unsigned short
  SrvParser::yyr1_[] =
  {
       0,   122,   123,   123,   124,   124,   124,   124,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   126,   126,   126,   126,   126,   126,
     126,   126,   126,   126,   126,   126,   126,   126,   126,   126,
     126,   126,   126,   126,   126,   126,   126,   126,   126,   126,
     126,   126,   126,   126,   126,   126,   126,   126,   128,   127,
     129,   127,   130,   130,   130,   130,   130,   130,   130,   130,
     130,   130,   132,   133,   131,   134,   134,   135,   135,   135,
     136,   137,   138,   138,   138,   140,   139,   141,   139,   142,
     139,   143,   143,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   145,
     146,   148,   147,   149,   149,   151,   150,   152,   152,   153,
     153,   153,   153,   153,   153,   153,   153,   155,   154,   156,
     156,   157,   157,   157,   157,   157,   157,   157,   157,   159,
     158,   158,   160,   160,   161,   161,   161,   162,   163,   164,
     165,   167,   166,   168,   168,   169,   169,   169,   169,   169,
     169,   169,   169,   170,   171,   171,   171,   171,   171,   171,
     172,   172,   173,   173,   174,   174,   174,   174,   174,   174,
     175,   175,   176,   176,   176,   176,   176,   177,   178,   178,
     178,   178,   178,   178,   178,   178,   180,   179,   182,   181,
     184,   183,   186,   185,   187,   188,   188,   189,   189,   190,
     191,   191,   192,   192,   193,   194,   195,   196,   197,   197,
     198,   197,   197,   200,   199,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   220,   221,   222,   223,   224,
     225,   225,   226,   226,   226,   227,   227,   228,   228,   228,
     228,   228,   228,   228,   228,   228,   228,   228,   228,   229,
     230,   232,   231,   234,   233,   236,   235,   237,   239,   238,
     241,   240,   243,   242,   244,   242,   245,   242,   246,   246,
     247,   248,   249,   251,   250,   253,   252,   254,   255,   256,
     258,   257,   260,   259,   261,   262,   262,   262,   262,   262,
     263,   263,   263,   263,   263,   263,   263
  };

  const unsigned char
  SrvParser::yyr2_[] =
  {
       0,     2,     1,     0,     1,     1,     2,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     6,
       0,     6,     1,     2,     1,     1,     1,     1,     2,     2,
       2,     2,     0,     0,     8,     1,     2,     1,     1,     1,
       3,     3,     3,     3,     3,     0,     7,     0,     9,     0,
       7,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       4,     0,     5,     1,     2,     0,     5,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     5,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       6,     2,     1,     2,     6,     4,     6,     2,     2,     2,
       2,     0,     3,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     3,     3,     3,     5,     5,
       1,     1,     1,     3,     5,     5,     5,     7,     7,     7,
       1,     3,     1,     3,     3,     3,     5,     3,     1,     3,
       3,     5,     1,     3,     3,     5,     0,     3,     0,     3,
       0,     3,     0,     3,     2,     2,     4,     2,     4,     2,
       2,     4,     2,     4,     2,     2,     2,     3,     4,     4,
       0,     5,     4,     0,     4,     2,     2,     1,     2,     2,
       2,     2,     2,     2,     2,     1,     1,     2,     2,     2,
       1,     1,     2,     2,     1,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     4,     4,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     0,     4,     0,     4,     0,     4,     3,     0,     4,
       0,     4,     0,     4,     0,     5,     0,     6,     3,     2,
       2,     2,     2,     0,     4,     0,     4,     3,     3,     3,
       0,     4,     0,     6,     2,     0,     5,     5,     5,     5,
       1,     1,     1,     1,     1,     1,     8
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const SrvParser::yytname_[] =
  {
  "$end", "error", "$undefined", "IFACE_", "RELAY_", "IFACE_ID_",
  "IFACE_ID_ORDER_", "CLASS_", "TACLASS_", "LOGNAME_", "LOGLEVEL_",
  "LOGMODE_", "LOGCOLORS_", "WORKDIR_", "OPTION_", "DNS_SERVER_",
  "DOMAIN_", "NTP_SERVER_", "TIME_ZONE_", "SIP_SERVER_", "SIP_DOMAIN_",
  "NIS_SERVER_", "NIS_DOMAIN_", "NISP_SERVER_", "NISP_DOMAIN_",
  "LIFETIME_", "FQDN_", "ACCEPT_UNKNOWN_FQDN_", "FQDN_DDNS_ADDRESS_",
  "DDNS_PROTOCOL_", "DDNS_TIMEOUT_", "ACCEPT_ONLY_", "REJECT_CLIENTS_",
  "POOL_", "SHARE_", "T1_", "T2_", "PREF_TIME_", "VALID_TIME_", "UNICAST_",
  "DROP_UNICAST_", "PREFERENCE_", "RAPID_COMMIT_", "IFACE_MAX_LEASE_",
  "CLASS_MAX_LEASE_", "CLNT_MAX_LEASE_", "STATELESS_", "CACHE_SIZE_",
  "PDCLASS_", "PD_LENGTH_", "PD_POOL_", "SCRIPT_", "VENDOR_SPEC_",
  "CLIENT_", "DUID_KEYWORD_", "REMOTE_ID_", "LINK_LOCAL_", "ADDRESS_",
  "PREFIX_", "GUESS_MODE_", "INACTIVE_MODE_", "EXPERIMENTAL_",
  "ADDR_PARAMS_", "REMOTE_AUTOCONF_NEIGHBORS_", "AFTR_",
  "PERFORMANCE_MODE_", "AUTH_PROTOCOL_", "AUTH_ALGORITHM_", "AUTH_REPLAY_",
  "AUTH_METHODS_", "AUTH_DROP_UNAUTH_", "AUTH_REALM_", "KEY_", "SECRET_",
  "ALGORITHM_", "FUDGE_", "DIGEST_NONE_", "DIGEST_PLAIN_",
  "DIGEST_HMAC_MD5_", "DIGEST_HMAC_SHA1_", "DIGEST_HMAC_SHA224_",
  "DIGEST_HMAC_SHA256_", "DIGEST_HMAC_SHA384_", "DIGEST_HMAC_SHA512_",
  "ACCEPT_LEASEQUERY_", "BULKLQ_ACCEPT_", "BULKLQ_TCPPORT_",
  "BULKLQ_MAX_CONNS_", "BULKLQ_TIMEOUT_", "CLIENT_CLASS_", "MATCH_IF_",
  "EQ_", "AND_", "OR_", "CLIENT_VENDOR_SPEC_ENTERPRISE_NUM_",
  "CLIENT_VENDOR_SPEC_DATA_", "CLIENT_VENDOR_CLASS_EN_",
  "CLIENT_VENDOR_CLASS_DATA_", "RECONFIGURE_ENABLED_", "ALLOW_", "DENY_",
  "SUBSTRING_", "STRING_KEYWORD_", "ADDRESS_LIST_", "CONTAIN_",
  "NEXT_HOP_", "ROUTE_", "INFINITE_", "SUBNET_", "STRING_", "HEXNUMBER_",
  "INTNUMBER_", "IPV6ADDR_", "DUID_", "'{'", "'}'", "';'", "'-'", "'/'",
  "','", "'('", "')'", "$accept", "Grammar", "GlobalDeclarationList",
  "GlobalOption", "InterfaceOptionDeclaration", "InterfaceDeclaration",
  "$@1", "$@2", "InterfaceDeclarationsList", "Key", "$@3", "$@4",
  "KeyOptions", "KeyOption", "KeySecret", "KeyFudge", "KeyAlgorithm",
  "Client", "$@5", "$@6", "$@7", "ClientOptions", "ClientOption",
  "AddressReservation", "PrefixReservation", "ClassDeclaration", "$@8",
  "ClassOptionDeclarationsList", "TAClassDeclaration", "$@9",
  "TAClassOptionsList", "TAClassOption", "PDDeclaration", "$@10",
  "PDOptionsList", "PDOptions", "NextHopDeclaration", "$@11", "RouteList",
  "Route", "AuthProtocol", "AuthAlgorithm", "AuthReplay", "AuthRealm",
  "AuthMethods", "$@12", "DigestList", "Digest", "AuthDropUnauthenticated",
  "FQDNList", "Number", "ADDRESSList", "VendorSpecList", "StringList",
  "ADDRESSRangeList", "PDRangeList", "ADDRESSDUIDRangeList",
  "RejectClientsOption", "$@13", "AcceptOnlyOption", "$@14", "PoolOption",
  "$@15", "PDPoolOption", "$@16", "PDLength", "PreferredTimeOption",
  "ValidTimeOption", "ShareOption", "T1Option", "T2Option",
  "ClntMaxLeaseOption", "ClassMaxLeaseOption", "AddrParams",
  "DsLiteAftrName", "ExtraOption", "$@17", "RemoteAutoconfNeighborsOption",
  "$@18", "IfaceMaxLeaseOption", "UnicastAddressOption", "DropUnicast",
  "RapidCommitOption", "PreferenceOption", "LogLevelOption",
  "LogModeOption", "LogNameOption", "LogColors", "WorkDirOption",
  "StatelessOption", "GuessMode", "ScriptName", "PerformanceMode",
  "ReconfigureEnabled", "InactiveMode", "Experimental", "IfaceIDOrder",
  "CacheSizeOption", "AcceptLeaseQuery", "BulkLeaseQueryAccept",
  "BulkLeaseQueryTcpPort", "BulkLeaseQueryMaxConns",
  "BulkLeaseQueryTimeout", "RelayOption", "InterfaceIDOption", "Subnet",
  "ClassOptionDeclaration", "AllowClientClassDeclaration",
  "DenyClientClassDeclaration", "DNSServerOption", "$@19", "DomainOption",
  "$@20", "NTPServerOption", "$@21", "TimeZoneOption", "SIPServerOption",
  "$@22", "SIPDomainOption", "$@23", "FQDNOption", "$@24", "$@25", "$@26",
  "AcceptUnknownFQDN", "FqdnDdnsAddress", "DdnsProtocol", "DdnsTimeout",
  "NISServerOption", "$@27", "NISPServerOption", "$@28", "NISDomainOption",
  "NISPDomainOption", "LifetimeOption", "VendorSpecOption", "$@29",
  "ClientClass", "$@30", "ClientClassDecleration", "Condition", "Expr", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned short
  SrvParser::yyrline_[] =
  {
       0,   226,   226,   227,   231,   232,   233,   234,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   247,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   267,   268,   269,   270,   271,   272,
     273,   274,   275,   276,   277,   278,   279,   280,   281,   282,
     283,   284,   285,   286,   287,   288,   289,   290,   291,   292,
     293,   294,   295,   296,   297,   298,   299,   300,   306,   305,
     319,   318,   330,   331,   332,   333,   334,   335,   336,   337,
     338,   339,   344,   349,   343,   376,   377,   381,   382,   383,
     387,   394,   400,   401,   402,   408,   407,   422,   421,   436,
     435,   450,   451,   455,   456,   457,   458,   459,   460,   461,
     462,   463,   464,   465,   466,   467,   468,   469,   470,   474,
     482,   492,   491,   504,   505,   511,   510,   522,   523,   527,
     528,   529,   530,   531,   532,   533,   534,   539,   538,   550,
     551,   554,   555,   556,   557,   558,   559,   560,   561,   570,
     569,   580,   589,   590,   594,   603,   612,   623,   646,   652,
     670,   680,   679,   693,   694,   698,   699,   700,   701,   702,
     703,   704,   705,   710,   727,   732,   739,   745,   750,   756,
     765,   766,   770,   774,   781,   788,   796,   804,   810,   818,
     828,   829,   833,   837,   846,   861,   865,   877,   899,   903,
     912,   916,   925,   930,   940,   945,   958,   957,   968,   967,
     978,   977,   988,   987,   995,  1007,  1012,  1020,  1025,  1033,
    1044,  1049,  1057,  1062,  1070,  1077,  1084,  1099,  1107,  1113,
    1122,  1121,  1131,  1140,  1139,  1159,  1166,  1173,  1179,  1190,
    1202,  1208,  1213,  1220,  1226,  1233,  1240,  1248,  1254,  1267,
    1283,  1289,  1296,  1317,  1328,  1333,  1350,  1361,  1367,  1373,
    1382,  1386,  1393,  1398,  1403,  1411,  1424,  1434,  1435,  1436,
    1437,  1438,  1439,  1440,  1441,  1442,  1443,  1444,  1445,  1449,
    1477,  1510,  1509,  1523,  1523,  1537,  1536,  1551,  1563,  1563,
    1577,  1577,  1593,  1592,  1606,  1605,  1633,  1632,  1668,  1674,
    1683,  1691,  1708,  1718,  1718,  1732,  1732,  1746,  1758,  1769,
    1780,  1780,  1790,  1789,  1802,  1808,  1808,  1816,  1825,  1834,
    1845,  1849,  1853,  1857,  1861,  1866,  1875
  };

  // Print the state stack on the debug stream.
  void
  SrvParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << '\n';
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  SrvParser::yy_reduce_print_ (int yyrule)
  {
    unsigned yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


#line 5 "SrvParser.y"
} // dibbler
#line 3458 "SrvParser.cpp"

#line 1882 "SrvParser.y"


void
dibbler::SrvParser::error(const location_type& loc,
                          const std::string& what)
{
    ctx.error(loc, what);
}


