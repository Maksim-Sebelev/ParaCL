/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"

#include <iostream>
#include <cstdio>
#include <unordered_map>
#include <memory>
#include <vector>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "global/global.hpp"

extern FILE* yyin;
extern int yyparse();

inline ParaCL::ProgramAST program;

extern std::string current_file;

typedef struct YYLTYPE YYLTYPE;
typedef union YYSTYPE YYSTYPE;

void yyerror(YYLTYPE* loc, const char* msg);
int yylex(YYSTYPE* yylval_param, YYLTYPE* yylloc_param);


#line 96 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
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

#include "parser.tab.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_OR = 3,                         /* OR  */
  YYSYMBOL_AND = 4,                        /* AND  */
  YYSYMBOL_ISEQ = 5,                       /* ISEQ  */
  YYSYMBOL_ISNE = 6,                       /* ISNE  */
  YYSYMBOL_ISAB = 7,                       /* ISAB  */
  YYSYMBOL_ISABE = 8,                      /* ISABE  */
  YYSYMBOL_ISLS = 9,                       /* ISLS  */
  YYSYMBOL_ISLSE = 10,                     /* ISLSE  */
  YYSYMBOL_ADD = 11,                       /* ADD  */
  YYSYMBOL_SUB = 12,                       /* SUB  */
  YYSYMBOL_MUL = 13,                       /* MUL  */
  YYSYMBOL_DIV = 14,                       /* DIV  */
  YYSYMBOL_REM = 15,                       /* REM  */
  YYSYMBOL_NEG = 16,                       /* NEG  */
  YYSYMBOL_NOT = 17,                       /* NOT  */
  YYSYMBOL_AS = 18,                        /* AS  */
  YYSYMBOL_ADDASGN = 19,                   /* ADDASGN  */
  YYSYMBOL_SUBASGN = 20,                   /* SUBASGN  */
  YYSYMBOL_MULASGN = 21,                   /* MULASGN  */
  YYSYMBOL_DIVASGN = 22,                   /* DIVASGN  */
  YYSYMBOL_NUM = 23,                       /* NUM  */
  YYSYMBOL_VAR = 24,                       /* VAR  */
  YYSYMBOL_LCIB = 25,                      /* LCIB  */
  YYSYMBOL_RCIB = 26,                      /* RCIB  */
  YYSYMBOL_LCUB = 27,                      /* LCUB  */
  YYSYMBOL_RCUB = 28,                      /* RCUB  */
  YYSYMBOL_WH = 29,                        /* WH  */
  YYSYMBOL_IN = 30,                        /* IN  */
  YYSYMBOL_PRINT = 31,                     /* PRINT  */
  YYSYMBOL_IF = 32,                        /* IF  */
  YYSYMBOL_ELIF = 33,                      /* ELIF  */
  YYSYMBOL_ELSE = 34,                      /* ELSE  */
  YYSYMBOL_SC = 35,                        /* SC  */
  YYSYMBOL_YYACCEPT = 36,                  /* $accept  */
  YYSYMBOL_program = 37,                   /* program  */
  YYSYMBOL_statements = 38,                /* statements  */
  YYSYMBOL_statement = 39,                 /* statement  */
  YYSYMBOL_assignment = 40,                /* assignment  */
  YYSYMBOL_combined_assignment = 41,       /* combined_assignment  */
  YYSYMBOL_print_statement = 42,           /* print_statement  */
  YYSYMBOL_while_statement = 43,           /* while_statement  */
  YYSYMBOL_condition_statement = 44,       /* condition_statement  */
  YYSYMBOL_if_statement = 45,              /* if_statement  */
  YYSYMBOL_elif_statements = 46,           /* elif_statements  */
  YYSYMBOL_else_statement = 47,            /* else_statement  */
  YYSYMBOL_expression = 48,                /* expression  */
  YYSYMBOL_assignment_expression = 49,     /* assignment_expression  */
  YYSYMBOL_logical_or_expression = 50,     /* logical_or_expression  */
  YYSYMBOL_logical_and_expression = 51,    /* logical_and_expression  */
  YYSYMBOL_equality_expression = 52,       /* equality_expression  */
  YYSYMBOL_relational_expression = 53,     /* relational_expression  */
  YYSYMBOL_additive_expression = 54,       /* additive_expression  */
  YYSYMBOL_multiplicative_expression = 55, /* multiplicative_expression  */
  YYSYMBOL_unary_expression = 56,          /* unary_expression  */
  YYSYMBOL_factor = 57,                    /* factor  */
  YYSYMBOL_block = 58,                     /* block  */
  YYSYMBOL_one_stmt_block = 59             /* one_stmt_block  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   114

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  36
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  24
/* YYNRULES -- Number of rules.  */
#define YYNRULES  61
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  116

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   290


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
      35
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    77,    77,    87,    90,    97,   100,   103,   106,   109,
     115,   122,   130,   138,   146,   157,   163,   169,   178,   193,
     199,   208,   211,   218,   228,   229,   234,   242,   246,   247,
     251,   259,   267,   275,   286,   287,   297,   298,   308,   309,
     316,   326,   327,   334,   341,   348,   358,   359,   366,   376,
     377,   384,   391,   401,   402,   408,   417,   420,   424,   427,
     433,   444
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "OR", "AND", "ISEQ",
  "ISNE", "ISAB", "ISABE", "ISLS", "ISLSE", "ADD", "SUB", "MUL", "DIV",
  "REM", "NEG", "NOT", "AS", "ADDASGN", "SUBASGN", "MULASGN", "DIVASGN",
  "NUM", "VAR", "LCIB", "RCIB", "LCUB", "RCUB", "WH", "IN", "PRINT", "IF",
  "ELIF", "ELSE", "SC", "$accept", "program", "statements", "statement",
  "assignment", "combined_assignment", "print_statement",
  "while_statement", "condition_statement", "if_statement",
  "elif_statements", "else_statement", "expression",
  "assignment_expression", "logical_or_expression",
  "logical_and_expression", "equality_expression", "relational_expression",
  "additive_expression", "multiplicative_expression", "unary_expression",
  "factor", "block", "one_stmt_block", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-81)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
     -81,    10,    43,   -81,    58,    -9,     1,    -5,   -81,   -23,
      12,    14,   -81,   -81,   -81,     1,     1,     1,     1,     1,
       1,     5,     5,   -81,    64,     1,   -81,   -81,   -81,    49,
      32,     9,    85,    22,    90,   -81,   -81,     1,   -81,   -81,
     -81,    25,   -81,   -81,   -81,   -81,   -81,    27,   -81,   -81,
     -81,     1,     1,     1,     1,     1,    47,     5,     5,     5,
       5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
      74,    44,    13,   -81,    19,   -81,   -81,   -81,   -81,   -81,
     -81,    32,     9,    85,    85,    22,    22,    22,    22,    90,
      90,   -81,   -81,   -81,    33,     1,   -81,   -81,   -81,   -81,
     -81,   -81,   -81,    75,    43,    80,    81,    82,    39,   -81,
     -81,   -81,   -81,   -81,    83,   -81
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       3,     0,     2,     1,     0,     0,     0,     0,     4,     0,
       0,     0,     8,     9,    21,     0,     0,     0,     0,     0,
       0,     0,     0,    56,    57,     0,    59,    15,    27,    28,
      34,    36,    38,    41,    46,    49,    53,     0,     5,     6,
       7,    24,    10,    11,    12,    13,    14,     0,    57,    54,
      55,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    18,     0,    29,    30,    31,    32,    33,
      58,    35,    37,    39,    40,    42,    43,    44,    45,    47,
      48,    50,    51,    52,     0,     0,     3,    61,    26,     3,
      17,     3,    20,     0,    60,     0,     0,     0,     0,    25,
      16,    19,     3,    23,     0,    22
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -81,   -81,    61,    -2,   -81,   -81,   -81,   -81,   -81,   -81,
     -81,   -81,   -14,    36,   -81,    55,    56,   -21,    35,    41,
     -13,   -81,   -80,   -67
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     1,   104,    97,     9,    10,    11,    12,    13,    14,
      41,    73,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,   105,    98
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
       8,    42,    43,    44,    45,    46,    47,   100,    49,    50,
       3,    56,    38,    21,    59,    60,    20,    21,    22,   106,
      37,   107,    22,    70,    23,    24,    25,   102,    23,    48,
      25,    26,   114,    65,    66,    26,    58,     4,    83,    84,
      96,   113,     5,     4,     6,     7,    99,    39,     5,    40,
       6,     7,    57,    74,    91,    92,    93,     4,    71,    72,
     101,     2,     5,     4,     6,     7,   112,     4,     5,    95,
       6,     7,     5,    80,     6,     7,    15,    16,    17,    18,
      19,   103,    51,    52,    53,    54,    55,    75,    76,    77,
      78,    79,    61,    62,    63,    64,    85,    86,    87,    88,
      94,   108,     8,    67,    68,    69,    89,    90,   109,   110,
     111,   115,    81,     0,    82
};

