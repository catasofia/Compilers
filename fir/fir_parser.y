%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <memory>
#include <cstring>
#include <cdk/compiler.h>
#include <cdk/types/types.h>
#include "ast/all.h"
#define LINE                         compiler->scanner()->lineno()
#define yylex()                      compiler->scanner()->scan()
#define yyerror(compiler, s)         compiler->scanner()->error(s)
//-- don't change *any* of these --- END!
%}

%parse-param {std::shared_ptr<cdk::compiler> compiler}

%union {
  //--- don't change *any* of these: if you do, you'll break the compiler.
  YYSTYPE() : type(cdk::primitive_type::create(0, cdk::TYPE_VOID)) {}
  ~YYSTYPE() {}
  YYSTYPE(const YYSTYPE &other) { *this = other; }
  YYSTYPE& operator=(const YYSTYPE &other) { type = other.type; return *this; }

  std::shared_ptr<cdk::basic_type> type;        /* expression type */
  //-- don't change *any* of these --- END!

  int                   i;	/* integer value */
  float                 f;    /* float value   */
  std::string          *s;	/* symbol name or string literal */
  cdk::basic_node      *node;	/* node pointer */
  cdk::sequence_node   *sequence;
  fir::block_node      *block;
  fir::prologue_node   *prologue;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;
};

%token <i> tINTEGER
%token <s> tIDENTIFIER tSTRING
%token <f> tFLOAT
%token tOR tAND
%token tWHILE tDO tIF tPRINT tREAD tTHEN
%token tINTTYPE tPOINTERTYPE tFLOATTYPE tSTRINGTYPE tVOID tSIZEOF tNULL tLEAVE tRESTART tRETURN tWRITE tWRITELN 
%token tPROLOGUE tEPILOGUE tDEFAULTVALUE

%nonassoc ')'
%nonassoc tIFX
%nonassoc tELSE
%nonassoc tFINX
%nonassoc tFINALLY

%right '='
%left tOR
%left tAND
%nonassoc '~'
%left tNE tEQ
%left '<' tLE tGE '>' 
%left '+' '-'
%left '*' '/' '%'
%nonassoc '?'
%nonassoc tUNARY
%nonassoc '[' '('

%type <node> stmt program functiondecl param declaration fvar vardecl functiondef blockb prologueb epilogueb blockep problock proep probep
%type <sequence> list exprs params declarations fvars //vardecls
%type <expression> expr functioncall literal
%type <lvalue> lval
%type <block> block epilogue
%type <prologue> prologue
%type <s> string
%type <type> type

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

program :                                              { compiler->ast(new cdk::sequence_node(LINE));}
        | declarations                                 { compiler->ast($1); }
        ;

declarations : declaration                             { $$ = new cdk::sequence_node(LINE, $1); }
             | declarations declaration                { $$ = new cdk::sequence_node(LINE, $2, $1); }
             ;

declaration : vardecl ';'                              { $$ = $1; }
            | functiondecl                             { $$ = $1; }
            | functiondef                              { $$ = $1; } 
            ;

vardecl : type tIDENTIFIER                             { $$ = new fir::variable_decl_node(LINE, false, false, *$2, $1, nullptr); }
        | type tIDENTIFIER '=' expr                    { $$ = new fir::variable_decl_node(LINE, false, false, *$2, $1, $4); }
        | type '*' tIDENTIFIER                         { $$ = new fir::variable_decl_node(LINE, true, false, *$3, $1, nullptr); }
        | type '*' tIDENTIFIER '=' expr                { $$ = new fir::variable_decl_node(LINE, true, false, *$3, $1, $5); }
        | type '?' tIDENTIFIER                         { $$ = new fir::variable_decl_node(LINE, false, true, *$3, $1, nullptr); }
        ;

