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
        .openedIfCount = 0,
        .strictTypesDeclared = false
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
    //scope.openedBracesCount--;
};

void while_condition(TOKEN_T* token, htab_t* symtable){
    DLList* condExpList = expression_list(symtable, RPAR);
    if(condExpList->first == NULL) exit_with_message(token->lineNum,token->charNum,"Invalid expression in condition", SYNTAX_ERR);
    BSTnode* condExprBST = analyze_precedence(condExpList);
    if (condExprBST == NULL) exit_with_message(token->lineNum, token->charNum, "Invalid expression condition", SEM_MATH_ERR);
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
        if (expressionTree->token->type == LITERAL){
            if (expressionTree->token->value.type == 0){
                htab_value val =  {.int_value = expressionTree->token->value.int_val};
                if(htab_insert_var(symtable, varNameToken->name, scope.num, expressionTree->type, val)){
                    gen_expression(varNameToken, expressionTree, scope.num, true);
                } else {
                    htab_update_var(symtable, varNameToken->name, scope.num, expressionTree->type, val);
                    gen_expression(varNameToken, expressionTree, scope.num, false);
                }
            } else if (expressionTree->token->value.type == 1){
                htab_value val =  {.str_value = expressionTree->token->value.char_val};
                if(htab_insert_var(symtable, varNameToken->name, scope.num, expressionTree->type, val)){
                    gen_expression(varNameToken, expressionTree, scope.num, true);
                } else {
                    htab_update_var(symtable, varNameToken->name, scope.num, expressionTree->type, val);
                    gen_expression(varNameToken, expressionTree, scope.num, false);
                }
            } else if (expressionTree->token->value.type == 2){
                htab_value val =  {.float_value = expressionTree->token->value.double_val};
                if(htab_insert_var(symtable, varNameToken->name, scope.num, expressionTree->type, val)){
                    gen_expression(varNameToken, expressionTree, scope.num, true);
                } else {
                    htab_update_var(symtable, varNameToken->name, scope.num, expressionTree->type, val);
                    gen_expression(varNameToken, expressionTree, scope.num, false);
                }
            }
        } else {
            //tree
            htab_value zero = {.str_value = NULL};
            if(htab_insert_var(symtable, varNameToken->name, scope.num, expressionTree->type, zero)){
                gen_expression(varNameToken, expressionTree,scope.num,true);
            } else {
                htab_update_var(symtable, varNameToken->name, scope.num, expressionTree->type, zero);
                gen_expression(varNameToken, expressionTree,scope.num,false);
            }
        }
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
        if (tmp->type != TOKEN_ID && tmp->type != LITERAL) exit_with_message(tmp->lineNum, tmp->charNum, "Invalid number of arguments", SEM_F_CALL_PARAM_ERR);
//        printf("param: %d", tmp->value.type);
        if (tmp->type == TOKEN_ID){
            htab_item_t *param = htab_find_var(symtable,tmp->name, scope.num);

            if (param == NULL)
            {
                exit_with_message(tmp->lineNum, tmp->charNum, "Undefined variable", SEM_UNDEF_VAR_ERR);
            }

            if((stFunction->data.data_type[i] == KEY_INT && param->data.data_type[0] == KEY_INT) ||
                (stFunction->data.data_type[i] == KEY_STRING && param->data.data_type[0] == KEY_STRING) ||
                (stFunction->data.data_type[i] == KEY_FLOAT && param->data.data_type[0] == KEY_FLOAT)
                    ) paramArr[i] = tmp;
            else exit_with_message(tmp->lineNum, tmp->charNum, "Invalid data type", SEM_F_CALL_PARAM_ERR);
        } else {
            printf("\n%u\n",stFunction->data.data_type[i]);
            if(tmp->value.type == 0 && stFunction->data.data_type[i] == KEY_INT ||
               tmp->value.type == 1 && stFunction->data.data_type[i] == KEY_STRING ||
               tmp->value.type == 2 && stFunction->data.data_type[i] == KEY_FLOAT
                    ) paramArr[i] = tmp;
            else exit_with_message(tmp->lineNum, tmp->charNum, "Invalid data type2", SEM_F_CALL_PARAM_ERR);
        }
    }

    TOKEN_T *tmp = get_next_token();
    if (tmp->type == TOKEN_ID || tmp->type == COMMA)
    {
        exit_with_message(tmp->lineNum, tmp->charNum, "Invalid number of arguments", SYNTAX_ERR);
    }
    else if (tmp->type != RPAR)
    {
        exit_with_message(tmp->lineNum, tmp->charNum, "Expected ')'", SYNTAX_ERR);
    }
    tmp = get_next_token();
    if (tmp->type != SEMICOLON)
    {
        exit_with_message(tmp->lineNum, tmp->charNum, "Missing semicolon after function call", SYNTAX_ERR);
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
                if(paramToken->type == RPAR) {
                    functionHelper.fParamPass = true;
                    continue;
                }
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
                if(returnTypeToken->keyword == KEY_STRING || returnTypeToken->keyword == KEY_INT || returnTypeToken->keyword == KEY_FLOAT || returnTypeToken->keyword == KEY_VOID){
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
            scope.openedBracesCount++;
            functionHelper.fHeadParsed = true;
            functionHelper.fBodyParsing = true;
            functionHelper.fBraceCountCheck = scope.openedBracesCount;
        } else {
            exit_with_message(token->lineNum, token->charNum, "declaration error", SYNTAX_ERR);
        }
    }
    //htab_insert_func(symtable)
    enum T_KEYWORD paramArr[functionHelper.fParamCount+1];
    if (functionHelper.fParamCount != 0) {
        DLLItem *tmp;
        tmp = DLL_get_first(functionHelper.paramsList);
        for (int i = 0; i < functionHelper.fParamCount+1; ++i) {
            paramArr[i] = tmp->token->keyword;
            tmp = tmp->nextItem;
        }
    }
    if (!htab_insert_func(symtable,functionHelper.name,functionHelper.returnType,functionHelper.fParamCount+1, paramArr)){
        exit_with_message(initToken->lineNum, initToken->charNum, "Symtable insert failed", GENERAL_ERR);
    }
    if(functionHelper.fParamCount != 0)DLL_dispose_list(functionHelper.paramsList);

    // START PARSING Function BODY
    //analyze_token(symtable);
};

