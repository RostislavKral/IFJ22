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
        .paramsList = NULL,
        .paramsListNames = NULL,
        .fNameToken = NULL
};
scopeHelper scope = {
        .num = 0,
        .openedBracesCount = 0,
        .lastScopeOpeningToken = NULL,
        .isDefined = false,
        .openedIfCount = 0,
        .strictTypesDeclared = false,
        .parseNext = true
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
    functionHelper.paramsListNames = NULL;
    functionHelper.fNameToken = NULL;
    scope.openedBracesCount--;
    scope.num--;
    DLL_pop_first(scope.braceList);
};

void while_condition(TOKEN_T* token, htab_t* symtable){
    TOKEN_T* lparToken = get_next_token();
    if (lparToken->type != LPAR) exit_with_message(lparToken->lineNum,lparToken->charNum,"Invalid expression in condition", SYNTAX_ERR);
    TOKEN_T *tmpToken = get_next_token();
    DLList* condExpList = expression_list(tmpToken,symtable, RPAR);
    if(condExpList->first == NULL) exit_with_message(token->lineNum,token->charNum,"Invalid expression in condition", SYNTAX_ERR);
    BSTnode* condExprBST = analyze_precedence(condExpList);
    if (condExprBST == NULL || condExprBST->type == KEY_NULL) exit_with_message(token->lineNum, token->charNum, "Invalid expression condition", SEM_MATH_ERR);
    open_brace_list(token);
    token = get_next_token();
    gen_if(condExprBST);
}

void if_condition(TOKEN_T* token, htab_t* symtable){
    TOKEN_T *tmpToken = get_next_token();
    DLList* condExpList = expression_list(tmpToken, symtable, RPAR);
    if(condExpList->first == NULL) exit_with_message(token->lineNum,token->charNum,"Invalid expression in condition", SYNTAX_ERR);
    BSTnode* condExprBST = analyze_precedence(condExpList);
    if (condExprBST == NULL || condExprBST->type == KEY_NULL) exit_with_message(token->lineNum, token->charNum, "Invalid expression condition", SEM_MATH_ERR);
    scope.openedIfCount++;
    open_brace_list(token);
    token = get_next_token();
    //if (token->type == RBRACE) close_brace_list();
    gen_if(condExprBST);
}
void else_condition(TOKEN_T* token, htab_t* symtable){
    TOKEN_T *tmpToken = get_next_token();
    open_brace_list(token);
    gen_else();
}


