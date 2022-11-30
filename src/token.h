#ifndef TOKEN_H
#define TOKEN_H
#include "stdlib.h"
#include "stdio.h"

enum T_TOKEN_TYPE
{
    KEYWORD,        //  0
    TOKEN_ID,       //  1   Variable
    FUNC_ID,        //  2   function name
    LITERAL,        //  3   cilo, pismenko
    ASSIGN,         //  4
    LPAR,           //  5 (
    RPAR,           //  6 )
    OPERATOR,       //  7
    ISEOF,          //  8   EOF
    PROG_START,     //  9   <?php
    SEMICOLON,      //  10
    LBRACE,         //  11
    RBRACE,         //  12
    COMMA,          //  13
    DATA_TYPE,      //  14

    //TBD
};


enum T_OPERATOR {
    MULTIPLY,           //0  *
    DIVIDE,             //1  /
    PLUS,               //2  +
    MINUS,              //3  -
    CONCAT,             //4  .
    LESS,               //5  <
    GREATER,            //6  >
    LESS_EQUAL,         //7  <=
    GREATER_EQUAL,      //8  >=
    EQUALS,             //9  ==
    NOT_EQUAL,          //10 !=
    TYPE_EQUALS,        //11 ===
    TYPE_NOT_EQUALS,    //12 !==
};



enum T_KEYWORD {
    KEY_ELSE,       //0  
    KEY_FLOAT,      //1  
    KEY_FUNCTION,   //2  
    KEY_IF,         //3  
    KEY_INT,        //4  
    KEY_NULL,       //5  
    KEY_RETURN,     //6  
    KEY_STRING,     //7  
    KEY_WHILE_LOOP, //8  
    KEY_VOID,       //9  
    KEY_BEGIN,      //10 <?php
    KEY_COLON       //11 :
};


struct T_VALUE
{
    int type; // 0 - int, 1 - char *, 2 - double
    double double_val;
    int int_val;
    char *char_val;
};

/**
 * @brief Token type
 * 
 * TYPE
 * 
 * KEYWORD      -> keyword
 * TOKEN_ID     -> name
 * FUNC_ID      -> name
 * STR_LIT      -> value
 * NUM_LIT      -> value
 * ASSIGN       ->
 * LPAR         ->
 * RPAR         ->
 * OPERATOR     ->
 * ISEOF        ->
 */
typedef struct{
    int lineNum;
    int charNum;
    enum T_TOKEN_TYPE type;
    enum T_KEYWORD keyword;
    enum T_OPERATOR operators;
    char* name;
    struct T_VALUE value;
} TOKEN_T;

//ERR HANDLING
enum ERROR_CODE{
    LEXICAL_ERR,            //1 - chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému)
    SYNTAX_ERR,             //2 - chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu, chybějící hlavička, atp.
    SEM_F_DECLARATION_ERR,  //3 - sémantická chyba v programu – nedefinovaná funkce, pokus o redefinice funkce.
    SEM_F_CALL_PARAM_ERR,   //4 - sémantická/běhová chyba v programu – špatný počet/typ parametrů u volání funkce či typ návratové hodnoty z funkce
    SEM_UNDEF_VAR_ERR,      //5 - sémantická chyba v programu – použití nedefinované proměnné.
    SEM_F_RETURN_VAL_ERR,   //6 - sémantická/běhová chyba v programu – chybějící/přebývající výraz v příkazu návratu z funkce.
    SEM_MATH_ERR,           //7 - sémantická/běhová chyba typové kompatibility v aritmetických, řetězcových arelačních výrazech.
    SEM_OTHER,              //8 - ostatní sémantické chyby.
    GENERAL_ERR             //99  interní chyba překladače tj. neovlivněná vstupním programem (např. chyba alo-kace paměti atd.).
};

void exit_with_message(int lineNum, int charNum, char* message, enum ERROR_CODE errorCode){
    fprintf(stderr, "ERR: line: %d, col: %d, message: %s", lineNum,charNum,message);
    switch (errorCode) {
        case LEXICAL_ERR:
            exit(1);
        case SYNTAX_ERR:
            exit(2);
        case SEM_F_DECLARATION_ERR:
            exit(3);
        case SEM_F_CALL_PARAM_ERR:
            exit(4);
        case SEM_UNDEF_VAR_ERR:
            exit(5);
        case SEM_F_RETURN_VAL_ERR:
            exit(6);
        case SEM_MATH_ERR:
            exit(7);
        case SEM_OTHER:
            exit(8);
        case GENERAL_ERR:
            exit(99);
    }
}
#endif