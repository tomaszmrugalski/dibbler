// A Bison parser, made by GNU Bison 3.4.1.

// Skeleton interface for Bison LALR(1) parsers in C++

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


/**
 ** \file SrvParser.h
 ** Define the dibbler::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// Undocumented macros, especially those whose name start with YY_,
// are private implementation details.  Do not rely on them.

#ifndef YY_YY_SRVPARSER_H_INCLUDED
# define YY_YY_SRVPARSER_H_INCLUDED
// //                    "%code requires" blocks.
#line 15 "SrvParser.y"

class SrvParserContext;


#line 53 "SrvParser.h"

# include <cassert>
# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif
# include "location.hh"
#include <typeinfo>
#ifndef YYASSERT
# include <cassert>
# define YYASSERT assert
#endif


#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

#line 5 "SrvParser.y"
namespace dibbler {
#line 169 "SrvParser.h"




  /// A Bison parser.
  class SrvParser
  {
  public:
#ifndef YYSTYPE
  /// A buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current parser state.
  class semantic_type
  {
  public:
    /// Type of *this.
    typedef semantic_type self_type;

    /// Empty construction.
    semantic_type () YY_NOEXCEPT
      : yybuffer_ ()
      , yytypeid_ (YY_NULLPTR)
    {}

    /// Construct and fill.
    template <typename T>
    semantic_type (YY_RVREF (T) t)
      : yytypeid_ (&typeid (T))
    {
      YYASSERT (sizeof (T) <= size);
      new (yyas_<T> ()) T (YY_MOVE (t));
    }

    /// Destruction, allowed only if empty.
    ~semantic_type () YY_NOEXCEPT
    {
      YYASSERT (!yytypeid_);
    }

# if 201103L <= YY_CPLUSPLUS
    /// Instantiate a \a T in here from \a t.
    template <typename T, typename... U>
    T&
    emplace (U&&... u)
    {
      YYASSERT (!yytypeid_);
      YYASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T (std::forward <U>(u)...);
    }
# else
    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    emplace ()
    {
      YYASSERT (!yytypeid_);
      YYASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T ();
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    emplace (const T& t)
    {
      YYASSERT (!yytypeid_);
      YYASSERT (sizeof (T) <= size);
      yytypeid_ = & typeid (T);
      return *new (yyas_<T> ()) T (t);
    }
# endif

    /// Instantiate an empty \a T in here.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build ()
    {
      return emplace<T> ();
    }

    /// Instantiate a \a T in here from \a t.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build (const T& t)
    {
      return emplace<T> (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as () YY_NOEXCEPT
    {
      YYASSERT (yytypeid_);
      YYASSERT (*yytypeid_ == typeid (T));
      YYASSERT (sizeof (T) <= size);
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const YY_NOEXCEPT
    {
      YYASSERT (yytypeid_);
      YYASSERT (*yytypeid_ == typeid (T));
      YYASSERT (sizeof (T) <= size);
      return *yyas_<T> ();
    }

    /// Swap the content with \a that, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsibility.
    /// Swapping between built and (possibly) non-built is done with
    /// self_type::move ().
    template <typename T>
    void
    swap (self_type& that) YY_NOEXCEPT
    {
      YYASSERT (yytypeid_);
      YYASSERT (*yytypeid_ == *that.yytypeid_);
      std::swap (as<T> (), that.as<T> ());
    }

    /// Move the content of \a that to this.
    ///
    /// Destroys \a that.
    template <typename T>
    void
    move (self_type& that)
    {
# if 201103L <= YY_CPLUSPLUS
      emplace<T> (std::move (that.as<T> ()));
# else
      emplace<T> ();
      swap<T> (that);
# endif
      that.destroy<T> ();
    }

# if 201103L <= YY_CPLUSPLUS
    /// Move the content of \a that to this.
    template <typename T>
    void
    move (self_type&& that)
    {
      emplace<T> (std::move (that.as<T> ()));
      that.destroy<T> ();
    }
#endif

    /// Copy the content of \a that to this.
    template <typename T>
    void
    copy (const self_type& that)
    {
      emplace<T> (that.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
      yytypeid_ = YY_NULLPTR;
    }

  private:
    /// Prohibit blind copies.
    self_type& operator= (const self_type&);
    semantic_type (const self_type&);

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ () YY_NOEXCEPT
    {
      void *yyp = yybuffer_.yyraw;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const YY_NOEXCEPT
    {
      const void *yyp = yybuffer_.yyraw;
      return static_cast<const T*> (yyp);
     }

    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // STRING_
      // IPV6ADDR_
      // DUID_
      char dummy1[sizeof (std::string)];

      // HEXNUMBER_
      // INTNUMBER_
      // Number
      char dummy2[sizeof (uint32_t)];
    };

    /// The size of the largest semantic type.
    enum { size = sizeof (union_type) };

    /// A buffer to store semantic values.
    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me;
      /// A buffer large enough to store any of the semantic values.
      char yyraw[size];
    } yybuffer_;

    /// Whether the content is built: if defined, the name of the stored type.
    const std::type_info *yytypeid_;
  };

#else
    typedef YYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m)
        : std::runtime_error (m)
        , location (l)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
        , location (s.location)
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;

      location_type location;
    };

    /// Tokens.
    struct token
    {
      enum yytokentype
      {
        TOKEN_IFACE_ = 258,
        TOKEN_RELAY_ = 259,
        TOKEN_IFACE_ID_ = 260,
        TOKEN_IFACE_ID_ORDER_ = 261,
        TOKEN_CLASS_ = 262,
        TOKEN_TACLASS_ = 263,
        TOKEN_LOGNAME_ = 264,
        TOKEN_LOGLEVEL_ = 265,
        TOKEN_LOGMODE_ = 266,
        TOKEN_LOGCOLORS_ = 267,
        TOKEN_WORKDIR_ = 268,
        TOKEN_OPTION_ = 269,
        TOKEN_DNS_SERVER_ = 270,
        TOKEN_DOMAIN_ = 271,
        TOKEN_NTP_SERVER_ = 272,
        TOKEN_TIME_ZONE_ = 273,
        TOKEN_SIP_SERVER_ = 274,
        TOKEN_SIP_DOMAIN_ = 275,
        TOKEN_NIS_SERVER_ = 276,
        TOKEN_NIS_DOMAIN_ = 277,
        TOKEN_NISP_SERVER_ = 278,
        TOKEN_NISP_DOMAIN_ = 279,
        TOKEN_LIFETIME_ = 280,
        TOKEN_FQDN_ = 281,
        TOKEN_ACCEPT_UNKNOWN_FQDN_ = 282,
        TOKEN_FQDN_DDNS_ADDRESS_ = 283,
        TOKEN_DDNS_PROTOCOL_ = 284,
        TOKEN_DDNS_TIMEOUT_ = 285,
        TOKEN_ACCEPT_ONLY_ = 286,
        TOKEN_REJECT_CLIENTS_ = 287,
        TOKEN_POOL_ = 288,
        TOKEN_SHARE_ = 289,
        TOKEN_T1_ = 290,
        TOKEN_T2_ = 291,
        TOKEN_PREF_TIME_ = 292,
        TOKEN_VALID_TIME_ = 293,
        TOKEN_UNICAST_ = 294,
        TOKEN_DROP_UNICAST_ = 295,
        TOKEN_PREFERENCE_ = 296,
        TOKEN_RAPID_COMMIT_ = 297,
        TOKEN_IFACE_MAX_LEASE_ = 298,
        TOKEN_CLASS_MAX_LEASE_ = 299,
        TOKEN_CLNT_MAX_LEASE_ = 300,
        TOKEN_STATELESS_ = 301,
        TOKEN_CACHE_SIZE_ = 302,
        TOKEN_PDCLASS_ = 303,
        TOKEN_PD_LENGTH_ = 304,
        TOKEN_PD_POOL_ = 305,
        TOKEN_SCRIPT_ = 306,
        TOKEN_VENDOR_SPEC_ = 307,
        TOKEN_CLIENT_ = 308,
        TOKEN_DUID_KEYWORD_ = 309,
        TOKEN_REMOTE_ID_ = 310,
        TOKEN_LINK_LOCAL_ = 311,
        TOKEN_ADDRESS_ = 312,
        TOKEN_PREFIX_ = 313,
        TOKEN_GUESS_MODE_ = 314,
        TOKEN_INACTIVE_MODE_ = 315,
        TOKEN_EXPERIMENTAL_ = 316,
        TOKEN_ADDR_PARAMS_ = 317,
        TOKEN_REMOTE_AUTOCONF_NEIGHBORS_ = 318,
        TOKEN_AFTR_ = 319,
        TOKEN_PERFORMANCE_MODE_ = 320,
        TOKEN_AUTH_PROTOCOL_ = 321,
        TOKEN_AUTH_ALGORITHM_ = 322,
        TOKEN_AUTH_REPLAY_ = 323,
        TOKEN_AUTH_METHODS_ = 324,
        TOKEN_AUTH_DROP_UNAUTH_ = 325,
        TOKEN_AUTH_REALM_ = 326,
        TOKEN_KEY_ = 327,
        TOKEN_SECRET_ = 328,
        TOKEN_ALGORITHM_ = 329,
        TOKEN_FUDGE_ = 330,
        TOKEN_DIGEST_NONE_ = 331,
        TOKEN_DIGEST_PLAIN_ = 332,
        TOKEN_DIGEST_HMAC_MD5_ = 333,
        TOKEN_DIGEST_HMAC_SHA1_ = 334,
        TOKEN_DIGEST_HMAC_SHA224_ = 335,
        TOKEN_DIGEST_HMAC_SHA256_ = 336,
        TOKEN_DIGEST_HMAC_SHA384_ = 337,
        TOKEN_DIGEST_HMAC_SHA512_ = 338,
        TOKEN_ACCEPT_LEASEQUERY_ = 339,
        TOKEN_BULKLQ_ACCEPT_ = 340,
        TOKEN_BULKLQ_TCPPORT_ = 341,
        TOKEN_BULKLQ_MAX_CONNS_ = 342,
        TOKEN_BULKLQ_TIMEOUT_ = 343,
        TOKEN_CLIENT_CLASS_ = 344,
        TOKEN_MATCH_IF_ = 345,
        TOKEN_EQ_ = 346,
        TOKEN_AND_ = 347,
        TOKEN_OR_ = 348,
        TOKEN_CLIENT_VENDOR_SPEC_ENTERPRISE_NUM_ = 349,
        TOKEN_CLIENT_VENDOR_SPEC_DATA_ = 350,
        TOKEN_CLIENT_VENDOR_CLASS_EN_ = 351,
        TOKEN_CLIENT_VENDOR_CLASS_DATA_ = 352,
        TOKEN_RECONFIGURE_ENABLED_ = 353,
        TOKEN_ALLOW_ = 354,
        TOKEN_DENY_ = 355,
        TOKEN_SUBSTRING_ = 356,
        TOKEN_STRING_KEYWORD_ = 357,
        TOKEN_ADDRESS_LIST_ = 358,
        TOKEN_CONTAIN_ = 359,
        TOKEN_NEXT_HOP_ = 360,
        TOKEN_ROUTE_ = 361,
        TOKEN_INFINITE_ = 362,
        TOKEN_SUBNET_ = 363,
        TOKEN_STRING_ = 364,
        TOKEN_HEXNUMBER_ = 365,
        TOKEN_INTNUMBER_ = 366,
        TOKEN_IPV6ADDR_ = 367,
        TOKEN_DUID_ = 368
      };
    };

    /// (External) token type, as returned by yylex.
    typedef token::yytokentype token_type;

    /// Symbol type: an internal symbol number.
    typedef int symbol_number_type;

    /// The symbol type number to denote an empty symbol.
    enum { empty_symbol = -2 };

    /// Internal symbol number for tokens (subsumed by symbol_number_type).
    typedef unsigned char token_number_type;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol type
    /// via type_get ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol ()
        : value ()
        , location ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that);
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, location_type&& l)
        : Base (t)
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const location_type& l)
        : Base (t)
        , location (l)
      {}
#endif
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::string&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::string& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, uint32_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const uint32_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }

      /// Destroy contents, and record that is empty.
      void clear ()
      {
        // User destructor.
        symbol_number_type yytype = this->type_get ();
        basic_symbol<Base>& yysym = *this;
        (void) yysym;
        switch (yytype)
        {
       default:
          break;
        }

        // Type destructor.
switch (yytype)
    {
      case 109: // STRING_
      case 112: // IPV6ADDR_
      case 113: // DUID_
        value.template destroy< std::string > ();
        break;

      case 110: // HEXNUMBER_
      case 111: // INTNUMBER_
      case 172: // Number
        value.template destroy< uint32_t > ();
        break;

      default:
        break;
    }

        Base::clear ();
      }

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      semantic_type value;

      /// The location.
      location_type location;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_type
    {
      /// Default constructor.
      by_type ();

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_type (by_type&& that);
#endif

      /// Copy constructor.
      by_type (const by_type& that);

      /// The symbol type as needed by the constructor.
      typedef token_type kind_type;

      /// Constructor from (external) token numbers.
      by_type (kind_type t);

      /// Record that this symbol is empty.
      void clear ();

      /// Steal the symbol type from \a that.
      void move (by_type& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_number_type type_get () const YY_NOEXCEPT;

      /// The token.
      token_type token () const YY_NOEXCEPT;

      /// The symbol type.
      /// \a empty_symbol when empty.
      /// An int, not token_number_type, to be able to store empty_symbol.
      int type;
    };

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_type>
    {
      /// Superclass.
      typedef basic_symbol<by_type> super_type;

      /// Empty symbol.
      symbol_type () {}

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, location_type l)
        : super_type(token_type (tok), std::move (l))
      {
        YYASSERT (tok == 0 || tok == token::TOKEN_IFACE_ || tok == token::TOKEN_RELAY_ || tok == token::TOKEN_IFACE_ID_ || tok == token::TOKEN_IFACE_ID_ORDER_ || tok == token::TOKEN_CLASS_ || tok == token::TOKEN_TACLASS_ || tok == token::TOKEN_LOGNAME_ || tok == token::TOKEN_LOGLEVEL_ || tok == token::TOKEN_LOGMODE_ || tok == token::TOKEN_LOGCOLORS_ || tok == token::TOKEN_WORKDIR_ || tok == token::TOKEN_OPTION_ || tok == token::TOKEN_DNS_SERVER_ || tok == token::TOKEN_DOMAIN_ || tok == token::TOKEN_NTP_SERVER_ || tok == token::TOKEN_TIME_ZONE_ || tok == token::TOKEN_SIP_SERVER_ || tok == token::TOKEN_SIP_DOMAIN_ || tok == token::TOKEN_NIS_SERVER_ || tok == token::TOKEN_NIS_DOMAIN_ || tok == token::TOKEN_NISP_SERVER_ || tok == token::TOKEN_NISP_DOMAIN_ || tok == token::TOKEN_LIFETIME_ || tok == token::TOKEN_FQDN_ || tok == token::TOKEN_ACCEPT_UNKNOWN_FQDN_ || tok == token::TOKEN_FQDN_DDNS_ADDRESS_ || tok == token::TOKEN_DDNS_PROTOCOL_ || tok == token::TOKEN_DDNS_TIMEOUT_ || tok == token::TOKEN_ACCEPT_ONLY_ || tok == token::TOKEN_REJECT_CLIENTS_ || tok == token::TOKEN_POOL_ || tok == token::TOKEN_SHARE_ || tok == token::TOKEN_T1_ || tok == token::TOKEN_T2_ || tok == token::TOKEN_PREF_TIME_ || tok == token::TOKEN_VALID_TIME_ || tok == token::TOKEN_UNICAST_ || tok == token::TOKEN_DROP_UNICAST_ || tok == token::TOKEN_PREFERENCE_ || tok == token::TOKEN_RAPID_COMMIT_ || tok == token::TOKEN_IFACE_MAX_LEASE_ || tok == token::TOKEN_CLASS_MAX_LEASE_ || tok == token::TOKEN_CLNT_MAX_LEASE_ || tok == token::TOKEN_STATELESS_ || tok == token::TOKEN_CACHE_SIZE_ || tok == token::TOKEN_PDCLASS_ || tok == token::TOKEN_PD_LENGTH_ || tok == token::TOKEN_PD_POOL_ || tok == token::TOKEN_SCRIPT_ || tok == token::TOKEN_VENDOR_SPEC_ || tok == token::TOKEN_CLIENT_ || tok == token::TOKEN_DUID_KEYWORD_ || tok == token::TOKEN_REMOTE_ID_ || tok == token::TOKEN_LINK_LOCAL_ || tok == token::TOKEN_ADDRESS_ || tok == token::TOKEN_PREFIX_ || tok == token::TOKEN_GUESS_MODE_ || tok == token::TOKEN_INACTIVE_MODE_ || tok == token::TOKEN_EXPERIMENTAL_ || tok == token::TOKEN_ADDR_PARAMS_ || tok == token::TOKEN_REMOTE_AUTOCONF_NEIGHBORS_ || tok == token::TOKEN_AFTR_ || tok == token::TOKEN_PERFORMANCE_MODE_ || tok == token::TOKEN_AUTH_PROTOCOL_ || tok == token::TOKEN_AUTH_ALGORITHM_ || tok == token::TOKEN_AUTH_REPLAY_ || tok == token::TOKEN_AUTH_METHODS_ || tok == token::TOKEN_AUTH_DROP_UNAUTH_ || tok == token::TOKEN_AUTH_REALM_ || tok == token::TOKEN_KEY_ || tok == token::TOKEN_SECRET_ || tok == token::TOKEN_ALGORITHM_ || tok == token::TOKEN_FUDGE_ || tok == token::TOKEN_DIGEST_NONE_ || tok == token::TOKEN_DIGEST_PLAIN_ || tok == token::TOKEN_DIGEST_HMAC_MD5_ || tok == token::TOKEN_DIGEST_HMAC_SHA1_ || tok == token::TOKEN_DIGEST_HMAC_SHA224_ || tok == token::TOKEN_DIGEST_HMAC_SHA256_ || tok == token::TOKEN_DIGEST_HMAC_SHA384_ || tok == token::TOKEN_DIGEST_HMAC_SHA512_ || tok == token::TOKEN_ACCEPT_LEASEQUERY_ || tok == token::TOKEN_BULKLQ_ACCEPT_ || tok == token::TOKEN_BULKLQ_TCPPORT_ || tok == token::TOKEN_BULKLQ_MAX_CONNS_ || tok == token::TOKEN_BULKLQ_TIMEOUT_ || tok == token::TOKEN_CLIENT_CLASS_ || tok == token::TOKEN_MATCH_IF_ || tok == token::TOKEN_EQ_ || tok == token::TOKEN_AND_ || tok == token::TOKEN_OR_ || tok == token::TOKEN_CLIENT_VENDOR_SPEC_ENTERPRISE_NUM_ || tok == token::TOKEN_CLIENT_VENDOR_SPEC_DATA_ || tok == token::TOKEN_CLIENT_VENDOR_CLASS_EN_ || tok == token::TOKEN_CLIENT_VENDOR_CLASS_DATA_ || tok == token::TOKEN_RECONFIGURE_ENABLED_ || tok == token::TOKEN_ALLOW_ || tok == token::TOKEN_DENY_ || tok == token::TOKEN_SUBSTRING_ || tok == token::TOKEN_STRING_KEYWORD_ || tok == token::TOKEN_ADDRESS_LIST_ || tok == token::TOKEN_CONTAIN_ || tok == token::TOKEN_NEXT_HOP_ || tok == token::TOKEN_ROUTE_ || tok == token::TOKEN_INFINITE_ || tok == token::TOKEN_SUBNET_ || tok == 123 || tok == 125 || tok == 59 || tok == 45 || tok == 47 || tok == 44 || tok == 40 || tok == 41);
      }
#else
      symbol_type (int tok, const location_type& l)
        : super_type(token_type (tok), l)
      {
        YYASSERT (tok == 0 || tok == token::TOKEN_IFACE_ || tok == token::TOKEN_RELAY_ || tok == token::TOKEN_IFACE_ID_ || tok == token::TOKEN_IFACE_ID_ORDER_ || tok == token::TOKEN_CLASS_ || tok == token::TOKEN_TACLASS_ || tok == token::TOKEN_LOGNAME_ || tok == token::TOKEN_LOGLEVEL_ || tok == token::TOKEN_LOGMODE_ || tok == token::TOKEN_LOGCOLORS_ || tok == token::TOKEN_WORKDIR_ || tok == token::TOKEN_OPTION_ || tok == token::TOKEN_DNS_SERVER_ || tok == token::TOKEN_DOMAIN_ || tok == token::TOKEN_NTP_SERVER_ || tok == token::TOKEN_TIME_ZONE_ || tok == token::TOKEN_SIP_SERVER_ || tok == token::TOKEN_SIP_DOMAIN_ || tok == token::TOKEN_NIS_SERVER_ || tok == token::TOKEN_NIS_DOMAIN_ || tok == token::TOKEN_NISP_SERVER_ || tok == token::TOKEN_NISP_DOMAIN_ || tok == token::TOKEN_LIFETIME_ || tok == token::TOKEN_FQDN_ || tok == token::TOKEN_ACCEPT_UNKNOWN_FQDN_ || tok == token::TOKEN_FQDN_DDNS_ADDRESS_ || tok == token::TOKEN_DDNS_PROTOCOL_ || tok == token::TOKEN_DDNS_TIMEOUT_ || tok == token::TOKEN_ACCEPT_ONLY_ || tok == token::TOKEN_REJECT_CLIENTS_ || tok == token::TOKEN_POOL_ || tok == token::TOKEN_SHARE_ || tok == token::TOKEN_T1_ || tok == token::TOKEN_T2_ || tok == token::TOKEN_PREF_TIME_ || tok == token::TOKEN_VALID_TIME_ || tok == token::TOKEN_UNICAST_ || tok == token::TOKEN_DROP_UNICAST_ || tok == token::TOKEN_PREFERENCE_ || tok == token::TOKEN_RAPID_COMMIT_ || tok == token::TOKEN_IFACE_MAX_LEASE_ || tok == token::TOKEN_CLASS_MAX_LEASE_ || tok == token::TOKEN_CLNT_MAX_LEASE_ || tok == token::TOKEN_STATELESS_ || tok == token::TOKEN_CACHE_SIZE_ || tok == token::TOKEN_PDCLASS_ || tok == token::TOKEN_PD_LENGTH_ || tok == token::TOKEN_PD_POOL_ || tok == token::TOKEN_SCRIPT_ || tok == token::TOKEN_VENDOR_SPEC_ || tok == token::TOKEN_CLIENT_ || tok == token::TOKEN_DUID_KEYWORD_ || tok == token::TOKEN_REMOTE_ID_ || tok == token::TOKEN_LINK_LOCAL_ || tok == token::TOKEN_ADDRESS_ || tok == token::TOKEN_PREFIX_ || tok == token::TOKEN_GUESS_MODE_ || tok == token::TOKEN_INACTIVE_MODE_ || tok == token::TOKEN_EXPERIMENTAL_ || tok == token::TOKEN_ADDR_PARAMS_ || tok == token::TOKEN_REMOTE_AUTOCONF_NEIGHBORS_ || tok == token::TOKEN_AFTR_ || tok == token::TOKEN_PERFORMANCE_MODE_ || tok == token::TOKEN_AUTH_PROTOCOL_ || tok == token::TOKEN_AUTH_ALGORITHM_ || tok == token::TOKEN_AUTH_REPLAY_ || tok == token::TOKEN_AUTH_METHODS_ || tok == token::TOKEN_AUTH_DROP_UNAUTH_ || tok == token::TOKEN_AUTH_REALM_ || tok == token::TOKEN_KEY_ || tok == token::TOKEN_SECRET_ || tok == token::TOKEN_ALGORITHM_ || tok == token::TOKEN_FUDGE_ || tok == token::TOKEN_DIGEST_NONE_ || tok == token::TOKEN_DIGEST_PLAIN_ || tok == token::TOKEN_DIGEST_HMAC_MD5_ || tok == token::TOKEN_DIGEST_HMAC_SHA1_ || tok == token::TOKEN_DIGEST_HMAC_SHA224_ || tok == token::TOKEN_DIGEST_HMAC_SHA256_ || tok == token::TOKEN_DIGEST_HMAC_SHA384_ || tok == token::TOKEN_DIGEST_HMAC_SHA512_ || tok == token::TOKEN_ACCEPT_LEASEQUERY_ || tok == token::TOKEN_BULKLQ_ACCEPT_ || tok == token::TOKEN_BULKLQ_TCPPORT_ || tok == token::TOKEN_BULKLQ_MAX_CONNS_ || tok == token::TOKEN_BULKLQ_TIMEOUT_ || tok == token::TOKEN_CLIENT_CLASS_ || tok == token::TOKEN_MATCH_IF_ || tok == token::TOKEN_EQ_ || tok == token::TOKEN_AND_ || tok == token::TOKEN_OR_ || tok == token::TOKEN_CLIENT_VENDOR_SPEC_ENTERPRISE_NUM_ || tok == token::TOKEN_CLIENT_VENDOR_SPEC_DATA_ || tok == token::TOKEN_CLIENT_VENDOR_CLASS_EN_ || tok == token::TOKEN_CLIENT_VENDOR_CLASS_DATA_ || tok == token::TOKEN_RECONFIGURE_ENABLED_ || tok == token::TOKEN_ALLOW_ || tok == token::TOKEN_DENY_ || tok == token::TOKEN_SUBSTRING_ || tok == token::TOKEN_STRING_KEYWORD_ || tok == token::TOKEN_ADDRESS_LIST_ || tok == token::TOKEN_CONTAIN_ || tok == token::TOKEN_NEXT_HOP_ || tok == token::TOKEN_ROUTE_ || tok == token::TOKEN_INFINITE_ || tok == token::TOKEN_SUBNET_ || tok == 123 || tok == 125 || tok == 59 || tok == 45 || tok == 47 || tok == 44 || tok == 40 || tok == 41);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, std::string v, location_type l)
        : super_type(token_type (tok), std::move (v), std::move (l))
      {
        YYASSERT (tok == token::TOKEN_STRING_ || tok == token::TOKEN_IPV6ADDR_ || tok == token::TOKEN_DUID_);
      }
#else
      symbol_type (int tok, const std::string& v, const location_type& l)
        : super_type(token_type (tok), v, l)
      {
        YYASSERT (tok == token::TOKEN_STRING_ || tok == token::TOKEN_IPV6ADDR_ || tok == token::TOKEN_DUID_);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, uint32_t v, location_type l)
        : super_type(token_type (tok), std::move (v), std::move (l))
      {
        YYASSERT (tok == token::TOKEN_HEXNUMBER_ || tok == token::TOKEN_INTNUMBER_);
      }
#else
      symbol_type (int tok, const uint32_t& v, const location_type& l)
        : super_type(token_type (tok), v, l)
      {
        YYASSERT (tok == token::TOKEN_HEXNUMBER_ || tok == token::TOKEN_INTNUMBER_);
      }
#endif
    };

    /// Build a parser object.
    SrvParser (SrvParserContext& ctx_yyarg);
    virtual ~SrvParser ();

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

    // Implementation of make_symbol for each symbol type.
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IFACE_ (location_type l)
      {
        return symbol_type (token::TOKEN_IFACE_, std::move (l));
      }
#else
      static
      symbol_type
      make_IFACE_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_IFACE_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RELAY_ (location_type l)
      {
        return symbol_type (token::TOKEN_RELAY_, std::move (l));
      }
#else
      static
      symbol_type
      make_RELAY_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_RELAY_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IFACE_ID_ (location_type l)
      {
        return symbol_type (token::TOKEN_IFACE_ID_, std::move (l));
      }
#else
      static
      symbol_type
      make_IFACE_ID_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_IFACE_ID_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IFACE_ID_ORDER_ (location_type l)
      {
        return symbol_type (token::TOKEN_IFACE_ID_ORDER_, std::move (l));
      }
#else
      static
      symbol_type
      make_IFACE_ID_ORDER_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_IFACE_ID_ORDER_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLASS_ (location_type l)
      {
        return symbol_type (token::TOKEN_CLASS_, std::move (l));
      }
#else
      static
      symbol_type
      make_CLASS_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_CLASS_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TACLASS_ (location_type l)
      {
        return symbol_type (token::TOKEN_TACLASS_, std::move (l));
      }
#else
      static
      symbol_type
      make_TACLASS_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_TACLASS_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LOGNAME_ (location_type l)
      {
        return symbol_type (token::TOKEN_LOGNAME_, std::move (l));
      }
#else
      static
      symbol_type
      make_LOGNAME_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_LOGNAME_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LOGLEVEL_ (location_type l)
      {
        return symbol_type (token::TOKEN_LOGLEVEL_, std::move (l));
      }
#else
      static
      symbol_type
      make_LOGLEVEL_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_LOGLEVEL_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LOGMODE_ (location_type l)
      {
        return symbol_type (token::TOKEN_LOGMODE_, std::move (l));
      }
#else
      static
      symbol_type
      make_LOGMODE_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_LOGMODE_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LOGCOLORS_ (location_type l)
      {
        return symbol_type (token::TOKEN_LOGCOLORS_, std::move (l));
      }
#else
      static
      symbol_type
      make_LOGCOLORS_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_LOGCOLORS_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_WORKDIR_ (location_type l)
      {
        return symbol_type (token::TOKEN_WORKDIR_, std::move (l));
      }
#else
      static
      symbol_type
      make_WORKDIR_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_WORKDIR_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OPTION_ (location_type l)
      {
        return symbol_type (token::TOKEN_OPTION_, std::move (l));
      }
#else
      static
      symbol_type
      make_OPTION_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_OPTION_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DNS_SERVER_ (location_type l)
      {
        return symbol_type (token::TOKEN_DNS_SERVER_, std::move (l));
      }
#else
      static
      symbol_type
      make_DNS_SERVER_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_DNS_SERVER_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DOMAIN_ (location_type l)
      {
        return symbol_type (token::TOKEN_DOMAIN_, std::move (l));
      }
#else
      static
      symbol_type
      make_DOMAIN_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_DOMAIN_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NTP_SERVER_ (location_type l)
      {
        return symbol_type (token::TOKEN_NTP_SERVER_, std::move (l));
      }
#else
      static
      symbol_type
      make_NTP_SERVER_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_NTP_SERVER_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TIME_ZONE_ (location_type l)
      {
        return symbol_type (token::TOKEN_TIME_ZONE_, std::move (l));
      }
#else
      static
      symbol_type
      make_TIME_ZONE_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_TIME_ZONE_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SIP_SERVER_ (location_type l)
      {
        return symbol_type (token::TOKEN_SIP_SERVER_, std::move (l));
      }
#else
      static
      symbol_type
      make_SIP_SERVER_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_SIP_SERVER_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SIP_DOMAIN_ (location_type l)
      {
        return symbol_type (token::TOKEN_SIP_DOMAIN_, std::move (l));
      }
#else
      static
      symbol_type
      make_SIP_DOMAIN_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_SIP_DOMAIN_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NIS_SERVER_ (location_type l)
      {
        return symbol_type (token::TOKEN_NIS_SERVER_, std::move (l));
      }
#else
      static
      symbol_type
      make_NIS_SERVER_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_NIS_SERVER_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NIS_DOMAIN_ (location_type l)
      {
        return symbol_type (token::TOKEN_NIS_DOMAIN_, std::move (l));
      }
#else
      static
      symbol_type
      make_NIS_DOMAIN_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_NIS_DOMAIN_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NISP_SERVER_ (location_type l)
      {
        return symbol_type (token::TOKEN_NISP_SERVER_, std::move (l));
      }
#else
      static
      symbol_type
      make_NISP_SERVER_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_NISP_SERVER_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NISP_DOMAIN_ (location_type l)
      {
        return symbol_type (token::TOKEN_NISP_DOMAIN_, std::move (l));
      }
#else
      static
      symbol_type
      make_NISP_DOMAIN_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_NISP_DOMAIN_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LIFETIME_ (location_type l)
      {
        return symbol_type (token::TOKEN_LIFETIME_, std::move (l));
      }
#else
      static
      symbol_type
      make_LIFETIME_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_LIFETIME_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FQDN_ (location_type l)
      {
        return symbol_type (token::TOKEN_FQDN_, std::move (l));
      }
#else
      static
      symbol_type
      make_FQDN_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_FQDN_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ACCEPT_UNKNOWN_FQDN_ (location_type l)
      {
        return symbol_type (token::TOKEN_ACCEPT_UNKNOWN_FQDN_, std::move (l));
      }
#else
      static
      symbol_type
      make_ACCEPT_UNKNOWN_FQDN_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_ACCEPT_UNKNOWN_FQDN_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FQDN_DDNS_ADDRESS_ (location_type l)
      {
        return symbol_type (token::TOKEN_FQDN_DDNS_ADDRESS_, std::move (l));
      }
#else
      static
      symbol_type
      make_FQDN_DDNS_ADDRESS_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_FQDN_DDNS_ADDRESS_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DDNS_PROTOCOL_ (location_type l)
      {
        return symbol_type (token::TOKEN_DDNS_PROTOCOL_, std::move (l));
      }
#else
      static
      symbol_type
      make_DDNS_PROTOCOL_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_DDNS_PROTOCOL_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DDNS_TIMEOUT_ (location_type l)
      {
        return symbol_type (token::TOKEN_DDNS_TIMEOUT_, std::move (l));
      }
#else
      static
      symbol_type
      make_DDNS_TIMEOUT_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_DDNS_TIMEOUT_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ACCEPT_ONLY_ (location_type l)
      {
        return symbol_type (token::TOKEN_ACCEPT_ONLY_, std::move (l));
      }
#else
      static
      symbol_type
      make_ACCEPT_ONLY_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_ACCEPT_ONLY_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_REJECT_CLIENTS_ (location_type l)
      {
        return symbol_type (token::TOKEN_REJECT_CLIENTS_, std::move (l));
      }
#else
      static
      symbol_type
      make_REJECT_CLIENTS_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_REJECT_CLIENTS_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_POOL_ (location_type l)
      {
        return symbol_type (token::TOKEN_POOL_, std::move (l));
      }
#else
      static
      symbol_type
      make_POOL_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_POOL_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SHARE_ (location_type l)
      {
        return symbol_type (token::TOKEN_SHARE_, std::move (l));
      }
#else
      static
      symbol_type
      make_SHARE_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_SHARE_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T1_ (location_type l)
      {
        return symbol_type (token::TOKEN_T1_, std::move (l));
      }
#else
      static
      symbol_type
      make_T1_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_T1_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_T2_ (location_type l)
      {
        return symbol_type (token::TOKEN_T2_, std::move (l));
      }
#else
      static
      symbol_type
      make_T2_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_T2_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PREF_TIME_ (location_type l)
      {
        return symbol_type (token::TOKEN_PREF_TIME_, std::move (l));
      }
#else
      static
      symbol_type
      make_PREF_TIME_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_PREF_TIME_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VALID_TIME_ (location_type l)
      {
        return symbol_type (token::TOKEN_VALID_TIME_, std::move (l));
      }
#else
      static
      symbol_type
      make_VALID_TIME_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_VALID_TIME_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_UNICAST_ (location_type l)
      {
        return symbol_type (token::TOKEN_UNICAST_, std::move (l));
      }
#else
      static
      symbol_type
      make_UNICAST_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_UNICAST_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DROP_UNICAST_ (location_type l)
      {
        return symbol_type (token::TOKEN_DROP_UNICAST_, std::move (l));
      }
#else
      static
      symbol_type
      make_DROP_UNICAST_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_DROP_UNICAST_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PREFERENCE_ (location_type l)
      {
        return symbol_type (token::TOKEN_PREFERENCE_, std::move (l));
      }
#else
      static
      symbol_type
      make_PREFERENCE_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_PREFERENCE_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RAPID_COMMIT_ (location_type l)
      {
        return symbol_type (token::TOKEN_RAPID_COMMIT_, std::move (l));
      }
#else
      static
      symbol_type
      make_RAPID_COMMIT_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_RAPID_COMMIT_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IFACE_MAX_LEASE_ (location_type l)
      {
        return symbol_type (token::TOKEN_IFACE_MAX_LEASE_, std::move (l));
      }
#else
      static
      symbol_type
      make_IFACE_MAX_LEASE_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_IFACE_MAX_LEASE_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLASS_MAX_LEASE_ (location_type l)
      {
        return symbol_type (token::TOKEN_CLASS_MAX_LEASE_, std::move (l));
      }
#else
      static
      symbol_type
      make_CLASS_MAX_LEASE_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_CLASS_MAX_LEASE_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLNT_MAX_LEASE_ (location_type l)
      {
        return symbol_type (token::TOKEN_CLNT_MAX_LEASE_, std::move (l));
      }
#else
      static
      symbol_type
      make_CLNT_MAX_LEASE_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_CLNT_MAX_LEASE_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STATELESS_ (location_type l)
      {
        return symbol_type (token::TOKEN_STATELESS_, std::move (l));
      }
#else
      static
      symbol_type
      make_STATELESS_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_STATELESS_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CACHE_SIZE_ (location_type l)
      {
        return symbol_type (token::TOKEN_CACHE_SIZE_, std::move (l));
      }
#else
      static
      symbol_type
      make_CACHE_SIZE_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_CACHE_SIZE_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PDCLASS_ (location_type l)
      {
        return symbol_type (token::TOKEN_PDCLASS_, std::move (l));
      }
#else
      static
      symbol_type
      make_PDCLASS_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_PDCLASS_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PD_LENGTH_ (location_type l)
      {
        return symbol_type (token::TOKEN_PD_LENGTH_, std::move (l));
      }
#else
      static
      symbol_type
      make_PD_LENGTH_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_PD_LENGTH_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PD_POOL_ (location_type l)
      {
        return symbol_type (token::TOKEN_PD_POOL_, std::move (l));
      }
#else
      static
      symbol_type
      make_PD_POOL_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_PD_POOL_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SCRIPT_ (location_type l)
      {
        return symbol_type (token::TOKEN_SCRIPT_, std::move (l));
      }
#else
      static
      symbol_type
      make_SCRIPT_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_SCRIPT_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VENDOR_SPEC_ (location_type l)
      {
        return symbol_type (token::TOKEN_VENDOR_SPEC_, std::move (l));
      }
#else
      static
      symbol_type
      make_VENDOR_SPEC_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_VENDOR_SPEC_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLIENT_ (location_type l)
      {
        return symbol_type (token::TOKEN_CLIENT_, std::move (l));
      }
#else
      static
      symbol_type
      make_CLIENT_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_CLIENT_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DUID_KEYWORD_ (location_type l)
      {
        return symbol_type (token::TOKEN_DUID_KEYWORD_, std::move (l));
      }
#else
      static
      symbol_type
      make_DUID_KEYWORD_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_DUID_KEYWORD_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_REMOTE_ID_ (location_type l)
      {
        return symbol_type (token::TOKEN_REMOTE_ID_, std::move (l));
      }
#else
      static
      symbol_type
      make_REMOTE_ID_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_REMOTE_ID_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LINK_LOCAL_ (location_type l)
      {
        return symbol_type (token::TOKEN_LINK_LOCAL_, std::move (l));
      }
#else
      static
      symbol_type
      make_LINK_LOCAL_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_LINK_LOCAL_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ADDRESS_ (location_type l)
      {
        return symbol_type (token::TOKEN_ADDRESS_, std::move (l));
      }
#else
      static
      symbol_type
      make_ADDRESS_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_ADDRESS_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PREFIX_ (location_type l)
      {
        return symbol_type (token::TOKEN_PREFIX_, std::move (l));
      }
#else
      static
      symbol_type
      make_PREFIX_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_PREFIX_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_GUESS_MODE_ (location_type l)
      {
        return symbol_type (token::TOKEN_GUESS_MODE_, std::move (l));
      }
#else
      static
      symbol_type
      make_GUESS_MODE_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_GUESS_MODE_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_INACTIVE_MODE_ (location_type l)
      {
        return symbol_type (token::TOKEN_INACTIVE_MODE_, std::move (l));
      }
#else
      static
      symbol_type
      make_INACTIVE_MODE_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_INACTIVE_MODE_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EXPERIMENTAL_ (location_type l)
      {
        return symbol_type (token::TOKEN_EXPERIMENTAL_, std::move (l));
      }
#else
      static
      symbol_type
      make_EXPERIMENTAL_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_EXPERIMENTAL_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ADDR_PARAMS_ (location_type l)
      {
        return symbol_type (token::TOKEN_ADDR_PARAMS_, std::move (l));
      }
#else
      static
      symbol_type
      make_ADDR_PARAMS_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_ADDR_PARAMS_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_REMOTE_AUTOCONF_NEIGHBORS_ (location_type l)
      {
        return symbol_type (token::TOKEN_REMOTE_AUTOCONF_NEIGHBORS_, std::move (l));
      }
#else
      static
      symbol_type
      make_REMOTE_AUTOCONF_NEIGHBORS_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_REMOTE_AUTOCONF_NEIGHBORS_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AFTR_ (location_type l)
      {
        return symbol_type (token::TOKEN_AFTR_, std::move (l));
      }
#else
      static
      symbol_type
      make_AFTR_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_AFTR_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PERFORMANCE_MODE_ (location_type l)
      {
        return symbol_type (token::TOKEN_PERFORMANCE_MODE_, std::move (l));
      }
#else
      static
      symbol_type
      make_PERFORMANCE_MODE_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_PERFORMANCE_MODE_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AUTH_PROTOCOL_ (location_type l)
      {
        return symbol_type (token::TOKEN_AUTH_PROTOCOL_, std::move (l));
      }
#else
      static
      symbol_type
      make_AUTH_PROTOCOL_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_AUTH_PROTOCOL_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AUTH_ALGORITHM_ (location_type l)
      {
        return symbol_type (token::TOKEN_AUTH_ALGORITHM_, std::move (l));
      }
#else
      static
      symbol_type
      make_AUTH_ALGORITHM_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_AUTH_ALGORITHM_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AUTH_REPLAY_ (location_type l)
      {
        return symbol_type (token::TOKEN_AUTH_REPLAY_, std::move (l));
      }
#else
      static
      symbol_type
      make_AUTH_REPLAY_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_AUTH_REPLAY_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AUTH_METHODS_ (location_type l)
      {
        return symbol_type (token::TOKEN_AUTH_METHODS_, std::move (l));
      }
#else
      static
      symbol_type
      make_AUTH_METHODS_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_AUTH_METHODS_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AUTH_DROP_UNAUTH_ (location_type l)
      {
        return symbol_type (token::TOKEN_AUTH_DROP_UNAUTH_, std::move (l));
      }
#else
      static
      symbol_type
      make_AUTH_DROP_UNAUTH_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_AUTH_DROP_UNAUTH_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AUTH_REALM_ (location_type l)
      {
        return symbol_type (token::TOKEN_AUTH_REALM_, std::move (l));
      }
#else
      static
      symbol_type
      make_AUTH_REALM_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_AUTH_REALM_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_KEY_ (location_type l)
      {
        return symbol_type (token::TOKEN_KEY_, std::move (l));
      }
#else
      static
      symbol_type
      make_KEY_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_KEY_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SECRET_ (location_type l)
      {
        return symbol_type (token::TOKEN_SECRET_, std::move (l));
      }
#else
      static
      symbol_type
      make_SECRET_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_SECRET_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ALGORITHM_ (location_type l)
      {
        return symbol_type (token::TOKEN_ALGORITHM_, std::move (l));
      }
#else
      static
      symbol_type
      make_ALGORITHM_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_ALGORITHM_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FUDGE_ (location_type l)
      {
        return symbol_type (token::TOKEN_FUDGE_, std::move (l));
      }
#else
      static
      symbol_type
      make_FUDGE_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_FUDGE_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DIGEST_NONE_ (location_type l)
      {
        return symbol_type (token::TOKEN_DIGEST_NONE_, std::move (l));
      }
#else
      static
      symbol_type
      make_DIGEST_NONE_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_DIGEST_NONE_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DIGEST_PLAIN_ (location_type l)
      {
        return symbol_type (token::TOKEN_DIGEST_PLAIN_, std::move (l));
      }
#else
      static
      symbol_type
      make_DIGEST_PLAIN_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_DIGEST_PLAIN_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DIGEST_HMAC_MD5_ (location_type l)
      {
        return symbol_type (token::TOKEN_DIGEST_HMAC_MD5_, std::move (l));
      }
#else
      static
      symbol_type
      make_DIGEST_HMAC_MD5_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_DIGEST_HMAC_MD5_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DIGEST_HMAC_SHA1_ (location_type l)
      {
        return symbol_type (token::TOKEN_DIGEST_HMAC_SHA1_, std::move (l));
      }
#else
      static
      symbol_type
      make_DIGEST_HMAC_SHA1_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_DIGEST_HMAC_SHA1_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DIGEST_HMAC_SHA224_ (location_type l)
      {
        return symbol_type (token::TOKEN_DIGEST_HMAC_SHA224_, std::move (l));
      }
#else
      static
      symbol_type
      make_DIGEST_HMAC_SHA224_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_DIGEST_HMAC_SHA224_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DIGEST_HMAC_SHA256_ (location_type l)
      {
        return symbol_type (token::TOKEN_DIGEST_HMAC_SHA256_, std::move (l));
      }
#else
      static
      symbol_type
      make_DIGEST_HMAC_SHA256_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_DIGEST_HMAC_SHA256_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DIGEST_HMAC_SHA384_ (location_type l)
      {
        return symbol_type (token::TOKEN_DIGEST_HMAC_SHA384_, std::move (l));
      }
#else
      static
      symbol_type
      make_DIGEST_HMAC_SHA384_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_DIGEST_HMAC_SHA384_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DIGEST_HMAC_SHA512_ (location_type l)
      {
        return symbol_type (token::TOKEN_DIGEST_HMAC_SHA512_, std::move (l));
      }
#else
      static
      symbol_type
      make_DIGEST_HMAC_SHA512_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_DIGEST_HMAC_SHA512_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ACCEPT_LEASEQUERY_ (location_type l)
      {
        return symbol_type (token::TOKEN_ACCEPT_LEASEQUERY_, std::move (l));
      }
#else
      static
      symbol_type
      make_ACCEPT_LEASEQUERY_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_ACCEPT_LEASEQUERY_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BULKLQ_ACCEPT_ (location_type l)
      {
        return symbol_type (token::TOKEN_BULKLQ_ACCEPT_, std::move (l));
      }
#else
      static
      symbol_type
      make_BULKLQ_ACCEPT_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_BULKLQ_ACCEPT_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BULKLQ_TCPPORT_ (location_type l)
      {
        return symbol_type (token::TOKEN_BULKLQ_TCPPORT_, std::move (l));
      }
#else
      static
      symbol_type
      make_BULKLQ_TCPPORT_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_BULKLQ_TCPPORT_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BULKLQ_MAX_CONNS_ (location_type l)
      {
        return symbol_type (token::TOKEN_BULKLQ_MAX_CONNS_, std::move (l));
      }
#else
      static
      symbol_type
      make_BULKLQ_MAX_CONNS_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_BULKLQ_MAX_CONNS_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BULKLQ_TIMEOUT_ (location_type l)
      {
        return symbol_type (token::TOKEN_BULKLQ_TIMEOUT_, std::move (l));
      }
#else
      static
      symbol_type
      make_BULKLQ_TIMEOUT_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_BULKLQ_TIMEOUT_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLIENT_CLASS_ (location_type l)
      {
        return symbol_type (token::TOKEN_CLIENT_CLASS_, std::move (l));
      }
#else
      static
      symbol_type
      make_CLIENT_CLASS_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_CLIENT_CLASS_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_MATCH_IF_ (location_type l)
      {
        return symbol_type (token::TOKEN_MATCH_IF_, std::move (l));
      }
#else
      static
      symbol_type
      make_MATCH_IF_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_MATCH_IF_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EQ_ (location_type l)
      {
        return symbol_type (token::TOKEN_EQ_, std::move (l));
      }
#else
      static
      symbol_type
      make_EQ_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_EQ_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_AND_ (location_type l)
      {
        return symbol_type (token::TOKEN_AND_, std::move (l));
      }
#else
      static
      symbol_type
      make_AND_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_AND_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OR_ (location_type l)
      {
        return symbol_type (token::TOKEN_OR_, std::move (l));
      }
#else
      static
      symbol_type
      make_OR_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_OR_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLIENT_VENDOR_SPEC_ENTERPRISE_NUM_ (location_type l)
      {
        return symbol_type (token::TOKEN_CLIENT_VENDOR_SPEC_ENTERPRISE_NUM_, std::move (l));
      }
#else
      static
      symbol_type
      make_CLIENT_VENDOR_SPEC_ENTERPRISE_NUM_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_CLIENT_VENDOR_SPEC_ENTERPRISE_NUM_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLIENT_VENDOR_SPEC_DATA_ (location_type l)
      {
        return symbol_type (token::TOKEN_CLIENT_VENDOR_SPEC_DATA_, std::move (l));
      }
#else
      static
      symbol_type
      make_CLIENT_VENDOR_SPEC_DATA_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_CLIENT_VENDOR_SPEC_DATA_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLIENT_VENDOR_CLASS_EN_ (location_type l)
      {
        return symbol_type (token::TOKEN_CLIENT_VENDOR_CLASS_EN_, std::move (l));
      }
#else
      static
      symbol_type
      make_CLIENT_VENDOR_CLASS_EN_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_CLIENT_VENDOR_CLASS_EN_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLIENT_VENDOR_CLASS_DATA_ (location_type l)
      {
        return symbol_type (token::TOKEN_CLIENT_VENDOR_CLASS_DATA_, std::move (l));
      }
#else
      static
      symbol_type
      make_CLIENT_VENDOR_CLASS_DATA_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_CLIENT_VENDOR_CLASS_DATA_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RECONFIGURE_ENABLED_ (location_type l)
      {
        return symbol_type (token::TOKEN_RECONFIGURE_ENABLED_, std::move (l));
      }
#else
      static
      symbol_type
      make_RECONFIGURE_ENABLED_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_RECONFIGURE_ENABLED_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ALLOW_ (location_type l)
      {
        return symbol_type (token::TOKEN_ALLOW_, std::move (l));
      }
#else
      static
      symbol_type
      make_ALLOW_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_ALLOW_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DENY_ (location_type l)
      {
        return symbol_type (token::TOKEN_DENY_, std::move (l));
      }
#else
      static
      symbol_type
      make_DENY_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_DENY_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SUBSTRING_ (location_type l)
      {
        return symbol_type (token::TOKEN_SUBSTRING_, std::move (l));
      }
#else
      static
      symbol_type
      make_SUBSTRING_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_SUBSTRING_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STRING_KEYWORD_ (location_type l)
      {
        return symbol_type (token::TOKEN_STRING_KEYWORD_, std::move (l));
      }
#else
      static
      symbol_type
      make_STRING_KEYWORD_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_STRING_KEYWORD_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ADDRESS_LIST_ (location_type l)
      {
        return symbol_type (token::TOKEN_ADDRESS_LIST_, std::move (l));
      }
#else
      static
      symbol_type
      make_ADDRESS_LIST_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_ADDRESS_LIST_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONTAIN_ (location_type l)
      {
        return symbol_type (token::TOKEN_CONTAIN_, std::move (l));
      }
#else
      static
      symbol_type
      make_CONTAIN_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_CONTAIN_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NEXT_HOP_ (location_type l)
      {
        return symbol_type (token::TOKEN_NEXT_HOP_, std::move (l));
      }
#else
      static
      symbol_type
      make_NEXT_HOP_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_NEXT_HOP_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ROUTE_ (location_type l)
      {
        return symbol_type (token::TOKEN_ROUTE_, std::move (l));
      }
#else
      static
      symbol_type
      make_ROUTE_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_ROUTE_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_INFINITE_ (location_type l)
      {
        return symbol_type (token::TOKEN_INFINITE_, std::move (l));
      }
#else
      static
      symbol_type
      make_INFINITE_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_INFINITE_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SUBNET_ (location_type l)
      {
        return symbol_type (token::TOKEN_SUBNET_, std::move (l));
      }
#else
      static
      symbol_type
      make_SUBNET_ (const location_type& l)
      {
        return symbol_type (token::TOKEN_SUBNET_, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STRING_ (std::string v, location_type l)
      {
        return symbol_type (token::TOKEN_STRING_, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_STRING_ (const std::string& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_STRING_, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_HEXNUMBER_ (uint32_t v, location_type l)
      {
        return symbol_type (token::TOKEN_HEXNUMBER_, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_HEXNUMBER_ (const uint32_t& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_HEXNUMBER_, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_INTNUMBER_ (uint32_t v, location_type l)
      {
        return symbol_type (token::TOKEN_INTNUMBER_, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_INTNUMBER_ (const uint32_t& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_INTNUMBER_, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IPV6ADDR_ (std::string v, location_type l)
      {
        return symbol_type (token::TOKEN_IPV6ADDR_, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_IPV6ADDR_ (const std::string& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_IPV6ADDR_, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DUID_ (std::string v, location_type l)
      {
        return symbol_type (token::TOKEN_DUID_, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_DUID_ (const std::string& v, const location_type& l)
      {
        return symbol_type (token::TOKEN_DUID_, v, l);
      }
#endif


  private:
    /// This class is not copyable.
    SrvParser (const SrvParser&);
    SrvParser& operator= (const SrvParser&);

    /// State numbers.
    typedef int state_type;

    /// Generate an error message.
    /// \param yystate   the state where the error occurred.
    /// \param yyla      the lookahead token.
    virtual std::string yysyntax_error_ (state_type yystate,
                                         const symbol_type& yyla) const;

    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue);

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue);

    static const short yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token number \a t to a symbol number.
    static token_number_type yytranslate_ (token_type t);

    // Tables.
  // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
  // STATE-NUM.
  static const short yypact_[];

  // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
  // Performed when YYTABLE does not specify something else to do.  Zero
  // means the default is an error.
  static const unsigned short yydefact_[];

  // YYPGOTO[NTERM-NUM].
  static const short yypgoto_[];

  // YYDEFGOTO[NTERM-NUM].
  static const short yydefgoto_[];

  // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
  // positive, shift that token.  If negative, reduce the rule whose
  // number is the opposite.  If YYTABLE_NINF, syntax error.
  static const unsigned short yytable_[];

  static const short yycheck_[];

  // YYSTOS[STATE-NUM] -- The (internal number of the) accessing
  // symbol of state STATE-NUM.
  static const unsigned short yystos_[];

  // YYR1[YYN] -- Symbol number of symbol that rule YYN derives.
  static const unsigned short yyr1_[];

  // YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.
  static const unsigned char yyr2_[];


    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    static std::string yytnamerr_ (const char *n);


    /// For a symbol, its name in clear.
    static const char* const yytname_[];
#if YYDEBUG
  // YYRLINE[YYN] -- Source line where rule number YYN was defined.
  static const unsigned short yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r);
    /// Print the state stack on the debug stream.
    virtual void yystack_print_ ();

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol type, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol type as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol type from \a that.
      void move (by_state& that);

      /// The (internal) type number (corresponding to \a state).
      /// \a empty_symbol when empty.
      symbol_number_type type_get () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      enum { empty_state = -1 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::reverse_iterator iterator;
      typedef typename S::const_reverse_iterator const_iterator;
      typedef typename S::size_type size_type;

      stack (size_type n = 200)
        : seq_ (n)
      {}

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (size_type i)
      {
        return seq_[size () - 1 - i];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (int i)
      {
        return operator[] (size_type (i));
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (size_type i) const
      {
        return seq_[size () - 1 - i];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (int i) const
      {
        return operator[] (size_type (i));
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (int n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      size_type
      size () const YY_NOEXCEPT
      {
        return seq_.size ();
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.rbegin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.rend ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, int range)
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (int i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        int range_;
      };

    private:
      stack (const stack&);
      stack& operator= (const stack&);
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1);

    /// Constants.
    enum
    {
      yyeof_ = 0,
      yylast_ = 722,     ///< Last index in yytable_.
      yynnts_ = 142,  ///< Number of nonterminal symbols.
      yyfinal_ = 207, ///< Termination state number.
      yyterror_ = 1,
      yyerrcode_ = 256,
      yyntokens_ = 122  ///< Number of tokens.
    };


    // User arguments.
    SrvParserContext& ctx;
  };

  inline
  SrvParser::token_number_type
  SrvParser::yytranslate_ (token_type t)
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const token_number_type
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     120,   121,     2,     2,   119,   117,     2,   118,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   116,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   114,     2,   115,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113
    };
    const unsigned user_token_number_max_ = 368;
    const token_number_type undef_token_ = 2;

    if (static_cast<int> (t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }

  // basic_symbol.
#if 201103L <= YY_CPLUSPLUS
  template <typename Base>
  SrvParser::basic_symbol<Base>::basic_symbol (basic_symbol&& that)
    : Base (std::move (that))
    , value ()
    , location (std::move (that.location))
  {
    switch (this->type_get ())
    {
      case 109: // STRING_
      case 112: // IPV6ADDR_
      case 113: // DUID_
        value.move< std::string > (std::move (that.value));
        break;

      case 110: // HEXNUMBER_
      case 111: // INTNUMBER_
      case 172: // Number
        value.move< uint32_t > (std::move (that.value));
        break;

      default:
        break;
    }

  }
#endif

  template <typename Base>
  SrvParser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->type_get ())
    {
      case 109: // STRING_
      case 112: // IPV6ADDR_
      case 113: // DUID_
        value.copy< std::string > (YY_MOVE (that.value));
        break;

      case 110: // HEXNUMBER_
      case 111: // INTNUMBER_
      case 172: // Number
        value.copy< uint32_t > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }



  template <typename Base>
  bool
  SrvParser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  void
  SrvParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->type_get ())
    {
      case 109: // STRING_
      case 112: // IPV6ADDR_
      case 113: // DUID_
        value.move< std::string > (YY_MOVE (s.value));
        break;

      case 110: // HEXNUMBER_
      case 111: // INTNUMBER_
      case 172: // Number
        value.move< uint32_t > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_type.
  inline
  SrvParser::by_type::by_type ()
    : type (empty_symbol)
  {}

#if 201103L <= YY_CPLUSPLUS
  inline
  SrvParser::by_type::by_type (by_type&& that)
    : type (that.type)
  {
    that.clear ();
  }
#endif

  inline
  SrvParser::by_type::by_type (const by_type& that)
    : type (that.type)
  {}

  inline
  SrvParser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  SrvParser::by_type::clear ()
  {
    type = empty_symbol;
  }

  inline
  void
  SrvParser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  inline
  int
  SrvParser::by_type::type_get () const YY_NOEXCEPT
  {
    return type;
  }

  inline
  SrvParser::token_type
  SrvParser::by_type::token () const YY_NOEXCEPT
  {
    // YYTOKNUM[NUM] -- (External) token number corresponding to the
    // (internal) symbol number NUM (which must be that of a token).  */
    static
    const unsigned short
    yytoken_number_[] =
    {
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   123,   125,    59,    45,    47,    44,
      40,    41
    };
    return token_type (yytoken_number_[type]);
  }

#line 5 "SrvParser.y"
} // dibbler
#line 3014 "SrvParser.h"





#endif // !YY_YY_SRVPARSER_H_INCLUDED
