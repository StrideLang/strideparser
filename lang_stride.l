%{
#include <cstdio>
#include <cstdlib>
#include "lang_stride.parser.hpp"
#define YY_DECL extern "C" int yylex()

using namespace std;

char * buffer;
void recordComment(int line, int column, const char *text);
// Copy of a number without its '_' digit separators
char *stripSeparators(const char *text);
int yycolumn = 1;
#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;

%}

%option noyywrap
%option nodefault
%option yylineno

DIGIT       [0-9]
LETTER      [a-z]
CLETTER     [A-Z]
HEXLETTER   [A-Fa-f]
DIGITS      {DIGIT}(_?{DIGIT})*
HEXDIGITS   ({DIGIT}|{HEXLETTER})(_?({DIGIT}|{HEXLETTER}))*
BINDIGITS   [01](_?[01])*
EXPONENT    [eE][+-]?{DIGITS}
INTSUFFIX   ([iu](8|16|32|64)|u|usize)
FLOATSUFFIX f(32|64)

%%
"("     |
")"     |
"{"     |
"}"     |
"["     |
"]"     |
"+"     |
"-"     |
"*"     |
"/"     { return yytext[0]; }

","     { return COMMA; }
"."     { return DOT; }
":"     { return COLON; }
"::"    { return COLONCOLON; }
";"     { return SEMICOLON; }
">>"    { return STREAM; }

"&&"    { return AND; }
"||"    { return OR; }
"!"     { return NOT; }

"&"     { return BITAND; }
"|"     { return BITOR; }
"~"     { return BITNOT; }
"@"     { return AT; }

"use"           { return USE; }
"package"       { return PACKAGE; }
"version"       { return VERSION; }
"with"          { return WITH; }
"import"        { return IMPORT; }
"as"            { return AS; }
"for"           { return FOR; }
"in"            { return IN; }
"none"          { return NONE; }
"on"            { return ON; }
"off"           { return OFF; }

"and"           { return AND; }
"or"            { return OR; }
"not"           { return NOT; }

"streamRate"    { return STREAMRATE; }

".."            { return DOTDOT; }
{DIGITS}/".."   { char *digits = stripSeparators(yytext); yylval.ival = atoll(digits); free(digits); return INT; }

{DIGITS}\.({DIGITS})?{EXPONENT}?   |
\.{DIGITS}{EXPONENT}?              |
{DIGITS}{EXPONENT}                 { char *digits = stripSeparators(yytext); yylval.fval = strtod(digits, NULL); free(digits); return REAL; }
0[xX]{HEXDIGITS}                   { yylval.sval = stripSeparators(yytext); return HEX; }
0[bB]{BINDIGITS}                   { yylval.sval = stripSeparators(yytext); return BIN; }
{DIGITS}                           { char *digits = stripSeparators(yytext); yylval.ival = atoll(digits); free(digits); return INT; }

 /* Literals with a type suffix (255u8, 0x1Fu8, 0.5f32). The grammar splits
    off and checks the suffix. Hex literals can't take a float suffix: in
    0x1f32 the f is a hex digit. */
{DIGITS}{INTSUFFIX}                |
0[xX]{HEXDIGITS}{INTSUFFIX}        |
0[bB]{BINDIGITS}{INTSUFFIX}        |
{DIGITS}{FLOATSUFFIX}              |
{DIGITS}\.({DIGITS})?{EXPONENT}?{FLOATSUFFIX} |
\.{DIGITS}{EXPONENT}?{FLOATSUFFIX} |
{DIGITS}{EXPONENT}{FLOATSUFFIX}    { yylval.sval = strdup(yytext); return NUMLIT; }

(_)*{CLETTER}({LETTER}|{CLETTER}|{DIGIT}|_)*    { yylval.sval = strdup(yytext); return UVAR; }
(_)*{LETTER}({LETTER}|{CLETTER}|{DIGIT})*       { yylval.sval = strdup(yytext); return WORD; }

'[^']*'             {   buffer = strdup(yytext);
                        buffer[strlen(buffer)-1] = '\0';
                        yylval.sval = strdup(buffer + 1);
                        free(buffer);
                        return STRING; }

\"[^\"]*\"          {   buffer = strdup(yytext);
                        buffer[strlen(buffer)-1] = '\0';
                        yylval.sval = strdup(buffer + 1);
                        free(buffer);
                        return STRING; }

[ \t]+              {   /* Skip spaces and tabs */ }
\r?\n               {   yycolumn = 1; /* Reset column on newline */ }
"#".*               {   recordComment(yylloc.first_line, yylloc.first_column, yytext); }
.                   {   yylval.sval = strdup(yytext);
                        return ERROR; }
%%

char *stripSeparators(const char *text) {
    char *out = strdup(text);
    char *dest = out;
    for (const char *c = text; *c; c++) {
        if (*c != '_') {
            *dest++ = *c;
        }
    }
    *dest = '\0';
    return out;
}