functiondecl : tVOID '*' tIDENTIFIER '(' params ')'    { $$ = new fir::function_declaration_node(LINE, true, false, cdk::primitive_type::create(0, cdk::TYPE_VOID), $3, $5); }
             | tVOID '?' tIDENTIFIER '(' params ')'    { $$ = new fir::function_declaration_node(LINE, false, true, cdk::primitive_type::create(0, cdk::TYPE_VOID), $3, $5); }
             | tVOID tIDENTIFIER '(' params ')'        { $$ = new fir::function_declaration_node(LINE, false, false, cdk::primitive_type::create(0, cdk::TYPE_VOID), $2, $4); }
             | type '*' tIDENTIFIER '(' params ')'     { $$ = new fir::function_declaration_node(LINE, true, false, $1, $3, $5); }
             | type '?' tIDENTIFIER '(' params ')'     { $$ = new fir::function_declaration_node(LINE, false, true, $1, $3, $5); }
             | type tIDENTIFIER '(' params ')'         { $$ = new fir::function_declaration_node(LINE, false, false, $1, $2, $4); }
             ;

functiondef : blockb                                   {$$ = $1;}
            | prologueb                                {$$ = $1;}
            | epilogueb                                {$$ = $1;}
            | blockep                                  {$$ = $1;}
            | problock                                 {$$ = $1;}
            | proep                                    {$$ = $1;}
            | probep                                   {$$ = $1;}
            ;

blockb : tVOID tIDENTIFIER '(' params ')' '{' block '}'                                                                                          { $$ = new fir::function_definition_node(LINE, false, false, $2, $4, $7, nullptr, nullptr, nullptr); }
       | tVOID '*' tIDENTIFIER '(' params ')' '{' block '}'                                                                                      { $$ = new fir::function_definition_node(LINE, true, false, $3, $5, $8, nullptr, nullptr, nullptr); }
       | tVOID '?' tIDENTIFIER '(' params ')' '{' block '}'                                                                                      { $$ = new fir::function_definition_node(LINE, false, true, $3, $5, $8, nullptr, nullptr, nullptr); }
       | type tIDENTIFIER '(' params ')' '{' block '}'                                                                                           { $$ = new fir::function_definition_node(LINE, $1 , false, false, $2, $4, $7, nullptr, nullptr, nullptr); }
       | type tIDENTIFIER '(' params ')' tDEFAULTVALUE literal '{' block '}'                                                                     { $$ = new fir::function_definition_node(LINE, $1, false, false, $2, $4, $9, nullptr, nullptr, $7); }
       | type '*' tIDENTIFIER '(' params ')' '{' block '}'                                                                                       { $$ = new fir::function_definition_node(LINE, $1, true, false, $3, $5, $8, nullptr, nullptr, nullptr); }          
       | type '*' tIDENTIFIER '(' params ')' tDEFAULTVALUE literal '{' block '}'                                                                 { $$ = new fir::function_definition_node(LINE, $1, true, false, $3, $5, $10, nullptr, nullptr, $8); }
       | type '?' tIDENTIFIER '(' params ')' '{' block '}'                                                                                       { $$ = new fir::function_definition_node(LINE, $1, false, true, $3, $5, $8, nullptr, nullptr, nullptr); }
       | type '?' tIDENTIFIER '(' params ')' tDEFAULTVALUE literal '{' block '}'                                                                 { $$ = new fir::function_definition_node(LINE, $1, false, true, $3, $5, $10, nullptr, nullptr, $8); }
       ;