DLList *expression_list(TOKEN_T*tmpToken,htab_t* symtable, enum T_TOKEN_TYPE closingToken){
    DLList *precedenceList = malloc(sizeof (struct DLList));
    DLL_init(precedenceList);
    TOKEN_T *previousTmpToken;
    //TOKEN_T *tmpToken = get_next_token();
    while (tmpToken->type != closingToken){
        if (tmpToken->type == LITERAL || tmpToken->type == TOKEN_ID || tmpToken->type == OPERATOR || (tmpToken->keyword == KEY_NULL && tmpToken->type == KEYWORD) ||
                tmpToken->type == LPAR || tmpToken->type == RPAR){
//            if (previousTmpToken != NULL && ((previousTmpToken->type == LPAR || previousTmpToken->type == RPAR || previousTmpToken->type == ASSIGN || previousTmpToken->type == LITERAL) && tmpToken->type != OPERATOR)) exit_with_message(previousTmpToken->lineNum, previousTmpToken->charNum, "Missing operator", SYNTAX_ERR);
            if (tmpToken->type == TOKEN_ID){
                htab_item_t *item = htab_find_var(symtable, tmpToken->name, scope.num);
                if (item == NULL) exit_with_message(tmpToken->lineNum, tmpToken->charNum, "Undefined variable", SEM_UNDEF_VAR_ERR);
                if (item->data.data_type[0] == KEY_FLOAT) tmpToken->value.type = 2;
                if (item->data.data_type[0] == KEY_INT) tmpToken->value.type = 0;
                if (item->data.data_type[0] == KEY_STRING) tmpToken->value.type = 1;
            }
            if (tmpToken->keyword == KEY_NULL){
                tmpToken->type = LITERAL;
                //tmpToken->keyword = KEY_INT;
                tmpToken->value.type = 0;
                tmpToken->value.int_val = 0;
            }
            previousTmpToken = tmpToken;
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

    TOKEN_T* tmpToken = get_next_token();
    if (tmpToken->type == FUNC_CALL){
        htab_item_t *item = htab_find_func(symtable, tmpToken->name);
        if (item != NULL){
            htab_value val = {.str_value = NULL};
            function_call(tmpToken,symtable);
            if(htab_insert_var(symtable, varNameToken->name, scope.num, item->data.data_type[0], val)){
                //TODO GEN Expr gen_expression(varNameToken, expressionTree, scope.num, true);
            }
        } else {
            exit_with_message(tmpToken->lineNum,tmpToken->charNum, "invalid assing", SEM_F_DECLARATION_ERR);
        }
    } else {
        DLList* precedenceList = expression_list(tmpToken, symtable, SEMICOLON);
        BSTnode *expressionTree = analyze_precedence(precedenceList);

        if (expressionTree == NULL)
        {
            exit_with_message(varNameToken->lineNum, varNameToken->charNum, "Invalid expression", SEM_MATH_ERR);
        }
//        if(expressionTree->type == KEY_NULL)
//        {
//            exit_with_message(varNameToken->lineNum, varNameToken->charNum, "Invalid expression", SEM_MATH_ERR);
//        }

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
                    htab_item_t *test = htab_find_var(symtable,varNameToken->name, scope.num);
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

//            enum T_KEYWORD data = stFunction->data.data_type[i];
//            enum T_KEYWORD data2 = param->data.data_type[0];

            if((stFunction->data.data_type[i] == KEY_INT && param->data.data_type[0] == KEY_INT) ||
                (stFunction->data.data_type[i] == KEY_STRING && param->data.data_type[0] == KEY_STRING) ||
                (stFunction->data.data_type[i] == KEY_FLOAT && param->data.data_type[0] == KEY_FLOAT)
                    ) paramArr[i] = tmp;
            else exit_with_message(tmp->lineNum, tmp->charNum, "Invalid data type", SEM_F_CALL_PARAM_ERR);
        } else {
            // printf("\n%u\n",stFunction->data.data_type[i]);
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
        exit_with_message(tmp->lineNum, tmp->charNum, "Expected ')'", SEM_F_CALL_PARAM_ERR);
    }
    tmp = get_next_token();
    if (tmp->type != SEMICOLON)
    {
        exit_with_message(tmp->lineNum, tmp->charNum, "Missing semicolon after function call", SYNTAX_ERR);
    }

//    gen_func_call(htab_find_func(symtable, functionToken->name), functionToken->name);
}


void function_detected(TOKEN_T* initToken, htab_t* symtable){
    //F already declared
    if(initToken->type == FUNC_ID && htab_find_func(symtable,initToken->name) != NULL){
        TOKEN_T *test = get_next_token();
        while(test->type != SEMICOLON){
            if (test->keyword == KEY_COLON) exit_with_message(test->lineNum,test->charNum, "Function is already declared", SEM_F_DECLARATION_ERR);
            test = get_next_token();
        }
        exit_with_message(initToken->lineNum,initToken->charNum, "bad call", SYNTAX_ERR);
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
                char* arr[10] = {"void","return","int","float","string","else", "if","while","funciton","null"};
                for (int i = 0; i < 10; ++i) {
                    if(strcmp(arr[i],token->name) == 0) exit_with_message(token->lineNum,token->charNum,"invalid function name", SYNTAX_ERR);
                }
                functionHelper.fNameToken = token;
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
                                functionHelper.paramsListNames = malloc(sizeof (struct DLList));;
                                DLL_init(functionHelper.paramsList);
                                DLL_init(functionHelper.paramsListNames);
                            }
                            functionHelper.fParamCount++;
                            if(DLL_find_token(functionHelper.paramsListNames, paramName))exit_with_message(paramToken->lineNum,paramToken->charNum, "redeclaration",SEM_F_CALL_PARAM_ERR);
                            DLL_insert_last(functionHelper.paramsList, paramToken);
                            DLL_insert_last(functionHelper.paramsListNames, paramName);
                        }
                    } else if (paramToken->type == COMMA) {
                        //functionHelper.fParamCount++;
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
            open_brace_list(functionHelper.fNameToken);
            functionHelper.fHeadParsed = true;
            functionHelper.fBodyParsing = true;
            functionHelper.fBraceCountCheck = scope.openedBracesCount;
        } else {
            exit_with_message(token->lineNum, token->charNum, "declaration error", SYNTAX_ERR);
        }
    }
    //htab_insert_func(symtable)
    enum T_KEYWORD paramArr[functionHelper.fParamCount+1];

    int a = functionHelper.fParamCount;
    if (functionHelper.fParamCount != 0) {
        DLLItem *tmp = DLL_get_first(functionHelper.paramsList);
        for (int i = 0; i < functionHelper.fParamCount; ++i) {
            paramArr[i] = tmp->token->keyword;
            tmp = tmp->nextItem;
        }
    }
    if (!htab_insert_func(symtable,functionHelper.name,functionHelper.returnType,functionHelper.fParamCount, paramArr)){
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
        if (tmpToken->type != LITERAL && tmpToken->type != TOKEN_ID && (tmpToken->keyword != KEY_NULL)) exit_with_message(tmpToken->lineNum,tmpToken->charNum,"params err", SYNTAX_ERR);
        DLL_insert_last(dynamicParams, tmpToken);

        if (tmpToken != NULL)
        {
            gen_write(tmpToken);
        }

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
//            printf("%d",tmpItem->token->value.int_val);
        } else if(tmpItem->token->value.type == 2){
//            printf("%a",tmpItem->token->value.double_val);
        } else if (tmpItem->token->value.type == 1){
//            printf("%s",tmpItem->token->value.char_val);
        } else {
            exit_with_message(tmpItem->token->lineNum, tmpItem->token->charNum, "INTERNAL ERR", SEM_F_CALL_PARAM_ERR);
        }
        tmpItem = tmpItem->nextItem;
    }
}

