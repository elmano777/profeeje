#include <iostream>
#include <stdexcept>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"

using namespace std;

// =============================
// Métodos de la clase Parser
// =============================

Parser::Parser(Scanner* sc) : scanner(sc) {
    previous = nullptr;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        throw runtime_error("Error léxico");
    }
}

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;

        if (check(Token::ERR)) {
            throw runtime_error("Error lexico");
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}


// =============================
// Reglas gramaticales
// =============================

Programa* Parser::parseProgram() {
    Programa* ast = parseP();
    if (!isAtEnd()) {
        throw runtime_error("Error sintáctico");
    }
    cout << "Parseo exitoso" << endl;
    return ast;
}

Programa* Parser::parseP() {
    Programa* p =  new Programa();
    while(check(Token::VAR)){
        p->vdlist.push_back(parseVarDec());
        match(Token::SEMICOL);
    }
    while(check(Token::FUN)){
        p->fdlist.push_back(parsefundec());
    }     
    return p;
}


Fundec* Parser::parsefundec(){
    Fundec* fd = new Fundec(); // en honor a Federico
    match(Token::FUN);
    match(Token::ID);
    fd -> tipo = previous -> text;
    match(Token::ID);
    fd -> nombre = previous -> text;
    match(Token::LPAREN);
    if (match(Token::ID)){
        fd->tipo_parametros.push_back(previous -> text);
        match(Token::ID);
        fd->id_parametros.push_back(previous -> text);
        while(match(Token::COMA)){
            match(Token::ID);
            fd->tipo_parametros.push_back(previous -> text);
            match(Token::ID);
            fd->id_parametros.push_back(previous -> text);
        }
    }
    match(Token::RPAREN);
    match(Token::DOSPUNTOS);
    fd->cuerpo = parseBody();
    match(Token::ENDFUN);
    return fd;
}

Body* Parser::parseBody(){
    Body* b = new Body();
    while(check(Token::VAR)){
        b->vdlist.push_back(parseVarDec());
        match(Token::SEMICOL);
    }    
    b->slist.push_back(parsestmt());
    while (match(Token::SEMICOL)) {
        b->slist.push_back(parsestmt());
    }
    return b;
} 

VarDec* Parser::parseVarDec(){
    match(Token::VAR);
    VarDec* decl = new VarDec();
    match(Token::ID);
    decl->tipo = previous->text;
    while(match(Token::ID)){
        decl->chicharron.push_back(previous->text);
        match(Token::COMA);
    }
    return decl;
}

Stmt *Parser::parsestmt() {
    Exp* e;
    if (match(Token::PRINT)) {
        match(Token::LPAREN);
        e = parseCEXP();
        match(Token::RPAREN);
        return new PrintStmt(e);
    }
    if (match(Token::RETURN)) {
        ReturnStm* retorno = new ReturnStm();
        retorno -> exp =  parseCEXP();
        return retorno;
    }
    else if (match(Token::IF)) {
        e = parseCEXP();
        IfStmt* roberto = new IfStmt(e);
        match(Token::THEN);
        roberto -> cuerpodelif = parseBody();
        if(match(Token::ELSE)){
            roberto->hayelse = true;
            roberto -> cuerpodelelse = parseBody();
        }
        match(Token::ENDIF);
        return roberto;
    }

    else if (match(Token::WHILE)) {
        e = parseCEXP();
        WhileStmt* roberto = new WhileStmt(e);
        match(Token::DO);
        roberto->cuerpodelwhile.push_back(parsestmt());
        while(match(Token::SEMICOL)){
            roberto->cuerpodelwhile.push_back(parsestmt());
        }
        match(Token::ENDWHILE);
        return roberto;
    }

    else if (match(Token::ID)) {
        string texto = previous->text;
        match(Token::ASSIGN);
        e = parseCEXP();
        return new AsignStmt(texto,e);
    }
}

Exp* Parser::parseCEXP() {
    Exp* l = parseE();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op;
        if (previous->type == Token::PLUS){
            op = PLUS_OP;
        }
        else{
            op = MINUS_OP;
        }
        Exp* r = parseE();
        l = new BinaryExp(l, r, op);
    }
    return l;
}



Exp* Parser::parseE() {
    Exp* l = parseT();
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op;
        if (previous->type == Token::MUL){
            op = MUL_OP;
        }
        else{
            op = DIV_OP;
        }
        Exp* r = parseT();
        l = new BinaryExp(l, r, op);
    }
    return l;
}


Exp* Parser::parseT() {
    Exp* l = parseF();
    if (match(Token::POW)) {
        BinaryOp op = POW_OP;
        Exp* r = parseF();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

Exp* Parser::parseF() {
    Exp* e; 
    if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text));
    }
    else if (match(Token::ID)) {
        string texto = previous->text;
        if(match(Token::LPAREN)){
            FcallExp* fcall = new FcallExp();
            fcall -> nombre = texto;
            fcall -> argumentos.push_back(parseCEXP());
            while(match(Token::COMA)){
                fcall -> argumentos.push_back(parseCEXP());
            }
            match(Token::RPAREN);
            return fcall;
        }
        else{
            return new IdExp(texto);
        }
    }
    else if (match(Token::LPAREN))
    {
        e = parseCEXP();
        match(Token::RPAREN);
        return e;
    }
    else if (match(Token::SQRT))
    {   
        match(Token::LPAREN);
        e = parseCEXP();
        match(Token::RPAREN);
        return new SqrtExp(e);
    }
    else {
        throw runtime_error("Error sintáctico");
    }
}
