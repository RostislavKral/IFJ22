/**
 * @file syntax_and_semantic_analyzer.c
 * @author Nikita Sniehovskyi xsnieh00
 * @author Lukáš Ježek <xjezek19@stud.fit.vutbr.cz>
 * @brief syntax and semantiv analyzer implementation
 * @version 0.1
 * @date 2022-11-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "syntax_and_semantic_analyzer.h"
#include "lexer.h"

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
        .isDefined = false
};

bool is_token_eof(TOKEN_T* token){
    if(token->type == ISEOF){
        fprintf(stderr, "%s", "EOF");
        return true;
    } else{
        return false;
    }
}

//frameList* create_new_frame() {
//    frameList *newFrameList = malloc(sizeof (struct List));
//   newFrameList->firstElement = NULL;
//    return newFrameList;
//}
//
//void remove_frame(frameList *frame){
//    //kontrola existence
//    if (frame != NULL || frame->firstElement != NULL) {
//        struct frameElement *delNode = frame->firstElement;
//        //projedu cely frame od zacatku do konce postupne uvolnuji dokud nanarazim na konec
//        while (delNode != NULL){
//            frame->firstElement = delNode->nextElement;
//            free(delNode);
//            delNode = frame->firstElement;
//        }
//        frame->firstElement = NULL;
//    }
//}
//void frame_error(){
//    printf("*ERROR* The program has performed an illegal operation.\n");
//    exit(8);
//}
//void insert_to_frame(frameList *frame, TOKEN_T token){
//    struct frameElement *newFrameElement = malloc(sizeof(struct frameElement));
//    if (frame == NULL ||newFrameElement == NULL){
//        frame_error();
//    } else {
//        // set dat a nastavim jako first
//        newFrameElement->localVariableToken = token;
//        newFrameElement->nextElement = frame->firstElement;
//        frame->firstElement = newFrameElement;
//    }
//}
//
//frameElementPtr search_in_frame(frameList frame, TOKEN_T token){
//    frameElementPtr tmp = frame.firstElement;
//    while(tmp != NULL){
//        if (tmp->localVariableToken.type == token.type && tmp->localVariableToken.name == token.name){
//            return tmp;
//        }
//        tmp = tmp->nextElement;
//    }
//    return NULL;
//}

void function_end_parsing(){
    functionHelper.fParsing = false;
    functionHelper.fNamePass = false;
    functionHelper.fParamPass = false;
    functionHelper.fBodyParsing = false;
    functionHelper.fHeadParsed = false;
    functionHelper.fParamCount = 0;
    functionHelper.fReturnTypePass = false;
    functionHelper.fBraceCountCheck = 0;
    scope.num--;
    scope.openedBracesCount--;
};

//TODO !!!!!!!!!!!!!!!!!!!!!!!!!! NOT FINISHED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void analyze_and_store_expression(DLList *list,TOKEN_T* token){
    TOKEN_T *nextToken = get_next_token();
    if(nextToken->operators == token->operators){
        //TODO ERR 2 operatory za s sebou
        // TODO: poslat cely expression v DLL listu, pouzivej insert last (abych to necetl odzadu)
    }
    DLL_insert_last(list, token);

}
void if_condition(){
    TOKEN_T *firstConditionToken = get_next_token();
    if (firstConditionToken->type == TOKEN_ID){
        //TODO CHECK IF VAR EXISTS
    }
}

void var_declaration(TOKEN_T token){
    if (token.keyword == KEY_INT || token.keyword == KEY_FLOAT || token.keyword == KEY_STRING){
        TOKEN_T *operatorToken = get_next_token();
        if(operatorToken->type != EQUALS) exit_with_message(operatorToken->lineNum, operatorToken->charNum, "Invalid assing",SYNTAX_ERR);
        //TODO store to symtable
    } else {
        exit_with_message(token.lineNum, token.charNum, "CASE STATEMENT ERR", SEM_OTHER);
    }
}

void variable_token(TOKEN_T *variable,htab_t* symtable){
    TOKEN_T *operator = get_next_token();
    if (operator->operators == EQUALS){
        //TODO CHECK symtable IF EXISTS..
        if(true){
            //update value
            TOKEN_T* expression = get_next_token();

        } else {
            //TODO ADD TO LOCAL SYMTABLE
            //htab_lookup_add(scope.localSymTable,operator->name);

        }
    } else {
        exit_with_message(operator->lineNum,operator->charNum,"Invalid operator", SYNTAX_ERR);
    }
}

void function_call(TOKEN_T *funcName,htab_t* symtable){
    //htab_find();

}
//TODO !!!!!!!!!!!!!!!!!!!!!!!!!! END NOT FINISHED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


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
                            DLL_insert_first(functionHelper.paramsList, paramToken);
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
                    functionHelper.returnType = returnTypeToken->type;
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
            scope.openedBracesCount++;
            functionHelper.fHeadParsed = true;
            functionHelper.fBodyParsing = true;
            functionHelper.fBraceCountCheck = scope.openedBracesCount;
        } else {
            exit_with_message(token->lineNum, token->charNum, "declaration error", SYNTAX_ERR);
        }
    }
    //htab_insert_func(symtable,)
    enum T_KEYWORD paramArr[functionHelper.fParamCount+1];
    DLLItem *tmp = DLL_get_first(functionHelper.paramsList);
    for (int i = 0; i < functionHelper.fParamCount; ++i) {
        paramArr[i] = tmp->token->keyword;
        tmp->nextItem;
    }
    if (!htab_insert_func(symtable,functionHelper.name,functionHelper.returnType,functionHelper.fParamCount, paramArr)){
        exit_with_message(initToken->lineNum, initToken->charNum, "Symtable insert failed", GENERAL_ERR);
    }
    DLL_dispose_list(functionHelper.paramsList);

    // START PARSING Function BODY
    analyze_token(symtable);
};

void analyze_token(htab_t* symtable){
    TOKEN_T *token;
    token = get_next_token();
    WriteToken(token);
    //TODO UNCOMMENT WHEN LEXER DONE
//    if(scope.isDefined == false && token->keyword != KEY_BEGIN){
//        exit_with_message(token->lineNum, token->charNum,"You must declare header <?php first", SYNTAX_ERR);
//    } else if (scope.isDefined == false && token->keyword == KEY_BEGIN){
//        scope.isDefined = true;
//    }
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
                        if_condition();
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
            variable_token(token,symtable);
            break;
        case FUNC_ID:
            function_detected(token, symtable);
            analyze_token(symtable);.
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
            break;
        case RBRACE:
            //this will stop function parsing, hopefully
            if(functionHelper.fParsing && scope.openedBracesCount == functionHelper.fBraceCountCheck) function_detected(token,symtable);
            break;
        case COMMA:
            break;
        case DATA_TYPE:
            break;
        case FUNC_CALL:
            function_call(token, symtable);
            break;
    }
};


enum SNS_ERROR errno = 0;


//int SNS_start(char *filename) {
//    TOKEN_T * token = NULL;
//
//    FILE * fp = fopen(filename, "r");
//
//    int state = 0;
//    RULE_T * rule = NULL;
//    enum RULE_TYPE_T type = T_UNDEFINED;
//    do {
//
//        /*
//            Command cycle:
//                - Get first token
//                - Decide which command is currently executed
//                    > Get second token to decide if needed (TODO)
//                - Read untill ; or {
//                    > Fill RULE_T
//                - If new IDs declareted -> add them to the symtable
//
//                - Send RULE_T to the codegen. Repeat
//        */
//
//        /* - Get first token */
//        token = get_next_token(fp);
//
//        switch (state) {
//
//            case 0:
//                /* - Decide which command is currently executed */
//                type = SNS_Decide_rule_type(token);
//                state = 1;
//            case 1:
//                switch (type) {
//
//                    case T_ASSIGN_VALUE:
//                        // Filling function for assign command
//                        int out_code = SNS_Assign_value(&rule, token);
//
//                        if (out_code == 0) { // Error
//                            errno = UNEXPECTED_TOKEN;
//                            Error_msg();
//                        } else if (out_code == 4) { // Completed
//                            // TODO Codegen(rule);
//                            // Table connect;
//                            // Destruct_rule(rule);
//                            // rule = NULL;
//
//                            type = T_UNDEFINED;
//                            state = 0;
//                        }
//
//                        break;
//
//                    default:
//                        errno = UNKNOWN_COMMAND;
//                        Error_msg();
//                        break;
//                }
//                break;
//
//            default:
//                break;
//        }
//
//    } while (token->type != ISEOF && !errno);
//
//
//    fclose(fp);
//
//    return 0;
//}




