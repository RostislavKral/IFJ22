#ifndef TOKEN_H
#define TOKEN_H


enum T_TOKEN_TYPE
{
    KEYWORD,        //  0
    TOKEN_ID,       //  1
    FUNC_ID,        //  2
    LITERAL,        //  3
    ASSIGN,         //  4
    LPAR,           //  5
    RPAR,           //  6
    OPERATOR,       //  7
    ISEOF,          //  8
    PROG_START,     //  9   <?php
    SEMICOLON,      //  10
    //TBD
};


enum T_OPERATOR {
    MULTIPLY,       //0
    DIVIDE,         //1
    PLUS,           //2
    MINUS,          //3
    CONCAT,         //4
    LESSER,         //5
    GREATER,        //6
    LESSER_EQUAL,   //7
    GREATER_EQUAL,  //8
    EQUALS,         //9
    NOT_EQUAL       //10
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
    KEY_VOID        //9
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
    enum T_TOKEN_TYPE type;
    enum T_KEYWORD keyword;
    enum T_OPERATOR operators;
    char* name;
    struct T_VALUE value;
} TOKEN_T;

TOKEN_T * init_token(struct T_VALUE,enum T_KEYWORD,enum T_OPERATOR,char* name,struct T_VALUE);
#endif