prologueb :  tVOID tIDENTIFIER '(' params ')' tPROLOGUE prologue '}'                                                                             { $$ = new fir::function_definition_node(LINE, false, false, $2, $4, nullptr, nullptr, $7, nullptr); }
          | tVOID '*' tIDENTIFIER '(' params ')' tPROLOGUE prologue '}'                                                                          { $$ = new fir::function_definition_node(LINE, true, false, $3, $5, nullptr, nullptr, $8, nullptr); }
          | tVOID '?' tIDENTIFIER '(' params ')' tPROLOGUE prologue '}'                                                                          { $$ = new fir::function_definition_node(LINE, false, true, $3, $5, nullptr, nullptr, $8, nullptr); }
          | type tIDENTIFIER '(' params ')' tPROLOGUE prologue '}'                                                                               { $$ = new fir::function_definition_node(LINE, $1, false, false, $2, $4, nullptr, nullptr, $7, nullptr); }
          | type tIDENTIFIER '(' params ')' tDEFAULTVALUE literal tPROLOGUE prologue '}'                                                         { $$ = new fir::function_definition_node(LINE, $1, false, false, $2, $4, nullptr, nullptr, $9, $7); }
          | type '*' tIDENTIFIER '(' params ')' tPROLOGUE prologue '}'                                                                           { $$ = new fir::function_definition_node(LINE, $1, true, false, $3, $5, nullptr, nullptr, $8, nullptr); }
          | type '*' tIDENTIFIER '(' params ')' tDEFAULTVALUE literal tPROLOGUE prologue '}'                                                     { $$ = new fir::function_definition_node(LINE, $1, true, false, $3, $5, nullptr, nullptr, $10, $8); }
          | type '?' tIDENTIFIER '(' params ')' tPROLOGUE prologue '}'                                                                           { $$ = new fir::function_definition_node(LINE, $1, false, true, $3, $5, nullptr, nullptr, $8, nullptr); }
          | type '?' tIDENTIFIER '(' params ')' tDEFAULTVALUE literal tPROLOGUE prologue '}'                                                     { $$ = new fir::function_definition_node(LINE, $1, false, true, $3, $5, nullptr, nullptr, $10, $8); }     
          ;

epilogueb : tVOID tIDENTIFIER '(' params ')' tEPILOGUE '{' epilogue '}'                                                                          { $$ = new fir::function_definition_node(LINE, false, false, $2, $4, nullptr, $8, nullptr, nullptr); }
          | tVOID '*' tIDENTIFIER '(' params ')' tEPILOGUE '{' epilogue '}'                                                                      { $$ = new fir::function_definition_node(LINE, true, false, $3, $5, nullptr, $9, nullptr, nullptr); }
          | tVOID '?' tIDENTIFIER '(' params ')' tEPILOGUE '{' epilogue '}'                                                                      { $$ = new fir::function_definition_node(LINE, false, true, $3, $5, nullptr, $9, nullptr, nullptr); }
          | type tIDENTIFIER '(' params ')' tEPILOGUE '{' epilogue '}'                                                                           { $$ = new fir::function_definition_node(LINE, $1, false, false, $2, $4, nullptr, $8, nullptr, nullptr); }
          | type tIDENTIFIER '(' params ')' tDEFAULTVALUE literal tEPILOGUE '{' epilogue '}'                                                     { $$ = new fir::function_definition_node(LINE, $1, false, false, $2, $4, nullptr, $10, nullptr, $7); }
          | type '*' tIDENTIFIER '(' params ')' tEPILOGUE '{' epilogue '}'                                                                       { $$ = new fir::function_definition_node(LINE, $1, true, false, $3, $5, nullptr, $9, nullptr, nullptr); }
          | type '*' tIDENTIFIER '(' params ')' tDEFAULTVALUE literal tEPILOGUE '{' epilogue '}'                                                 { $$ = new fir::function_definition_node(LINE, $1, true, false, $3, $5, nullptr, $11, nullptr, $8); }
          | type '?' tIDENTIFIER '(' params ')' tEPILOGUE '{' epilogue '}'                                                                       { $$ = new fir::function_definition_node(LINE, $1, false, true, $3, $5, nullptr, $9, nullptr, nullptr); }
          | type '?' tIDENTIFIER '(' params ')' tDEFAULTVALUE literal tEPILOGUE '{' epilogue '}'                                                 { $$ = new fir::function_definition_node(LINE, $1, false, true, $3, $5, nullptr, $11, nullptr, $8); }
          ;