int SNS_Assign_value(RULE_T ** rule, TOKEN_T * token) {

    if ((*rule) == NULL) {
        (*rule) = malloc(sizeof(RULE_T));
        (*rule)->type = T_ASSIGN_VALUE;

        (*rule)->rule.rule_av.token_id_token = NULL;
        (*rule)->rule.rule_av.assign_token = NULL;
        (*rule)->rule.rule_av.expression = NULL;
        (*rule)->rule.rule_av.semicolon = NULL;

        (*rule)->rule.rule_av.token_id_token = token;

        return 1;
    } else if ((*rule)->rule.rule_av.assign_token == NULL) {

        if (token->type == ASSIGN) {
            (*rule)->rule.rule_av.assign_token = token;
        } else {
            // Error
            return 0;
        }

        return 2;
    } else if (token->type != SEMICOLON) {
        // TODO Fill_expression(rule, token);
        (*rule)->rule.rule_av.expression = malloc(sizeof(R_EXPRESSION));
        (*rule)->rule.rule_av.expression->value_left_token = malloc(sizeof(R_VALUE));
        (*rule)->rule.rule_av.expression->value_left_token->value_token = token; // TODO remove

        return 3;
    } else if (token->type == SEMICOLON) {
        (*rule)->rule.rule_av.semicolon = token;

        return 4;
    } else {
        // Error
        return 0;
    }
}