void builtin_write(htab_t* symtable){
    TOKEN_T *tmpToken = get_next_token();
    if (tmpToken->type != LPAR) exit_with_message(tmpToken->lineNum, tmpToken->charNum, "expected (", SYNTAX_ERR);

    DLList *dynamicParams = malloc(sizeof (struct DLList));
    DLL_init(dynamicParams);
    tmpToken = get_next_token();
    while (tmpToken->type != RPAR && tmpToken != NULL){
        if (tmpToken->type == COMMA) tmpToken = get_next_token();
        if (tmpToken->type != LITERAL && tmpToken->type != TOKEN_ID) exit_with_message(tmpToken->lineNum,tmpToken->charNum,"params err", SYNTAX_ERR);
        DLL_insert_last(dynamicParams, tmpToken);
        tmpToken = get_next_token();
    }
    DLLItem *tmpItem = DLL_get_first(dynamicParams);
    while (tmpItem != NULL){
        if(tmpItem->token->type == TOKEN_ID){
            htab_item_t *item = htab_find_var(symtable, tmpItem->token->name,scope.num);
           // printf("KOKOT LEO: %d\n",item->data.value.int_value);
            if (item->data.data_type[0] == KEY_INT) {
                tmpItem->token->value.type = 0;
                tmpItem->token->value.int_val = item->data.value.int_value;
            }
            else if (item->data.data_type[0] == KEY_FLOAT){
                tmpItem->token->value.type = 2;
                tmpItem->token->value.double_val = item->data.value.float_value;
            } else if (item->data.data_type[0] == KEY_STRING){
                tmpItem->token->value.type = 1;
                tmpItem->token->value.char_val = item->data.value.str_value;
            }
        }
        if (tmpItem->token->value.type == 0){
            printf("%d",tmpItem->token->value.int_val);
        } else if(tmpItem->token->value.type == 2){
            printf("%a",tmpItem->token->value.double_val);
        } else if (tmpItem->token->value.type == 1){
            printf("%s",tmpItem->token->value.char_val);
        } else {
            exit_with_message(tmpItem->token->lineNum, tmpItem->token->charNum, "INTERNAL ERR", SEM_F_CALL_PARAM_ERR);
        }
        tmpItem = tmpItem->nextItem;
    }
}