blockep : tVOID tIDENTIFIER '(' params ')' '{' block '}' tEPILOGUE '{' epilogue '}'                                                              { $$ = new fir::function_definition_node(LINE, false, false, $2, $4, $7, $11, nullptr, nullptr); }
        | tVOID '*' tIDENTIFIER '(' params ')' '{' block '}' tEPILOGUE '{' epilogue '}'                                                          { $$ = new fir::function_definition_node(LINE, true, false, $3, $5, $8, $12, nullptr, nullptr); }
        | tVOID '?' tIDENTIFIER '(' params ')' '{' block '}' tEPILOGUE '{' epilogue '}'                                                          { $$ = new fir::function_definition_node(LINE, false, true, $3, $5, $8, $12, nullptr, nullptr); }
        | type tIDENTIFIER '(' params ')' '{' block '}' tEPILOGUE '{' epilogue '}'                                                               { $$ = new fir::function_definition_node(LINE, $1, false, false, $2, $4, $7, $11, nullptr, nullptr); }
        | type tIDENTIFIER '(' params ')' tDEFAULTVALUE literal '{' block '}' tEPILOGUE '{' epilogue '}'                                         { $$ = new fir::function_definition_node(LINE, $1, false, false, $2, $4, $9, $13, nullptr, $7); }
        | type '*' tIDENTIFIER '(' params ')' '{' block '}' tEPILOGUE '{' epilogue '}'                                                           { $$ = new fir::function_definition_node(LINE, $1, true, false, $3, $5, $8, $12, nullptr, nullptr); }
        | type '*' tIDENTIFIER '(' params ')' tDEFAULTVALUE literal '{' block '}' tEPILOGUE '{' epilogue '}'                                     { $$ = new fir::function_definition_node(LINE, $1, true, false, $3, $5, $10, $14, nullptr, $8); }
        | type '?' tIDENTIFIER '(' params ')' '{' block '}' tEPILOGUE '{' epilogue '}'                                                           { $$ = new fir::function_definition_node(LINE, $1, false, true, $3, $5, $8, $12, nullptr, nullptr); }
        | type '?' tIDENTIFIER '(' params ')' tDEFAULTVALUE literal '{' block '}' tEPILOGUE '{' epilogue '}'                                     { $$ = new fir::function_definition_node(LINE, $1, false, true, $3, $5, $10, $14, nullptr, $8); }
        ;

problock : tVOID tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' '{' block '}'                                                                 { $$ = new fir::function_definition_node(LINE, false, false, $2, $4, $10, nullptr, $7, nullptr); }
         | tVOID '*' tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' '{' block '}'                                                             { $$ = new fir::function_definition_node(LINE, true, false, $3, $5, $11, nullptr, $8, nullptr); }
         | tVOID '?' tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' '{' block '}'                                                             { $$ = new fir::function_definition_node(LINE, false, true, $3, $5, $11, nullptr, $8, nullptr); }
         | type tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' '{' block '}'                                                                  { $$ = new fir::function_definition_node(LINE, $1, false, false, $2, $4, $10, nullptr, $7, nullptr); }
         | type tIDENTIFIER '(' params ')' tDEFAULTVALUE literal tPROLOGUE prologue '}' '{' block '}'                                            { $$ = new fir::function_definition_node(LINE, $1, false, false, $2, $4, $12, nullptr, $9, $7); }
         | type '*' tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' '{' block '}'                                                              { $$ = new fir::function_definition_node(LINE, $1, true, false, $3, $5, $11, nullptr, $8, nullptr); }
         | type '*' tIDENTIFIER '(' params ')' tDEFAULTVALUE literal tPROLOGUE prologue '}' '{' block '}'                                        { $$ = new fir::function_definition_node(LINE, $1, true, false, $3, $5, $13, nullptr, $10, $8); }
         | type '?' tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' '{' block '}'                                                              { $$ = new fir::function_definition_node(LINE, $1, false, true, $3, $5, $11, nullptr, $8, nullptr); }
         | type '?' tIDENTIFIER '(' params ')' tDEFAULTVALUE literal tPROLOGUE prologue '}' '{' block '}'                                        { $$ = new fir::function_definition_node(LINE, $1, false, true, $3, $5, $13, nullptr, $10, $8); }  
         ;

