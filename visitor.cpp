#include "visitor.h"
#include "ast.h"
#include <cmath>
#include <fstream>
#include <iostream>

using namespace std;

///////////////////////////////////////////////////////////////////////////////////
int BinaryExp::accept(Visitor *visitor) { return visitor->visit(this); }

int NumberExp::accept(Visitor *visitor) { return visitor->visit(this); }

int IdExp::accept(Visitor *visitor) { return visitor->visit(this); }

int SqrtExp::accept(Visitor *visitor) { return visitor->visit(this); }

void PrintStmt::accept(Visitor *visitor) { visitor->visit(this); }

void AsignStmt::accept(Visitor *visitor) { visitor->visit(this); }

void IfStmt::accept(Visitor *visitor) { visitor->visit(this); }

void WhileStmt::accept(Visitor *visitor) { visitor->visit(this); }

void BreakStmt::accept(Visitor *visitor) { visitor->visit(this); }

void VarDec::accept(Visitor *visitor) { visitor->visit(this); }

void Body::accept(Visitor *visitor) { visitor->visit(this); }

int FcallExp::accept(Visitor *visitor) { return visitor->visit(this); }

void ReturnStm::accept(Visitor *visitor) { visitor->visit(this); }

void Fundec::accept(Visitor *visitor) { visitor->visit(this); }

void Programa::accept(Visitor *visitor) { visitor->visit(this); }

///////////////////////////////////////////////////////////////////////////////////

int PrintVisitor::visit(BinaryExp *exp) {
  exp->left->accept(this);
  cout << ' ' << Exp::binopToChar(exp->op) << ' ';
  exp->right->accept(this);
  return 0;
}

int PrintVisitor::visit(NumberExp *exp) {
  cout << exp->value;
  return 0;
}

int PrintVisitor::visit(SqrtExp *exp) {
  cout << "sqrt(";
  exp->value->accept(this);
  cout << ")";
  return 0;
}

void PrintVisitor::imprimir(Programa *programa) {
  if (programa) {
    cout << "Codigo:" << endl;
    programa->accept(this);
    cout << endl;
  }
  return;
}

///////////////////////////////////////////////////////////////////////////////////
int EVALVisitor::visit(BinaryExp *exp) {
  int result;
  int v1 = exp->left->accept(this);
  int v2 = exp->right->accept(this);

  switch (exp->op) {
    // --- Operadores Aritméticos ---
    case PLUS_OP:
      result = v1 + v2;
      break;
    case MINUS_OP:
      result = v1 - v2;
      break;
    case MUL_OP:
      result = v1 * v2;
      break;
    case DIV_OP:
      if (v2 != 0)
        result = v1 / v2;
      else {
        cout << "Error: división por cero" << endl;
        result = 0;
      }
      break;
    case POW_OP:
      result = pow(v1, v2);
      break;
    case MENOR_OP:
      result = (v1 < v2) ? 1 : 0;
      break;
    case MENORIGUAL_OP:
      result = (v1 <= v2) ? 1 : 0;
      break;
    case IGUALIGUAL_OP:
      result = (v1 == v2) ? 1 : 0;
      break;
    case AND_OP:
      result = (v1 != 0 && v2 != 0) ? 1 : 0;
      break;
    case OR_OP:
      result = (v1 != 0 || v2 != 0) ? 1 : 0;
      break;
    default:
      cout << "Operador desconocido" << endl;
      result = 0;
  }
  return result;
}


int EVALVisitor::visit(NumberExp *exp) { return exp->value; }

int EVALVisitor::visit(SqrtExp *exp) {
  return floor(sqrt(exp->value->accept(this)));
}

void EVALVisitor::interprete(Programa *programa) {
  if (programa) {
    cout << "Interprete:" << endl;
    programa->accept(this);
    cout << endl;
  }
  return;
}

void EVALVisitor::visit(AsignStmt *stm) {
  memoria.update(stm->variable, stm->exp->accept(this));
}

int EVALVisitor::visit(IdExp *e) { return memoria.lookup(e->value); }

void EVALVisitor::visit(PrintStmt *stm) {
  cout << stm->exp->accept(this);
  cout << endl;
}

void PrintVisitor::visit(AsignStmt *stm) {
  cout << stm->variable << " = ";
  stm->exp->accept(this);
  cout << endl;
}
void PrintVisitor::visit(PrintStmt *stm) {
  cout << "print (";
  stm->exp->accept(this);
  cout << ")" << endl;
}