static const yytype_int8 yycheck[] =
{
       2,    15,    16,    17,    18,    19,    20,    74,    21,    22,
       0,    25,    35,    12,     5,     6,    25,    12,    17,    99,
      25,   101,    17,    37,    23,    24,    25,    94,    23,    24,
      25,    30,   112,    11,    12,    30,     4,    24,    59,    60,
      27,   108,    29,    24,    31,    32,    27,    35,    29,    35,
      31,    32,     3,    26,    67,    68,    69,    24,    33,    34,
      27,     0,    29,    24,    31,    32,    27,    24,    29,    25,
      31,    32,    29,    26,    31,    32,    18,    19,    20,    21,
      22,    95,    18,    19,    20,    21,    22,    51,    52,    53,
      54,    55,     7,     8,     9,    10,    61,    62,    63,    64,
      26,    26,   104,    13,    14,    15,    65,    66,    28,    28,
      28,    28,    57,    -1,    58
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    37,    38,     0,    24,    29,    31,    32,    39,    40,
      41,    42,    43,    44,    45,    18,    19,    20,    21,    22,
      25,    12,    17,    23,    24,    25,    30,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    25,    35,    35,
      35,    46,    48,    48,    48,    48,    48,    48,    24,    56,
      56,    18,    19,    20,    21,    22,    48,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      48,    33,    34,    47,    26,    49,    49,    49,    49,    49,
      26,    51,    52,    53,    53,    54,    54,    54,    54,    55,
      55,    56,    56,    56,    26,    25,    27,    39,    59,    27,
      59,    27,    59,    48,    38,    58,    58,    58,    26,    28,
      28,    28,    27,    59,    58,    28
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    36,    37,    38,    38,    39,    39,    39,    39,    39,
      40,    41,    41,    41,    41,    42,    43,    43,    44,    45,
      45,    46,    46,    46,    47,    47,    47,    48,    49,    49,
      49,    49,    49,    49,    50,    50,    51,    51,    52,    52,
      52,    53,    53,    53,    53,    53,    54,    54,    54,    55,
      55,    55,    55,    56,    56,    56,    57,    57,    57,    57,
      58,    59
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     2,     2,     2,     2,     1,     1,
       3,     3,     3,     3,     3,     2,     7,     5,     3,     7,
       5,     0,     8,     6,     0,     4,     2,     1,     1,     3,
       3,     3,     3,     3,     1,     3,     1,     3,     1,     3,
       3,     1,     3,     3,     3,     3,     1,     3,     3,     1,
       3,     3,     3,     1,     2,     2,     1,     1,     3,     1,
       1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: statements  */
#line 77 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
               {
        for (auto stmt : *(yyvsp[0].stmt_vector))
        {
            program.statements.push_back(std::unique_ptr<ParaCL::Stmt>(stmt));
        }
        delete (yyvsp[0].stmt_vector);
    }
#line 1349 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 3: /* statements: %empty  */
#line 87 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
           {
        (yyval.stmt_vector) = new std::vector<ParaCL::Stmt*>();
    }
#line 1357 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 4: /* statements: statements statement  */
#line 90 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                           {
        (yyvsp[-1].stmt_vector)->push_back((yyvsp[0].stmt));
        (yyval.stmt_vector) = (yyvsp[-1].stmt_vector);
    }