void analyze_token(htab_t* symtable){
    //TODO ELSE, Return
    TOKEN_T *previousToken;
    while (true){
        TOKEN_T *token;
        token = get_next_token();
        WriteToken(token);
        if(scope.isDefined == false && token->type != PROG_START){
            exit_with_message(token->lineNum, token->charNum,"You must declare header <?php first", SYNTAX_ERR);
        } else if (scope.isDefined == false && token->type == PROG_START){
            scope.isDefined = true;
            continue;
        } else if (scope.isDefined && !scope.strictTypesDeclared){
            if (token->type != FUNC_CALL) exit_with_message(token->lineNum, token->charNum, "strict types missing", SYNTAX_ERR);
            else {
                if (strcmp(token->name, "declare") == 0){
                    token = get_next_token();
                    if (token->type != LPAR)exit_with_message(token->lineNum, token->charNum, "strict types missing", SYNTAX_ERR);
                    else token = get_next_token();
                    if (strcmp(token->name, "strict_types") == 0){
                        token = get_next_token();
                        if (token->type == ASSIGN){
                            token = get_next_token();
                            if (token->type == LITERAL && token->value.type == 0 && token->value.int_val == 1) scope.strictTypesDeclared = true;
                            token = get_next_token();
                            if (token->type != RPAR) exit_with_message(token->lineNum, token->charNum, "RPAR missing", SYNTAX_ERR);
                            token = get_next_token();
                            if(token->type != SEMICOLON) exit_with_message(token->lineNum, token->charNum, "RPAR missing", SYNTAX_ERR);
                        }
                    } else {
                        exit_with_message(token->lineNum, token->charNum, "strict types missing", SYNTAX_ERR);
                    }
                };
            }
        }
        switch (token->type) {
            case KEYWORD:
                switch (token->keyword) {
                    case KEY_ELSE:
                        break;
                    case KEY_FLOAT:
                        //exit_with_message(token->lineNum,token->charNum,"Syntax err", SYNTAX_ERR);
                        break;
                    case KEY_IF:
                        token = get_next_token();
                        if(token->type != LPAR){
                            exit_with_message(token->lineNum,token->charNum,"Expected '('", SYNTAX_ERR);
                        } else {
                            //expression parse
                            if_condition(token,symtable);
                            scope.num++;
                        }
                        break;
                    case KEY_INT:
                        exit_with_message(token->lineNum,token->charNum,"Syntax err", SYNTAX_ERR);
                        break;
                    case KEY_NULL:
                        break;
                    case KEY_RETURN:
                        break;
                    case KEY_STRING:
                        exit_with_message(token->lineNum,token->charNum,"Syntax err", SYNTAX_ERR);
                        break;
                    case KEY_WHILE_LOOP:
                        while_condition(token, symtable);
                        scope.num++;
                        break;
                    case KEY_VOID:
                        break;
                    case KEY_BEGIN:
                        break;
                    case KEY_COLON:
                        break;
                    case KEY_FUNCTION:
                        break;
                    case NULL_KEYWORD:
                        break;
                    case KEY_BOOLEAN:
                        break;
                }
                break;
            case TOKEN_ID:
                var_declaration(symtable, token);
                break;
            case FUNC_ID:
                function_detected(token, symtable);
                scope.num++;
                //analyze_token(symtable);
                break;
            case LITERAL:
                break;
            case ASSIGN:
                exit_with_message(token->lineNum,token->charNum,"Syntax err", SYNTAX_ERR);
                break;
            case LPAR:
                exit_with_message(token->lineNum,token->charNum,"Syntax err", SYNTAX_ERR);
                break;
            case RPAR:
                exit_with_message(token->lineNum,token->charNum,"Syntax err", SYNTAX_ERR);
                break;
            case OPERATOR:
                exit_with_message(token->lineNum,token->charNum,"Syntax err", SYNTAX_ERR);
                break;
            case ISEOF:
                //TODO: EOF exit, check opened functions, params, attr, etc.
                break;
            case PROG_START:
                exit_with_message(token->lineNum,token->charNum,"Syntax err", SYNTAX_ERR);
                break;
            case SEMICOLON:
                //exit_with_message(token->lineNum,token->charNum,"Syntax err", SYNTAX_ERR);
                break;
            case LBRACE:
                scope.openedBracesCount++;
                break;
            case RBRACE:
                //this will stop function parsing, hopefully
                if(functionHelper.fParsing && scope.openedBracesCount == functionHelper.fBraceCountCheck) function_detected(token,symtable);
                scope.openedBracesCount--;
                scope.num--;
                break;
            case COMMA:
                exit_with_message(token->lineNum,token->charNum,"Syntax err", SYNTAX_ERR);
                break;
            case DATA_TYPE:
                exit_with_message(token->lineNum,token->charNum,"Syntax err", SYNTAX_ERR);
                break;
            case FUNC_CALL:
                if (strcmp(token->name, "write") == 0){
                    builtin_write(symtable);
                } else function_call(token, symtable);
                break;
            case NULL_TYPE:
                break;
            case PROG_END:
                scope.isDefined = false;
                scope.strictTypesDeclared = false;
                token = get_next_token();
                break;
            case DOLLAR:
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
