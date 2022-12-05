/**
 * @file syntax_and_semantic_analyzer.c
 * @author Nikita Sniehovskyi xsnieh00
 * @author Lukáš Ježek <xjezek19@stud.fit.vutbr.cz>
 * @brief syntax and semantic analyzer implementation
 * @version 0.1
 * @date 2022-11-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "syntax_and_semantic_analyzer.h"
#include "lexer.h"
#include "codegen.h"

parseFunctionHelper functionHelper = {
        .fParsing = false,
        .fNamePass = false,
        .fParamPass = false,
        .fBodyParsing = false,
        .fHeadParsed = false,
        .fParamCount = 0,
        .fReturnTypePass = false,
        .fBraceCountCheck = 0,
        .paramsList = NULL
};
scopeHelper scope = {
        .num = 0,
        .openedBracesCount = 0,
        .lastScopeOpeningToken = NULL,
        .isDefined = false,
        .openedIfCount = 0
};

bool is_token_eof(TOKEN_T* token){
    if(token->type == ISEOF){
        fprintf(stderr, "%s", "EOF");
        return true;
    } else{
        return false;
    }
}

void function_end_parsing(){
    functionHelper.fParsing = false;
    functionHelper.fNamePass = false;
    functionHelper.fParamPass = false;
    functionHelper.fBodyParsing = false;
    functionHelper.fHeadParsed = false;
    functionHelper.fParamCount = 0;
    functionHelper.fReturnTypePass = false;
    functionHelper.fBraceCountCheck = 0;
    functionHelper.paramsList = NULL;
    scope.num--;
    //scope.openedBracesCount--;
};

void while_condition(TOKEN_T* token, htab_t* symtable){
    DLList* condExpList = expression_list(symtable, RPAR);
    if(condExpList->first == NULL) exit_with_message(token->lineNum,token->charNum,"Invalid expression in condition", SYNTAX_ERR);
    BSTnode* condExprBST = analyze_precedence(condExpList);
    if (condExprBST == NULL) exit_with_message(token->lineNum, token->charNum, "Invalid expression condition", SEM_MATH_ERR);
    scope.num++;
    gen_if(condExprBST);
}

void if_condition(TOKEN_T* token, htab_t* symtable){
    DLList* condExpList = expression_list(symtable, RPAR);
    if(condExpList->first == NULL) exit_with_message(token->lineNum,token->charNum,"Invalid expression in condition", SYNTAX_ERR);
    BSTnode* condExprBST = analyze_precedence(condExpList);
    if (condExprBST == NULL) exit_with_message(token->lineNum, token->charNum, "Invalid expression condition", SEM_MATH_ERR);
    scope.openedIfCount++;
    gen_if(condExprBST);
}

DLList *expression_list(htab_t* symtable, enum T_TOKEN_TYPE closingToken){
    DLList *precedenceList = malloc(sizeof (struct DLList));
    TOKEN_T *tmpToken = get_next_token();
    while (tmpToken->type != closingToken){
        if (tmpToken->type == LITERAL || tmpToken->type == TOKEN_ID || tmpToken->type == OPERATOR || (tmpToken->keyword == KEY_NULL && tmpToken->type == KEYWORD)){
            if (tmpToken->type == TOKEN_ID){
                htab_item_t *item = htab_find_var(symtable, tmpToken->name, scope.num);
                if (item == NULL) exit_with_message(tmpToken->lineNum, tmpToken->charNum, "Undefined variable", SEM_UNDEF_VAR_ERR);
            }
            DLL_insert_last(precedenceList, tmpToken);
            tmpToken = get_next_token();
        }
        else exit_with_message(tmpToken->lineNum, tmpToken->charNum, "Invalid token in assign or EOF", SYNTAX_ERR);
    }
    return precedenceList;
}

void var_declaration(htab_t* symtable, TOKEN_T *varNameToken){
    TOKEN_T *equalToken = get_next_token();
    if (equalToken->type != ASSIGN) exit_with_message(equalToken->lineNum, equalToken->charNum, "Invalid operator to assing", SYNTAX_ERR);

    DLList* precedenceList = expression_list(symtable, SEMICOLON);
    BSTnode *expressionTree = analyze_precedence(precedenceList);

    if (expressionTree != NULL){
        //one element
        int a = scope.num;
        if (expressionTree->token->type == LITERAL){
            if (expressionTree->token->value.type == 0){
                htab_value val =  {.int_value = expressionTree->token->value.int_val};
                if(!htab_insert_var(symtable, varNameToken->name, scope.num, expressionTree->type, val)) exit_with_message(varNameToken->lineNum, varNameToken->charNum, "Insert to symtable failed", GENERAL_ERR);
            } else if (expressionTree->token->value.type == 1){
                htab_value val =  {.str_value = expressionTree->token->value.char_val};
                if(!htab_insert_var(symtable, varNameToken->name, scope.num, expressionTree->type, val)) exit_with_message(varNameToken->lineNum, varNameToken->charNum, "Insert to symtable failed", GENERAL_ERR);
            } else if (expressionTree->token->value.type == 2){
                htab_value val =  {.float_value = expressionTree->token->value.double_val};
                if(!htab_insert_var(symtable, varNameToken->name, scope.num, expressionTree->type, val)) exit_with_message(varNameToken->lineNum, varNameToken->charNum, "Insert to symtable failed", GENERAL_ERR);
            }
        } else {
            //tree
            htab_value zero = {.str_value = NULL};
            if(!htab_insert_var(symtable, varNameToken->name, scope.num, expressionTree->type, zero)) exit_with_message(varNameToken->lineNum, varNameToken->charNum, "Insert to symtable failed", GENERAL_ERR);
        }
        gen_expression(varNameToken, expressionTree);
    }
}

void function_call(TOKEN_T *functionToken,htab_t* symtable){
    htab_item_t* stFunction = htab_find_func(symtable, functionToken->name);

    if (stFunction == NULL) exit_with_message(functionToken->lineNum, functionToken->charNum,"Function doesnt exist", SEM_F_DECLARATION_ERR);
    TOKEN_T *pars = get_next_token();

    if (pars->type != LPAR) exit_with_message(pars->lineNum, pars->charNum, "Expected '('", SYNTAX_ERR);
    TOKEN_T *paramArr[stFunction->data.params_count];

    for (int i = 1; i < stFunction->data.params_count+1; ++i) {
        TOKEN_T *tmp = get_next_token();
        if (tmp->type != TOKEN_ID && tmp->type != LITERAL) exit_with_message(tmp->lineNum, tmp->charNum, "invalid token", SEM_F_DECLARATION_ERR);
        printf("param: %d", tmp->value.type);
        if (tmp->type == TOKEN_ID){
            htab_item_t *param = htab_find_var(symtable,tmp->name, scope.num);
            if(stFunction->data.data_type[i] == 0 && param->data.data_type == KEY_INT ||
               stFunction->data.data_type[i] == 1 && param->data.data_type == KEY_STRING ||
               stFunction->data.data_type[i] == 2 && param->data.data_type == KEY_FLOAT
                    ) paramArr[i] = tmp;
            else exit_with_message(tmp->lineNum, tmp->charNum, "Invalid data type", SEM_F_DECLARATION_ERR);
        } else {
            printf("\n%u\n",stFunction->data.data_type[i]);
            if(tmp->value.type == 0 && stFunction->data.data_type[i] == KEY_INT ||
               tmp->value.type == 1 && stFunction->data.data_type[i] == KEY_STRING ||
               tmp->value.type == 2 && stFunction->data.data_type[i] == KEY_FLOAT
                    ) paramArr[i] = tmp;
            else exit_with_message(tmp->lineNum, tmp->charNum, "Invalid data type2", SEM_F_DECLARATION_ERR);
        }
    }
    //TODO code gen
}


void function_detected(TOKEN_T* initToken, htab_t* symtable){
    //F already declared
    if(initToken->type == FUNC_ID && htab_find_func(symtable,initToken->name) != NULL){
        exit_with_message(initToken->lineNum,initToken->charNum, "Function is already declared", SEM_F_DECLARATION_ERR);
    }
    //catch declaration of f in f
    if((functionHelper.fParsing == true && initToken->type == FUNC_ID) || (functionHelper.fParsing == true && initToken->keyword == KEY_FUNCTION)){
        exit_with_message(initToken->lineNum, initToken->charNum, "already parsing another function", SEM_F_DECLARATION_ERR);
        return;
    } else if(functionHelper.fParsing == true && initToken->type == RBRACE) {
        function_end_parsing();
        return;
    }
    functionHelper.fParsing = true;
    functionHelper.fParamCount = 0;
    //get f name and params
    int whilecount = 0;
    while (functionHelper.fHeadParsed == false){
        TOKEN_T *token = initToken;
        if(whilecount > 0){
            token = get_next_token();
        }
        //skip first cycle,
        whilecount++;
        //check eof and declaration of F inside Fname
        if(is_token_eof(token)){
            //exit msg, found keyword
            exit_with_message(token->lineNum, token->charNum, "Invalid token EOF ", GENERAL_ERR);
        }
        if(functionHelper.fNamePass == false){
            //get f name if not exist, expecting id
            if(token->type != FUNC_ID){
                exit_with_message(token->lineNum,token->charNum,"Invalid function name", SEM_F_DECLARATION_ERR);
            } else {
                functionHelper.name = token->name;
                functionHelper.fNamePass = true;
                continue;
            }
        }
        //get f params if not exist, expecting "(param1, param2, ...)"
        if(functionHelper.fParamPass == false){
            //if ( is found, cycle until )
            if(token->type == LPAR){
                TOKEN_T *paramToken = get_next_token();
                while (functionHelper.fParamPass == false){
                    if(paramToken->keyword == KEY_INT || paramToken->keyword == KEY_STRING || paramToken->keyword == KEY_FLOAT){
                        TOKEN_T *paramName = get_next_token();
                        if(paramName->keyword){
                            //Err 2 keyword in row
                            exit_with_message(paramToken->lineNum, paramToken->charNum, "Invalid token 2 keywords in row", SEM_F_DECLARATION_ERR);
                        } else {
                            if (functionHelper.paramsList == NULL) {
                                functionHelper.paramsList = malloc(sizeof (struct DLList));
                                DLL_init(functionHelper.paramsList);
                            }
                            DLL_insert_last(functionHelper.paramsList, paramToken);
                        }
                    } else if (paramToken->type == COMMA) {
                        functionHelper.fParamCount++;
                    } else {
                        //ERR invalid token expecting dataType
                        exit_with_message(paramToken->lineNum, paramToken->charNum, "invalid token expecting $int,$string,$float", SYNTAX_ERR);
                    }
                    paramToken = get_next_token();
                    if(paramToken->type == RPAR){
                        functionHelper.fParamPass = true;
                        token = get_next_token();
                        break;
                    }
                }
            } else {
                //INVALID TOKEN expecting (
                exit_with_message(token->lineNum, token->charNum,"Invalid token expecting ')'",SYNTAX_ERR);
            }
        }
        if(functionHelper.fReturnTypePass == false){
            if(token->keyword == KEY_COLON){
                TOKEN_T *returnTypeToken = get_next_token();
                if(returnTypeToken->keyword == KEY_STRING || returnTypeToken->keyword == KEY_INT || returnTypeToken->keyword == KEY_FLOAT){
                    functionHelper.returnType = returnTypeToken->keyword;
                    functionHelper.fReturnTypePass = true;
                    continue;
                } else {
                    exit_with_message(returnTypeToken->lineNum, returnTypeToken->charNum, "Invalid function return type", SYNTAX_ERR);
                }
            }
        }
        if(token->type == LBRACE && functionHelper.fNamePass && functionHelper.fParamPass){
            scope.lastScopeOpeningToken = token;
            scope.num++;
            //scope.openedBracesCount++;
            functionHelper.fHeadParsed = true;
            functionHelper.fBodyParsing = true;
            functionHelper.fBraceCountCheck = scope.openedBracesCount;
        } else {
            exit_with_message(token->lineNum, token->charNum, "declaration error", SYNTAX_ERR);
        }
    }
    //htab_insert_func(symtable)
    enum T_KEYWORD paramArr[functionHelper.fParamCount+1];
    DLLItem *tmp = DLL_get_first(functionHelper.paramsList);
    for (int i = 0; i < functionHelper.fParamCount+1; ++i) {
        paramArr[i] = tmp->token->keyword;
        tmp = tmp->nextItem;
    }
    enum T_KEYWORD test = functionHelper.returnType;
    if (!htab_insert_func(symtable,functionHelper.name,functionHelper.returnType,functionHelper.fParamCount+1, paramArr)){
        exit_with_message(initToken->lineNum, initToken->charNum, "Symtable insert failed", GENERAL_ERR);
    }
    DLL_dispose_list(functionHelper.paramsList);

    // START PARSING Function BODY
    //analyze_token(symtable);
};

void analyze_token(htab_t* symtable){
    //TODO SCOPE CHECK
    while (true){
        TOKEN_T *token;
        token = get_next_token();
        WriteToken(token);
        if(scope.isDefined == false && token->type != PROG_START){
            exit_with_message(token->lineNum, token->charNum,"You must declare header <?php first", SYNTAX_ERR);
        } else if (scope.isDefined == false && token->type == PROG_START){
            scope.isDefined = true;
        }
        switch (token->type) {
            case KEYWORD:
                switch (token->keyword) {
                    case KEY_ELSE:
                        break;
                    case KEY_FLOAT:
                        break;
                    case KEY_IF:
                        token = get_next_token();
                        if(token->type != LPAR){
                            exit_with_message(token->lineNum,token->charNum,"Expected '('", SYNTAX_ERR);
                        } else {
                            //expression parse
                            if_condition(token,symtable);
                        }
                        break;
                    case KEY_INT:
                        break;
                    case KEY_NULL:
                        break;
                    case KEY_RETURN:
                        break;
                    case KEY_STRING:
                        break;
                    case KEY_WHILE_LOOP:
                        while_condition(token, symtable);
                        break;
                    case KEY_VOID:
                        break;
                    case KEY_BEGIN:
                        break;
                    case KEY_COLON:
                        break;
                    case KEY_FUNCTION:
                        break;
                }
                break;
            case TOKEN_ID:
                var_declaration(symtable, token);
                break;
            case FUNC_ID:
                function_detected(token, symtable);
                //analyze_token(symtable);
                break;
            case LITERAL:
                break;
            case ASSIGN:
                break;
            case LPAR:
                break;
            case RPAR:
                break;
            case OPERATOR:
                break;
            case ISEOF:
                //TODO: EOF exit, check opened functions, params, attr, etc.
                break;
            case PROG_START:
                //TODO: call generate code
                break;
            case SEMICOLON:
                break;
            case LBRACE:
                scope.openedBracesCount++;
                break;
            case RBRACE:
                //this will stop function parsing, hopefully
                if(functionHelper.fParsing && scope.openedBracesCount == functionHelper.fBraceCountCheck) function_detected(token,symtable);
                scope.openedBracesCount--;
                break;
            case COMMA:
                break;
            case DATA_TYPE:
                break;
            case FUNC_CALL:
                function_call(token, symtable);
                break;
        }
        if (token->type == ISEOF) break;
    }
};

/**
 * @brief Debug function. Writes token
 *
 * @param token
 */