#line 1366 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 5: /* statement: assignment SC  */
#line 97 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                  { 
        (yyval.stmt) = (yyvsp[-1].stmt);
    }
#line 1374 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 6: /* statement: combined_assignment SC  */
#line 100 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                             {
        (yyval.stmt) = (yyvsp[-1].stmt);
    }
#line 1382 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 7: /* statement: print_statement SC  */
#line 103 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                         {
        (yyval.stmt) = (yyvsp[-1].stmt);
    }
#line 1390 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 8: /* statement: while_statement  */
#line 106 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                      {
        (yyval.stmt) = (yyvsp[0].stmt);
    }
#line 1398 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 9: /* statement: condition_statement  */
#line 109 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                          {
        (yyval.stmt) = (yyvsp[0].condition_stmt);
    }
#line 1406 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 10: /* assignment: VAR AS expression  */
#line 115 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                      {
        (yyval.stmt) = new ParaCL::AssignStmt(*(yyvsp[-2].str_value), std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr)));
        delete (yyvsp[-2].str_value);
    }
#line 1415 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 11: /* combined_assignment: VAR ADDASGN expression  */
#line 122 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                           {
        (yyval.stmt) = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::ADDASGN,
            *(yyvsp[-2].str_value),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
        delete (yyvsp[-2].str_value);
    }