void checkReturnType(TOKEN_T* token, htab_t* symtable){
    TOKEN_T* returnValToken = get_next_token();
    if (functionHelper.returnType == KEY_VOID && returnValToken->type != SEMICOLON)exit_with_message(token->lineNum,token->charNum,"Invalid expression in condition", SEM_F_RETURN_VAL_ERR);
    if(returnValToken->type == FUNC_CALL){
        htab_item_t* fCall = htab_find_func(symtable, returnValToken->name);
        if (fCall->data.data_type[0] != functionHelper.returnType) exit_with_message(token->lineNum, token->charNum, "Mismatch in return type", SEM_F_CALL_PARAM_ERR);
        else {
            function_call(returnValToken, symtable);
        }
    } else {
        DLList* condExpList = expression_list(returnValToken, symtable, SEMICOLON);
        if(condExpList->first == NULL) exit_with_message(token->lineNum,token->charNum,"Invalid expression in condition", SEM_F_RETURN_VAL_ERR);
        BSTnode* condExprBST = analyze_precedence(condExpList);
        if (condExprBST->type != functionHelper.returnType) exit_with_message(token->lineNum, token->charNum, "Mismatch in return type", SEM_F_CALL_PARAM_ERR);
    }
    if (token->type == SEMICOLON) return;
//    if (functionHelper.returnType == KEY_VOID && returnValToken->type != SEMICOLON) exit_with_message(token->lineNum, token->charNum, "Mismatch in return type", SEM_F_CALL_PARAM_ERR);
//    if (returnValToken->type == TOKEN_ID){
//        htab_item_t* varToken = htab_find_var(symtable, returnValToken->name,scope.num);
//        if (varToken == NULL) exit_with_message(returnValToken->lineNum, returnValToken->charNum, "Return var not found", SEM_UNDEF_VAR_ERR);
//        else {
//            if (varToken->data.data_type[0] != functionHelper.returnType){
//                exit_with_message(token->lineNum, token->charNum, "Mismatch in return type", SEM_F_CALL_PARAM_ERR);
//            }
//        }
//    } else if (returnValToken->type == LITERAL){
//        if (returnValToken->value.type == 0 && functionHelper.returnType != KEY_INT)exit_with_message(token->lineNum, token->charNum, "Mismatch in return type", SEM_F_CALL_PARAM_ERR);
//        if (returnValToken->value.type == 1 && functionHelper.returnType != KEY_STRING)exit_with_message(token->lineNum, token->charNum, "Mismatch in return type", SEM_F_CALL_PARAM_ERR);
//        if (returnValToken->value.type == 2 && functionHelper.returnType != KEY_FLOAT)exit_with_message(token->lineNum, token->charNum, "Mismatch in return type", SEM_F_CALL_PARAM_ERR);
//    }

//    TOKEN_T *tmpToken = get_next_token();
//    if (tmpToken->type == SEMICOLON) return;
//    else if (tmpToken->type == OPERATOR) {
//        tmpToken = get_next_token();
//        checkReturnType(tmpToken, symtable);
//    }

}
void open_brace_list(TOKEN_T *token){
    DLL_insert_first(scope.braceList, token);
    scope.openedBracesCount++;
}
TOKEN_T *close_brace_list(){
    if (scope.braceList->itemsCount == 0) exit_with_message(0,0,"missing opening brace", SYNTAX_ERR);
    DLLItem *item = DLL_pop_first(scope.braceList);
    scope.openedBracesCount--;
    if(item->token->keyword == KEY_IF){
        TOKEN_T *tmpToken = get_next_token();
        if (tmpToken->type == RBRACE){
            scope.openedBracesCount--;
            DLL_pop_first(scope.braceList);
        }
        if (tmpToken->type == KEY_ELSE){
            else_condition(tmpToken, scope.globalSymTable);
        } else {
            return tmpToken;
        }
    } else if (item->token->keyword == KEY_ELSE){
        gen_else_exit();
    } else if (item->tokenType == FUNC_ID){
        if (scope.openedBracesCount != 0) exit_with_message(item->token->lineNum, item->token->charNum, "Missing brace", SYNTAX_ERR);
    }
    return NULL;
}
void analyze_token(htab_t* symtable){
    scope.globalSymTable =symtable;
    scope.braceList = malloc(sizeof (struct DLList));
    DLL_init(scope.braceList);
    //TODO ELSE, Return
    TOKEN_T *previousToken;
    TOKEN_T *token;
    while (true){
        if(scope.parseNext) token = get_next_token();
        scope.parseNext = true;
        if(scope.isDefined == false && token->type != PROG_START){
            exit_with_message(token->lineNum, token->charNum,"You must declare header <?php first", SYNTAX_ERR);
        } else if (scope.isDefined == false && token->type == PROG_START){
            gen_header();
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
                        if (scope.lastScopeOpeningToken == NULL || scope.lastScopeOpeningToken->keyword != KEY_IF){
                            exit_with_message(token->lineNum, token->charNum, "Must define IF first", SYNTAX_ERR);
                        } else {
                            //else_condition(token,symtable);
                        }
                        break;
                    case KEY_FLOAT:
                        //exit_with_message(token->lineNum,token->charNum,"Syntax err", SYNTAX_ERR);
                        break;
                    case KEY_IF:
                        previousToken = token;
                        token = get_next_token();
                        if(token->type != LPAR){
                            exit_with_message(token->lineNum,token->charNum,"Expected '('", SYNTAX_ERR);
                        } else {
                            //expression parse
                            scope.lastScopeOpeningToken = previousToken;
                            if_condition(previousToken,symtable);
                            //scope.num++;
                        }
                        break;
                    case KEY_INT:
                        exit_with_message(token->lineNum,token->charNum,"Syntax err", SYNTAX_ERR);
                        break;
                    case KEY_NULL:
                        break;
                    case KEY_RETURN:
                        if (functionHelper.fParsing)checkReturnType(token,symtable);
                        break;
                    case KEY_STRING:
                        exit_with_message(token->lineNum,token->charNum,"Syntax err", SYNTAX_ERR);
                        break;
                    case KEY_WHILE_LOOP:
                        scope.lastScopeOpeningToken = token;
                        while_condition(token, symtable);
                        //scope.num++;
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
                open_brace_list(token);
                //exit_with_message(token->lineNum,token->charNum,"Syntax err", SYNTAX_ERR);
                break;
            case RBRACE:
                //this will stop function parsing, hopefully
                if(functionHelper.fParsing && scope.openedBracesCount == functionHelper.fBraceCountCheck) function_detected(token,symtable);
                else{
                    TOKEN_T *tmp = close_brace_list();
                    if (tmp != NULL){
                        token = tmp;
                        scope.parseNext = false;
                    }
                }
                //scope.openedBracesCount--;

                //scope.num--;
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
                if(scope.openedBracesCount != 0) exit_with_message(token->lineNum, token->charNum,"opened braces", SYNTAX_ERR);
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