void WriteToken(TOKEN_T * token) {
    switch (token->type) {
        case KEYWORD:
            printf("KEYWORD: %u", token->keyword);
            break;

        case TOKEN_ID:
            printf("TOKEN_ID: name: %s", token->name);
            break;

        case FUNC_ID:
            printf("FUNC_ID: name: %s", token->name);
            break;

        case LITERAL:
            switch (token->value.type) {
                case 0:
                    printf("LITERAL: INT: value: %d", token->value.int_val);
                    break;

                case 1:
                    printf("LITERAL: STRING: value: %s", token->value.char_val);
                    break;

                case 2:
                    printf("LITERAL: DOUBLE value: %f", token->value.double_val);
                    break;

                default:
                    printf("LITERAL: UNKNOWN TYPE: %d", token->value.type);
                    break;
            }
            break;

        case ASSIGN:
            printf("ASSIGN: =");
            break;

        case LPAR:
            printf("LPAR: (");
            break;

        case RPAR:
            printf("RPAR: )");
            break;
        case RBRACE:
            printf("RBRACE: {");
            break;
        case LBRACE:
            printf("LBRACE: }");
            break;

        case OPERATOR:
            switch (token->operators) {
                case EQUALS:
                    printf("OPERATOR: ==");
                    break;

                case PLUS:
                    printf("OPERATOR: +");
                    break;

                case MINUS:
                    printf("OPERATOR: -");
                    break;
                case TYPE_EQUALS:
                    printf("OPERATOR: === ");
                    break;
                default:
                    printf("OPERATOR: unknown, %u", token->operators);
                    break;
            }
            break;

        case ISEOF:
            printf("ISEOF");
            break;

        case FUNC_CALL:
            printf("FUNC CALL name: %s", token->name);
            break;
        case PROG_START:
            printf("PROG_START <?php");
            break;
        case PROG_END:
            printf("PROG_END ?>");
            break;

        default:
            printf("UNKNOWN: type: %d, name: %s, value type: %d", token->type, token->name, token->value.type);
            break;
    }

    // printf("\n");
    printf(", %d:%d\n", token->lineNum, token->charNum);
}