#line 1428 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 12: /* combined_assignment: VAR SUBASGN expression  */
#line 130 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                             {
        (yyval.stmt) = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::SUBASGN,
            *(yyvsp[-2].str_value),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
        delete (yyvsp[-2].str_value);
    }
#line 1441 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 13: /* combined_assignment: VAR MULASGN expression  */
#line 138 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                             {
        (yyval.stmt) = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::MULASGN,
            *(yyvsp[-2].str_value),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
        delete (yyvsp[-2].str_value);
    }
#line 1454 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 14: /* combined_assignment: VAR DIVASGN expression  */
#line 146 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                             {
        (yyval.stmt) = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::DIVASGN,
            *(yyvsp[-2].str_value),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
        delete (yyvsp[-2].str_value);
    }
#line 1467 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 15: /* print_statement: PRINT expression  */
#line 157 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                     {
        (yyval.stmt) = new ParaCL::PrintStmt(std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr)));
    }
#line 1475 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 16: /* while_statement: WH LCIB expression RCIB LCUB block RCUB  */
#line 163 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                            {
        (yyval.stmt) = new ParaCL::WhileStmt(
            std::unique_ptr<ParaCL::Expr>((yyvsp[-4].expr)),
            std::unique_ptr<ParaCL::BlockStmt>((yyvsp[-1].block))
        );
    }
#line 1486 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 17: /* while_statement: WH LCIB expression RCIB one_stmt_block  */
#line 169 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                             {
        (yyval.stmt) = new ParaCL::WhileStmt(
            std::unique_ptr<ParaCL::Expr>((yyvsp[-2].expr)), 
            std::unique_ptr<ParaCL::BlockStmt>((yyvsp[0].block))
        );
    }
#line 1497 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 18: /* condition_statement: if_statement elif_statements else_statement  */
#line 178 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                {
        auto cond_stmt = new ParaCL::ConditionStatement(
            std::unique_ptr<ParaCL::IfStatement>((yyvsp[-2].if_stmt))
        );

        cond_stmt->add_elif_conditions((yyvsp[-1].elif_stmts));
        delete (yyvsp[-1].elif_stmts);

        if ((yyvsp[0].else_stmt)) cond_stmt->add_else_condition(std::unique_ptr<ParaCL::ElseStatement>((yyvsp[0].else_stmt)));

        (yyval.condition_stmt) = cond_stmt;
    }
