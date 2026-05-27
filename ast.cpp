#include "ast.h"
#include <iostream>

using namespace std;

// ------------------ Exp ------------------
Exp::~Exp() {}

string Exp::binopToChar(BinaryOp op) {
    switch (op) {
        case PLUS_OP:  return "+";
        case MINUS_OP: return "-";
        case MUL_OP:   return "*";
        case DIV_OP:   return "/";
        case POW_OP:   return "**";
        default:       return "?";
    }
}

// ------------------ BinaryExp ------------------
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp o)
    : left(l), right(r), op(o) {}

    
BinaryExp::~BinaryExp() {
    delete left;
    delete right;
}



// ------------------ NumberExp ------------------
NumberExp::NumberExp(int v) : value(v) {}

NumberExp::~NumberExp() {}


// ------------------ SqrtExp ------------------
SqrtExp::SqrtExp(Exp* v) : value(v) {}

SqrtExp::~SqrtExp() {}

//
Programa::Programa() {}
Programa::~Programa(){}

FcallExp::FcallExp(){}
FcallExp::~FcallExp(){}

Fundec::Fundec(){}
Fundec::~Fundec(){}

ReturnStm::ReturnStm(){}
ReturnStm::~ReturnStm(){}

Body::Body() {}
Body::~Body(){}

Stmt::~Stmt(){}

PrintStmt::PrintStmt(Exp* e) {
    exp=e;
}

PrintStmt::~PrintStmt() {

}

IfStmt::IfStmt(Exp* e){
    condicion = e;
}

IfStmt::~IfStmt(){
}

VarDec::VarDec(){
}

VarDec::~VarDec(){
}


WhileStmt::WhileStmt(Exp* e){
    condicion = e;
}

WhileStmt::~WhileStmt(){
}


AsignStmt::AsignStmt(string texto, Exp * e) {
    variable=  texto;
    exp = e;
}

AsignStmt::~AsignStmt() {

}

// ------------------ NumberExp ------------------
IdExp::IdExp(string v) : value(v) {}

IdExp::~IdExp() {}
