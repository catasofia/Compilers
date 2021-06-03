%option c++ prefix="fir_scanner_" outfile="fir_scanner.cpp"
%option stack noyywrap yylineno 8bit
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "fir_parser.tab.h"

// don't change this
#define yyerror LexerError
%}

SPACE                                           [ \r\t\n]

%x X_STRING X_COMMENT X_STRING_SPECIAL X_STR_IGNORE
%%


">="                                            return tGE; 
"<="                                            return tLE; 
"=="                                            return tEQ; 
"!="                                            return tNE; 
"&&"                                            return tAND;
"||"                                            return tOR;

"@"{SPACE}"{"                                   return tPROLOGUE;
">>"                                            return tEPILOGUE;
"@"                                             return tREAD;
"->"                                            return tDEFAULTVALUE;

"while"                                         return tWHILE;
"do"                                            return tDO;
"if"                                            return tIF; 
"then"                                          return tTHEN;
"else"                                          return tELSE; 

"int"                                           return tINTTYPE;
"float"                                         return tFLOATTYPE;
"string"                                        return tSTRINGTYPE;
"void"                                          return tVOID;
"sizeof"                                        return tSIZEOF;
"null"                                          return tNULL;
"leave"                                         return tLEAVE;
"restart"                                       return tRESTART;
"return"                                        return tRETURN;
"write"                                         return tWRITE;
"writeln"                                       return tWRITELN;
"finally"                                       return tFINALLY;

[A-Za-z][A-Za-z0-9_]*                           yylval.s = new std::string(yytext); return tIDENTIFIER;

\'                                              yy_push_state(X_STRING); yylval.s = new std::string("");
<X_STRING,X_STR_IGNORE>\'                       yy_pop_state(); return tSTRING;
<X_STRING>~                                     yy_push_state(X_STRING_SPECIAL); 
<X_STRING>.                                     *yylval.s += yytext;
<X_STRING>~n                                    *yylval.s += "\n";

<X_STRING_SPECIAL>0                             yy_pop_state(); yy_pop_state(); yy_push_state(X_STR_IGNORE);
<X_STRING_SPECIAL>00                            yy_pop_state(); yy_pop_state(); yy_push_state(X_STR_IGNORE);
<X_STRING_SPECIAL>n                             *yylval.s += "\n"; yy_pop_state();
<X_STRING_SPECIAL>r                             *yylval.s += "\r"; yy_pop_state();
<X_STRING_SPECIAL>t                             *yylval.s += "\t"; yy_pop_state();
<X_STRING_SPECIAL>~'                            *yylval.s += "\'"; yy_pop_state();
<X_STRING_SPECIAL>[0-9A-Fa-f][0-9A-Fa-f]?       *yylval.s += std::string(1, strtol(yytext, nullptr,16)); yy_pop_state();


<X_STR_IGNORE>~'                                ;
<X_STR_IGNORE>~~                                ;
<X_STR_IGNORE>.|~n                              ;


[0-9]+"."[0-9]+                                 yylval.f = strtof(yytext, nullptr); if(errno == ERANGE) yyerror("Overflow!"); return tFLOAT;
[0-9]+"."[0-9]+[eE][-+]?[0-9]+                  yylval.f = strtof(yytext, nullptr); if(errno == ERANGE) yyerror("Overflow!"); return tFLOAT;
[0-9]+[eE][-+]?[0-9]+                           yylval.f = strtof(yytext, nullptr); if(errno == ERANGE) yyerror("Overflow!"); return tFLOAT;

[0][0-7]+                                       yylval.i = strtol(yytext, nullptr, 8); if(errno == ERANGE) yyerror("Overflow!"); return tINTEGER; 
[1-9][0-9]*|0                                   yylval.i = strtol(yytext, nullptr, 10); if(errno == ERANGE) yyerror("Overflow!"); return tINTEGER;                                        

"(*"                                            yy_push_state(X_COMMENT);
<X_COMMENT>"*)"                                 yy_pop_state();
<X_COMMENT>.|~n                                 ;

"!!".*$                                         ;

[-%\]{}()<>*/~+&|[;?,=.]                      return *yytext;

[ \r\t\n]+                                      ; /* ignore whitespace */

.                                               yyerror("Unknown character");

%%