#line 1514 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 19: /* if_statement: IF LCIB expression RCIB LCUB block RCUB  */
#line 193 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                            {
        (yyval.if_stmt) = new ParaCL::IfStatement(
            std::unique_ptr<ParaCL::Expr>((yyvsp[-4].expr)),
            std::unique_ptr<ParaCL::BlockStmt>((yyvsp[-1].block))
        );
    }
#line 1525 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 20: /* if_statement: IF LCIB expression RCIB one_stmt_block  */
#line 199 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                             {
        (yyval.if_stmt) = new ParaCL::IfStatement(
            std::unique_ptr<ParaCL::Expr>((yyvsp[-2].expr)),
            std::unique_ptr<ParaCL::BlockStmt>((yyvsp[0].block))
        );
    }
#line 1536 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 21: /* elif_statements: %empty  */
#line 208 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
           {
        (yyval.elif_stmts) = new std::vector<ParaCL::ElifStatement*>();
    }
#line 1544 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 22: /* elif_statements: elif_statements ELIF LCIB expression RCIB LCUB block RCUB  */
#line 211 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                                {
        (yyvsp[-7].elif_stmts)->push_back(new ParaCL::ElifStatement(
            std::unique_ptr<ParaCL::Expr>((yyvsp[-4].expr)),
            std::unique_ptr<ParaCL::BlockStmt>((yyvsp[-1].block))
        ));
        (yyval.elif_stmts) = (yyvsp[-7].elif_stmts);
    }
#line 1556 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 23: /* elif_statements: elif_statements ELIF LCIB expression RCIB one_stmt_block  */
#line 218 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                               {
        (yyvsp[-5].elif_stmts)->push_back(new ParaCL::ElifStatement(
            std::unique_ptr<ParaCL::Expr>((yyvsp[-2].expr)),
            std::unique_ptr<ParaCL::BlockStmt>((yyvsp[0].block))
        ));
        (yyval.elif_stmts) = (yyvsp[-5].elif_stmts);
    }
#line 1568 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 24: /* else_statement: %empty  */
#line 228 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
           { (yyval.else_stmt) = nullptr; }
#line 1574 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 25: /* else_statement: ELSE LCUB block RCUB  */
#line 229 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                           {
        (yyval.else_stmt) = new ParaCL::ElseStatement(
            std::unique_ptr<ParaCL::BlockStmt>((yyvsp[-1].block))
        );
    }
#line 1584 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 26: /* else_statement: ELSE one_stmt_block  */
#line 234 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                          {
        (yyval.else_stmt) = new ParaCL::ElseStatement(
            std::unique_ptr<ParaCL::BlockStmt>((yyvsp[0].block))
        );
    }
#line 1594 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 27: /* expression: assignment_expression  */
#line 242 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                          { (yyval.expr) = (yyvsp[0].expr); }
#line 1600 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 28: /* assignment_expression: logical_or_expression  */
#line 246 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                          { (yyval.expr) = (yyvsp[0].expr); }
#line 1606 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 29: /* assignment_expression: VAR AS assignment_expression  */
#line 247 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                            {
        (yyval.expr) = new ParaCL::AssignExpr(*(yyvsp[-2].str_value), std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr)));
        delete (yyvsp[-2].str_value);
    }
#line 1615 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 30: /* assignment_expression: VAR ADDASGN assignment_expression  */
#line 251 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                      {
        (yyval.expr) = new ParaCL::CombinedAssingExpr(
            ParaCL::token_t::ADDASGN,
            *(yyvsp[-2].str_value),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
        delete (yyvsp[-2].str_value);
    }
#line 1628 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 31: /* assignment_expression: VAR SUBASGN assignment_expression  */
#line 259 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                      {
        (yyval.expr) = new ParaCL::CombinedAssingExpr(
            ParaCL::token_t::SUBASGN,
            *(yyvsp[-2].str_value),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
        delete (yyvsp[-2].str_value);
    }
#line 1641 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 32: /* assignment_expression: VAR MULASGN assignment_expression  */
#line 267 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                      {
        (yyval.expr) = new ParaCL::CombinedAssingExpr(
            ParaCL::token_t::MULASGN,
            *(yyvsp[-2].str_value),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
        delete (yyvsp[-2].str_value);
    }