enum RULE_TYPE_T SNS_Decide_rule_type(TOKEN_T * token) {
    switch (token->type) {

        case TOKEN_ID:
            return T_ASSIGN_VALUE;
            break;

        default:
            return T_UNDEFINED;
            break;
    }
}




//TODO: UNUSED !!!!!!!!!!!!!! REFACTOR TO exit_with_message
void Error_msg() {
    switch (errno) {

        case OK:
            printf("No errors\n");
            break;

        case UNKNOWN_COMMAND:
            printf("Cannot identify command\n");
            break;

        default:
            printf("Unknown error\n");
            break;
    }
}




/**
 * @brief Debug function. Writes token
 *
 * @param token
 */
void WriteToken(TOKEN_T * token) {
    switch (token->type) {
        case KEYWORD:
            printf("KEYWORD: %u\n", token->keyword);
            break;

        case TOKEN_ID:
            printf("TOKEN_ID: name: %s\n", token->name);
            break;

        case FUNC_ID:
            printf("FUNC_ID: name: %s\n", token->name);
            break;

        case LITERAL:
            switch (token->value.type) {
                case 0:
                    printf("LITERAL: INT: value: %d\n", token->value.int_val);
                    break;

                case 1:
                    printf("LITERAL: STRING: value: %s\n", token->value.char_val);
                    break;

                case 2:
                    printf("LITERAL: DOUBLE value: %f\n", token->value.double_val);
                    break;

                default:
                    printf("LITERAL: UNKNOWN TYPE: %d\n", token->value.type);
                    break;
            }
            break;

        case ASSIGN:
            printf("ASSIGN: =\n");
            break;

        case LPAR:
            printf("LPAR: (\n");
            break;

        case RPAR:
            printf("RPAR: )\n");
            break;
        case RBRACE:
            printf("RBRACE: {\n");
            break;
        case LBRACE:
            printf("LBRACE: }\n");
            break;

        case OPERATOR:
            switch (token->operators) {
                case EQUALS:
                    printf("OPERATOR: ==\n");
                    break;

                case PLUS:
                    printf("OPERATOR: +\n");
                    break;

                case MINUS:
                    printf("OPERATOR: -\n");
                    break;

                default:
                    printf("OPERATOR: unknown\n");
                    break;
            }
            break;

        case ISEOF:
            printf("ISEOF\n");
            break;

        case FUNC_CALL:
            printf("FUNC CALL name: %s\n", token->name);
            break;

        default:
            printf("UNKNOWN: type: %d, name: %s, value type: %d\n", token->type, token->name, token->value.type);
            break;
    }
}
