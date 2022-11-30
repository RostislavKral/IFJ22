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
        .fReturnTypePass = false
};

bool is_token_eof(TOKEN_T* token){
    if(token->type == ISEOF){
        fprintf(stderr, "%s", "EOF");
        return true;
    } else{
        return false;
    }
}

void function_detected(TOKEN_T* initToken){
    //catch declaration of f in f
    if(functionHelper.fParsing == true){
        exit_with_message(initToken->lineNum, initToken->charNum, "already parsing another function", SEM_F_DECLARATION_ERR);
        return;
    }
    functionHelper.fParsing = true;
    functionHelper.fHeadParsed = true;
    functionHelper.fParamCount = 0;
    //get f name and params
    while (functionHelper.fHeadParsed == false){
        TOKEN_T *token = get_next_token();
        //check eof and declaration of F inside Fname
        if(is_token_eof(token) || token->keyword == KEY_FUNCTION){
            //exit msg, found keyword
            exit_with_message(token->lineNum, token->charNum, "Invalid token EOF or double function declaration", SEM_F_DECLARATION_ERR);
        }
        if(functionHelper.fNamePass == false){
            //get f name if not exist, expecting id
            if(token->type != FUNC_ID){
                exit_with_message(token->lineNum,token->charNum,"Invalid function name", SEM_F_DECLARATION_ERR);
            } else {
                //TODO symtable write
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
                            //TODO to symtable write
                            functionHelper.fParamCount++;
                        }
                    } else if (paramToken->type == COMMA) {
                        functionHelper.fParamCount++;
                        //TODO Symtable write
                    } else {
                        //ERR invalid token expecting dataType
                        exit_with_message(paramToken->lineNum, paramToken->charNum, "invalid token expecting $int,$string,$float", SYNTAX_ERR);
                        continue;
                    }
                    paramToken = get_next_token();
                    if(paramToken->keyword == RPAR) functionHelper.fParamPass = true;
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
                    //TODO sym write
                    functionHelper.fReturnTypePass = true;
                    continue;
                } else {
                    exit_with_message(returnTypeToken->lineNum, returnTypeToken->charNum, "Invalid function return type", SYNTAX_ERR);
                }
            }
        }
        if(token->type == LBRACE && functionHelper.fNamePass && functionHelper.fParamPass){
            functionHelper.fHeadParsed = true;
            functionHelper.fBodyParsing = true;
        } else {
            exit_with_message(token->lineNum, token->charNum, "declaration error", SYNTAX_ERR);
        }
    }
    //TODO: START PARSING Function BODY
    analyze_token();
};

void analyze_token(){
    TOKEN_T *token = get_next_token();
    switch (token->type) {
        case KEYWORD:
            switch (token->keyword) {
                case KEY_ELSE:
                    break;
                case KEY_FLOAT:
                    break;
                case KEY_FUNCTION:
                    function_detected(token);
                    break;
                case KEY_IF:
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
            }
            break;
        case TOKEN_ID:
            break;
        case FUNC_ID:
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
            break;
        case SEMICOLON:
            break;
        case LBRACE:
            break;
        case RBRACE:
            //this will stop function parsing, hopefully
            if(functionHelper.fParsing) function_detected(token);
            break;
        case COMMA:
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
            printf("KEYWORD: %s\n", token->name);
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

        default:
            printf("UNKNOWN: type: %d, name: %s, value type: %d\n", token->type, token->name, token->value.type);
            break;
    }
}