#line 1654 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 33: /* assignment_expression: VAR DIVASGN assignment_expression  */
#line 275 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                      {
        (yyval.expr) = new ParaCL::CombinedAssingExpr(
            ParaCL::token_t::DIVASGN,
            *(yyvsp[-2].str_value),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
        delete (yyvsp[-2].str_value);
    }
#line 1667 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 34: /* logical_or_expression: logical_and_expression  */
#line 286 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                           { (yyval.expr) = (yyvsp[0].expr); }
#line 1673 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 35: /* logical_or_expression: logical_or_expression OR logical_and_expression  */
#line 287 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                               {
        (yyval.expr) = new ParaCL::BinExpr(
            ParaCL::token_t::OR,
            std::unique_ptr<ParaCL::Expr>((yyvsp[-2].expr)),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
    }
#line 1685 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 36: /* logical_and_expression: equality_expression  */
#line 297 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                        { (yyval.expr) = (yyvsp[0].expr); }
#line 1691 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 37: /* logical_and_expression: logical_and_expression AND equality_expression  */
#line 298 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                               {
        (yyval.expr) = new ParaCL::BinExpr(
            ParaCL::token_t::AND,
            std::unique_ptr<ParaCL::Expr>((yyvsp[-2].expr)),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
    }
#line 1703 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 38: /* equality_expression: relational_expression  */
#line 308 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                          { (yyval.expr) = (yyvsp[0].expr); }
#line 1709 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 39: /* equality_expression: equality_expression ISEQ relational_expression  */
#line 309 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                                {
        (yyval.expr) = new ParaCL::BinExpr(
            ParaCL::token_t::ISEQ,
            std::unique_ptr<ParaCL::Expr>((yyvsp[-2].expr)),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
    }
#line 1721 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 40: /* equality_expression: equality_expression ISNE relational_expression  */
#line 316 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                                {
        (yyval.expr) = new ParaCL::BinExpr(
            ParaCL::token_t::ISNE,
            std::unique_ptr<ParaCL::Expr>((yyvsp[-2].expr)),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
    }
#line 1733 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 41: /* relational_expression: additive_expression  */
#line 326 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                        { (yyval.expr) = (yyvsp[0].expr); }
#line 1739 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 42: /* relational_expression: relational_expression ISAB additive_expression  */
#line 327 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                                {
        (yyval.expr) = new ParaCL::BinExpr(
            ParaCL::token_t::ISAB,
            std::unique_ptr<ParaCL::Expr>((yyvsp[-2].expr)),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
    }
#line 1751 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 43: /* relational_expression: relational_expression ISABE additive_expression  */
#line 334 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                                  {
        (yyval.expr) = new ParaCL::BinExpr(
            ParaCL::token_t::ISABE,
            std::unique_ptr<ParaCL::Expr>((yyvsp[-2].expr)),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
    }
#line 1763 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 44: /* relational_expression: relational_expression ISLS additive_expression  */
#line 341 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                                {
        (yyval.expr) = new ParaCL::BinExpr(
            ParaCL::token_t::ISLS,
            std::unique_ptr<ParaCL::Expr>((yyvsp[-2].expr)),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
    }
#line 1775 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 45: /* relational_expression: relational_expression ISLSE additive_expression  */
#line 348 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                                  {
        (yyval.expr) = new ParaCL::BinExpr(
            ParaCL::token_t::ISLSE,
            std::unique_ptr<ParaCL::Expr>((yyvsp[-2].expr)),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
    }
#line 1787 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 46: /* additive_expression: multiplicative_expression  */
#line 358 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                              { (yyval.expr) = (yyvsp[0].expr); }
#line 1793 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 47: /* additive_expression: additive_expression ADD multiplicative_expression  */
#line 359 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                                  { 
        (yyval.expr) = new ParaCL::BinExpr(
            ParaCL::token_t::ADD,
            std::unique_ptr<ParaCL::Expr>((yyvsp[-2].expr)),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
    }
