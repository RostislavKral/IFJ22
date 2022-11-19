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
    ISEOF          //  8
    //TBD
};


enum T_OPERATOR {
    EQUALS,     // 0
    PLUS,       // 1
    MINUS,      //
};



enum T_KEYWORD {
    function,
    while_loop,
    do_while_loop,
    predialog, // WTF is this

};


struct T_VALUE
{
    int type; // 0 - int, 1 - char *, 2 - double
    double double_val;
    int int_val;
    char *char_val;
};


 /* Don't use this
struct T_TOKEN {

    enum T_TOKEN_TYPE type;
    char* name;
    struct T_VALUE value;

};
*/


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
typedef struct {

    enum T_TOKEN_TYPE type;
    enum T_KEYWORD keyword;
    enum T_OPERATOR operator;
    char* name;
    struct T_VALUE value;

} TOKEN_T;

#endif