#ifndef MACROS_H
#define MACROS_H

#include <string.h>

typedef int bool;
enum { false, true };

/* <------- SYNTACTICAL ANALYSIS CONSTS -------> */
#define VProgram 1
#define VDecls 2
#define VDecl 3
#define VVars 4
#define VType 5
#define VStmts 6
#define VStmt 7
#define VSimp 8
#define VControl 9
#define VAsop 10
#define VExp 11
#define VBlock 12
#define VElseBlock 13
#define VUnop 14
#define VBinopExp 15
#define VBinop1 16
#define VBinop2 17

#define AssigEQ 18
#define AssigPLE 19
#define AssigMNE 20
#define AssigMLE 21
#define AssigSBE 22
#define AssigMDE 23

#define BiPLUS 24
#define BiMINUS 25
#define BiMULT 26
#define BiSUB 27
#define BiDIV 28
#define BiLS 29
#define BiLE 30
#define BiGT 31
#define BiGE 32
#define BiEQ 33
#define BiNE 34
#define BiAND 35
#define BiOR 36

#define OpNOT 37
#define OpNEG 38

#define VFactor 39

#define TYPEVARIABLE 40
#define TYPECONSTANT 41
#define TYPEINT 42
#define TYPEOPERATOR 43

char* macro_to_string(int macro) {

  switch (macro) {
    case 1: return "VProgram";
    case 2: return "VDecls";
    case 3: return "VDecl";
    case 4: return "VVars";
    case 5: return "VType";
    case 6: return "VStmts";
    case 7: return "VStmt";
    case 8: return "VSimp";
    case 9: return "VControl";
    case 10: return "VAsop";
    case 11: return "VExp";
    case 12: return "VBlock";
    case 13: return "VElseBlock";
    case 14: return "VUnop";
    case 15: return "VBinopExp";
    case 16: return "VBinop1";
    case 17: return "VBinop2";

    case 18: return "AssigEQ";
    case 19: return "AssigPLE";
    case 20: return "AssigMNE";
    case 21: return "AssigMLE";
    case 22: return "AssigSBE";
    case 23: return "AssigMDE";

    case 24: return "BiPLUS";
    case 25: return "BiMINUS";
    case 26: return "BiMULT";
    case 27: return "BiSUB";
    case 28: return "BiDIV";
    case 29: return "BiLS";
    case 30: return "BiLE";
    case 31: return "BiGT";
    case 32: return "BiGE";
    case 33: return "BiEQ";
    case 34: return "BiNE";
    case 35: return "BiAND";
    case 36: return "BiOR";

    case 37: return "OpNOT";
    case 38: return "OpNEG";

    case 39: return "VFactor";
    default:
              printf("Undefined %d ", macro);
              return "Undefined";
  }
}

bool isBinaryOperator(int op) {
  switch (op) {
    case BiPLUS:
    case BiMINUS:
    case BiMULT:
    case BiSUB:
    case BiDIV:
    case BiLS:
    case BiLE:
    case BiGT:
    case BiGE:
    case BiEQ:
    case BiNE:
    case BiAND:
    case BiOR:
      return true;
  }
  return false;
}


#endif