proep : tVOID tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' tEPILOGUE '{' epilogue '}'                                                       { $$ = new fir::function_definition_node(LINE, false, false, $2, $4, nullptr, $11, $7, nullptr); }
      | tVOID '*' tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' tEPILOGUE '{' epilogue '}'                                                   { $$ = new fir::function_definition_node(LINE, true, false, $3, $5, nullptr, $12, $8, nullptr); }
      | tVOID '?' tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' tEPILOGUE '{' epilogue '}'                                                   { $$ = new fir::function_definition_node(LINE, false, true, $3, $5, nullptr, $12, $8, nullptr); }
      | type tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' tEPILOGUE '{' epilogue '}'                                                        { $$ = new fir::function_definition_node(LINE, $1, false, false, $2, $4, nullptr, $11, $7, nullptr); }
      | type tIDENTIFIER '(' params ')' tDEFAULTVALUE literal tPROLOGUE prologue '}' tEPILOGUE '{' epilogue '}'                                  { $$ = new fir::function_definition_node(LINE, $1, false, false, $2, $4, nullptr, $13, $9, $7); }
      | type '*' tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' tEPILOGUE '{' epilogue '}'                                                    { $$ = new fir::function_definition_node(LINE, $1, true, false, $3, $5, nullptr, $12, $8, nullptr); }
      | type '*' tIDENTIFIER '(' params ')' tDEFAULTVALUE literal tPROLOGUE prologue '}' tEPILOGUE '{' epilogue '}'                              { $$ = new fir::function_definition_node(LINE, $1, true, false, $3, $5, nullptr, $14, $10, $8); }
      | type '?' tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' tEPILOGUE '{' epilogue '}'                                                    { $$ = new fir::function_definition_node(LINE, $1, false, true, $3, $5, nullptr, $12, $8, nullptr); }
      | type '?' tIDENTIFIER '(' params ')' tDEFAULTVALUE literal tPROLOGUE prologue '}' tEPILOGUE '{' epilogue '}'                              { $$ = new fir::function_definition_node(LINE, $1, false, true, $3, $5, nullptr, $14, $10, $8); }            
      ;

probep : tVOID tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' '{' block '}' tEPILOGUE '{' epilogue '}'                                        { $$ = new fir::function_definition_node(LINE, false, false, $2, $4, $10, $14, $7, nullptr); }
       | tVOID '*' tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' '{' block '}' tEPILOGUE '{' epilogue '}'                                    { $$ = new fir::function_definition_node(LINE, true, false, $3, $5, $11, $15, $8, nullptr); }
       | tVOID '?' tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' '{' block '}' tEPILOGUE '{' epilogue '}'                                    { $$ = new fir::function_definition_node(LINE, false, true, $3, $5, $11, $15, $8, nullptr); }
       | type tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' '{' block '}' tEPILOGUE '{' epilogue '}'                                         { $$ = new fir::function_definition_node(LINE, $1, false, false, $2, $4, $10, $14, $7, nullptr); }
       | type tIDENTIFIER '(' params ')' tDEFAULTVALUE literal tPROLOGUE prologue '}' '{' block '}' tEPILOGUE '{' epilogue '}'                   { $$ = new fir::function_definition_node(LINE, $1, false, false, $2, $4, $12, $16, $9, $7); }
       | type '*' tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' '{' block '}' tEPILOGUE '{' epilogue '}'                                     { $$ = new fir::function_definition_node(LINE, $1, true, false, $3, $5, $11, $15, $8, nullptr); }
       | type '*' tIDENTIFIER '(' params ')' tDEFAULTVALUE literal tPROLOGUE prologue '}' '{' block '}' tEPILOGUE '{' epilogue '}'               { $$ = new fir::function_definition_node(LINE, $1, true, false, $3, $5, $13, $17, $10, $8); }
       | type '?' tIDENTIFIER '(' params ')' tPROLOGUE prologue '}' '{' block '}' tEPILOGUE '{' epilogue '}'                                     { $$ = new fir::function_definition_node(LINE, $1, false, true, $3, $5, $11, $15, $8, nullptr); }
       | type '?' tIDENTIFIER '(' params ')' tDEFAULTVALUE literal tPROLOGUE prologue '}' '{' block '}' tEPILOGUE '{' epilogue '}'               { $$ = new fir::function_definition_node(LINE, $1, false, true, $3, $5, $13, $17, $10, $8); }
       ;