int PrintVisitor::visit(IdExp *e) {
  cout << e->value;
  return 0;
}

void PrintVisitor::visit(IfStmt *stm) {
  cout << "if ";
  stm->condicion->accept(this);
  cout << " then " << endl;
  stm->cuerpodelif->accept(this);
  if (stm->hayelse) {
    cout << "else" << endl;
    stm->cuerpodelelse->accept(this);
  }
  cout << "endif" << endl;
}

void EVALVisitor::visit(IfStmt *stm) {
  if (stm->condicion->accept(this) != 0) {
    stm->cuerpodelif->accept(this);
  } else if (stm->hayelse) {
    stm->cuerpodelelse->accept(this);
  }
}

void PrintVisitor::visit(WhileStmt *stm) {
  cout << "while ";
  stm->condicion->accept(this);
  cout << " do " << endl;
  stm->cuerpo->accept(this);
  cout << "endwhile" << endl;
}

void PrintVisitor::visit(BreakStmt *stm) { cout << "break" << endl; }

void EVALVisitor::visit(WhileStmt *stm) {
  while (stm->condicion->accept(this) != 0) {
    breakSignal = false;
    stm->cuerpo->accept(this);
    if (returnSignal) {
      break;
    }
    if (breakSignal) {
      breakSignal = false;
      break;
    }
  }
}

void EVALVisitor::visit(BreakStmt *stm) { breakSignal = true; }

void PrintVisitor::visit(VarDec *decl) {
  cout << "var " << decl->tipo << " ";
  for (auto i : decl->chicharron) {
    cout << i << ",";
  }
  cout << endl;
}

void EVALVisitor::visit(VarDec *decl) {
  for (auto i : decl->chicharron) {
    memoria.add_var(i);
  }
}

void EVALVisitor::visit(Body *p) {
  memoria.add_level();
  for (auto i : p->vdlist) {
    i->accept(this);
  }
  for (auto i : p->slist) {
    i->accept(this);
    if (breakSignal || returnSignal) {
      break;
    }
  }
  memoria.remove_level();
}

void PrintVisitor::visit(Body *p) {
  for (auto i : p->vdlist) {
    i->accept(this);
  }

  for (auto i : p->slist) {
    i->accept(this);
  }
}

void PrintVisitor::visit(Programa *p) {
  for (auto i : p->vdlist) {
    i->accept(this);
  }
  for (auto i : p->fdlist) {
    i->accept(this);
  }
}
void PrintVisitor::visit(Fundec *fd) {
  cout << "fun " << fd->tipo << " " << fd->nombre << "(";
  for (int i = 0; i < fd->tipo_parametros.size(); i++) {
    cout << fd->tipo_parametros[i] << " " << fd->id_parametros[i] << ",";
  }
  cout << "):" << endl;
  fd->cuerpo->accept(this);
  cout << "endfun" << endl;
}

void PrintVisitor::visit(ReturnStm *stm) {
  cout << "return ";
  stm->exp->accept(this);
  cout << endl;
}
int PrintVisitor::visit(FcallExp *exp) {
  cout << exp->nombre << "(";
  for (auto i : exp->argumentos) {
    i->accept(this);
    cout << ",";
  }
  cout << ")";
  return 0;
}

void EVALVisitor::visit(Programa *p) {
  memoria.add_level();
  for (auto i : p->vdlist) {
    i->accept(this);
  }
  for (auto i : p->fdlist) {
    fmemoria[i->nombre] = i;
  }
  fmemoria["main"]->accept(this);
}

void EVALVisitor::visit(Fundec *fd) { fd->cuerpo->accept(this); }

int EVALVisitor::visit(FcallExp *exp) {
  int retornoPrevio = retornito;
  bool returnPrevio = returnSignal;

  retornito = 0;
  returnSignal = false;

  memoria.add_level();
  Fundec *f = fmemoria[exp->nombre];
  for (int i = 0; i < f->id_parametros.size(); i++) {
    memoria.add_var(f->id_parametros[i], exp->argumentos[i]->accept(this));
  }
  f->cuerpo->accept(this);
  memoria.remove_level();

  int resultado = retornito;
  retornito = retornoPrevio;
  returnSignal = returnPrevio;
  return resultado;
}

void EVALVisitor::visit(ReturnStm *stm) {
  retornito = stm->exp->accept(this);
  returnSignal = true;
}