#line 1805 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 48: /* additive_expression: additive_expression SUB multiplicative_expression  */
#line 366 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                                  { 
        (yyval.expr) = new ParaCL::BinExpr(
            ParaCL::token_t::SUB,
            std::unique_ptr<ParaCL::Expr>((yyvsp[-2].expr)),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
    }
#line 1817 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 49: /* multiplicative_expression: unary_expression  */
#line 376 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                     { (yyval.expr) = (yyvsp[0].expr); }
#line 1823 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 50: /* multiplicative_expression: multiplicative_expression MUL unary_expression  */
#line 377 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                               { 
        (yyval.expr) = new ParaCL::BinExpr(
            ParaCL::token_t::MUL, 
            std::unique_ptr<ParaCL::Expr>((yyvsp[-2].expr)),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
    }
#line 1835 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 51: /* multiplicative_expression: multiplicative_expression DIV unary_expression  */
#line 384 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                               {
        (yyval.expr) = new ParaCL::BinExpr(
            ParaCL::token_t::DIV, 
            std::unique_ptr<ParaCL::Expr>((yyvsp[-2].expr)),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
    }
#line 1847 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 52: /* multiplicative_expression: multiplicative_expression REM unary_expression  */
#line 391 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                                               {
        (yyval.expr) = new ParaCL::BinExpr(
            ParaCL::token_t::REM,
            std::unique_ptr<ParaCL::Expr>((yyvsp[-2].expr)),
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
    }
#line 1859 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 53: /* unary_expression: factor  */
#line 401 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
           { (yyval.expr) = (yyvsp[0].expr); }
#line 1865 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 54: /* unary_expression: SUB unary_expression  */
#line 402 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                     {
        (yyval.expr) = new ParaCL::UnExpr(
            ParaCL::token_t::SUB,
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
    }
#line 1876 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 55: /* unary_expression: NOT unary_expression  */
#line 408 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                                     {
        (yyval.expr) = new ParaCL::UnExpr(
            ParaCL::token_t::NOT,
            std::unique_ptr<ParaCL::Expr>((yyvsp[0].expr))
        );
    }
#line 1887 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 56: /* factor: NUM  */
#line 417 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
        { 
        (yyval.expr) = new ParaCL::NumExpr((yyvsp[0].num_value));
    }
#line 1895 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 57: /* factor: VAR  */
#line 420 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
          { 
        (yyval.expr) = new ParaCL::VarExpr(*(yyvsp[0].str_value));
        delete (yyvsp[0].str_value);
    }
#line 1904 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 58: /* factor: LCIB expression RCIB  */
#line 424 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
                           { 
        (yyval.expr) = (yyvsp[-1].expr);
    }
#line 1912 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 59: /* factor: IN  */
#line 427 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
         {
        (yyval.expr) = new ParaCL::InputExpr();
    }
#line 1920 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 60: /* block: statements  */
#line 433 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
               {
        std::vector<std::unique_ptr<ParaCL::Stmt>> body_stmts;
        for (auto stmt : *(yyvsp[0].stmt_vector))
            body_stmts.push_back(std::unique_ptr<ParaCL::Stmt>(stmt));

        (yyval.block) = new ParaCL::BlockStmt(std::move(body_stmts));
        delete (yyvsp[0].stmt_vector); /* delete vector of statements, created in 'statements' rule */
    }
#line 1933 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;

  case 61: /* one_stmt_block: statement  */
#line 444 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"
              {
        std::vector<std::unique_ptr<ParaCL::Stmt>> body_stmts;
        body_stmts.push_back(std::unique_ptr<ParaCL::Stmt>((yyvsp[0].stmt)));
        (yyval.block) = new ParaCL::BlockStmt(std::move(body_stmts));
    }
#line 1943 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"
    break;


#line 1947 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/build/bison-out/parser.tab.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (&yylloc, YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 450 "/home/ananasik/projects/C++/vlados/5/ParaCL/1_version/src/frontend/parser/parser.y"


void yyerror(YYLTYPE* loc, const char* msg)
{
    std::cerr << current_file     << ":"
              << loc->first_line   << ":"
              << loc->first_column << ":"
              " paracl: error: " << msg << "\n";
}