block: fvars                                           { $$ = new fir::block_node(LINE, $1, new cdk::sequence_node(LINE), false); }
     | fvars list                                      { $$ = new fir::block_node(LINE, $1, $2, false); }
     | list                                            { $$ = new fir::block_node(LINE, new cdk::sequence_node(LINE), $1, false); }
     |                                                 { $$ = new fir::block_node(LINE, new cdk::sequence_node(LINE), new cdk::sequence_node(LINE), false); }
     ;

epilogue: fvars                                        { $$ = new fir::block_node(LINE, $1, new cdk::sequence_node(LINE), true); }
        | fvars list                                   { $$ = new fir::block_node(LINE, $1, $2, true); }
        | list                                         { $$ = new fir::block_node(LINE, new cdk::sequence_node(LINE), $1, true); }
        |                                              { $$ = new fir::block_node(LINE, new cdk::sequence_node(LINE), new cdk::sequence_node(LINE), true); }
        ;

prologue: fvars                                        { $$ = new fir::prologue_node(LINE, $1, new::cdk::sequence_node(LINE)); }
        | fvars list                                   { $$ = new fir::prologue_node(LINE, $1, $2); }
        | list                                         { $$ = new fir::prologue_node(LINE, new::cdk::sequence_node(LINE), $1); }
        |                                              { $$ = new fir::prologue_node(LINE, new cdk::sequence_node(LINE), new cdk::sequence_node(LINE)); }
        ;

fvar : param                                           { $$  = $1; }
     | type tIDENTIFIER '=' expr                       { $$ = new fir::variable_decl_node(LINE, false, false, *$2, $1, $4); }
     ;

fvars : fvar ';'                                       { $$ = new cdk::sequence_node(LINE, $1); }
      | fvars fvar ';'                                 { $$ = new cdk::sequence_node(LINE, $2, $1); }
      ;

param: type tIDENTIFIER                                {$$ = new fir::variable_decl_node(LINE, false, false, *$2, $1, nullptr);}
     ;

params: param                                          { $$ = new cdk::sequence_node(LINE, $1); }
      | params ',' param                               { $$ = new cdk::sequence_node(LINE, $3, $1); }
      |                                                { $$ = new cdk::sequence_node(LINE); }
      ;

type : tINTTYPE                                        { $$ = cdk::primitive_type::create(4, cdk::TYPE_INT); }
     | tFLOATTYPE                                      { $$ = cdk::primitive_type::create(8, cdk::TYPE_DOUBLE); }
     | tSTRINGTYPE                                     { $$ = cdk::primitive_type::create(4, cdk::TYPE_STRING); }
     | tPOINTERTYPE '<' type '>'                       { $$ = cdk::reference_type::create(4, $3); } 
     | '<' type '>'                                    { $$ = cdk::reference_type::create(4, $2); } 
     ;

list : stmt	                                        { $$ = new cdk::sequence_node(LINE, $1); }
	  | list stmt                                     { $$ = new cdk::sequence_node(LINE, $2, $1); }
	  ;

stmt : expr ';'                                        { $$ = new fir::evaluation_node(LINE, $1); }
     | tWRITE exprs ';'                                { $$ = new fir::write_node(LINE, $2, false); }
     | tWRITELN exprs ';'                              { $$ = new fir::write_node(LINE, $2, true); }
     | tREAD lval ';'                                  { $$ = new fir::read_node(LINE, $2); }
     | tRETURN                                         { $$ = new fir::return_node(LINE); }
     | tLEAVE ';'                                      { $$ = new fir::leave_node(LINE); }
     | tRESTART ';'                                    { $$ = new fir::restart_node(LINE); }
     | tLEAVE tINTEGER ';'                             { $$ = new fir::leave_node(LINE, $2); }
     | tRESTART tINTEGER ';'                           { $$ = new fir::restart_node(LINE, $2); }
     | tIF expr tTHEN stmt %prec tIFX                  { $$ = new fir::if_node(LINE, $2, $4); }
     | tIF expr tTHEN stmt tELSE stmt                  { $$ = new fir::if_else_node(LINE, $2, $4, $6); }
     | tWHILE expr tDO stmt %prec tFINX                { $$ = new fir::while_do_node(LINE, $2, $4); }
     | tWHILE expr tDO stmt tFINALLY stmt              { $$ = new fir::while_do_finally_node(LINE, $2, $4, $6); }
     | '{' block '}'                                   { $$ = $2; }
     ;

expr : literal                                         { $$ = $1; }
     | functioncall                                    { $$ = $1; }   
     | '+' expr %prec tUNARY                           { $$ = new fir::identity_node(LINE, $2); }
     | '-' expr %prec tUNARY                           { $$ = new cdk::neg_node(LINE, $2); }
     | '~' expr                                        { $$ = new cdk::not_node(LINE, $2); }
     | expr '+' expr	                              { $$ = new cdk::add_node(LINE, $1, $3); }
     | expr '-' expr	                              { $$ = new cdk::sub_node(LINE, $1, $3); }
     | expr '*' expr	                              { $$ = new cdk::mul_node(LINE, $1, $3); }
     | expr '/' expr	                              { $$ = new cdk::div_node(LINE, $1, $3); }
     | expr '%' expr	                              { $$ = new cdk::mod_node(LINE, $1, $3); }
     | expr '<' expr	                              { $$ = new cdk::lt_node(LINE, $1, $3); }
     | expr '>' expr	                              { $$ = new cdk::gt_node(LINE, $1, $3); }
     | expr tGE expr	                              { $$ = new cdk::ge_node(LINE, $1, $3); }
     | expr tLE expr                                   { $$ = new cdk::le_node(LINE, $1, $3); }
     | expr tNE expr	                              { $$ = new cdk::ne_node(LINE, $1, $3); }
     | expr tEQ expr	                              { $$ = new cdk::eq_node(LINE, $1, $3); }
     | expr tAND expr                                  { $$ = new cdk::and_node(LINE, $1, $3); }
     | expr tOR expr                                   { $$ = new cdk::or_node(LINE, $1, $3); } 
     | lval                                            { $$ = new cdk::rvalue_node(LINE, $1); }  //FIXME
     | tSIZEOF '(' expr ')'                            { $$ = new fir::size_of_node(LINE, $3); }
     | lval '=' expr                                   { $$ = new cdk::assignment_node(LINE, $1, $3); }
     | lval '?'                                        { $$ = new fir::address_node(LINE, $1); }
     | '[' expr ']'                                    { $$ = new fir:: mem_alloc_node(LINE, $2); }
     | '(' expr ')'                                    { $$ = $2; }
     ;

exprs : expr                                           { $$ = new cdk::sequence_node(LINE, $1); }
      | expr ',' exprs                                 { $$ = new cdk::sequence_node(LINE, $1, $3); }
      ;

functioncall : tIDENTIFIER '(' ')'                     { $$ = new fir::function_call_node(LINE, *$1, new cdk::sequence_node(LINE)); }
             | tIDENTIFIER '(' exprs ')'               { $$ = new fir::function_call_node(LINE, *$1, $3); }
             ;

literal : tINTEGER                                     { $$ = new cdk::integer_node(LINE, $1); }
        | tFLOAT                                       { $$ = new cdk::double_node(LINE, $1); }
        | tNULL                                        { $$ = new fir::null_node(LINE); }
	   | string                                       { $$ = new cdk::string_node(LINE, $1); }
        ;

string : tSTRING                                       { $$ = $1; }
       | string tSTRING                                { $$ = $1; $$->append(*$2); delete $2; }
       ;

lval : expr '[' expr ']'                               { $$ = new fir::index_node(LINE, $1, $3);}
     | tIDENTIFIER                                     { $$ = new cdk::variable_node(LINE, $1); }
     ;

%%
