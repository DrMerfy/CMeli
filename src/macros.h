#ifndef MACROS_H
#define MACROS_H

#include <string.h>

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
#define VBinop 15

#define AssigEQ 16
#define AssigPLE 17
#define AssigMNE 18
#define AssigMLE 19
#define AssigSBE 20
#define AssigMDE 21

#define BiPLUS 22
#define BiMINUS 23
#define BiMULT 24
#define BiSUB 25
#define BiDIV 26
#define BiLS 27
#define BiLE 28
#define BiGT 29
#define BiGE 30
#define BiEQ 31
#define BiNE 32
#define BiAND 33
#define BiOR 34

#define OpNOT 35
#define OpNEG 36

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

    case 15: return "VBinop";

    case 16: return "AssigEQ";

    case 17: return "AssigPLE";

    case 18: return "AssigMNE";

    case 19: return "AssigMLE";

    case 20: return "AssigSBE";

    case 21: return "AssigMDE";

    case 22: return "BiPLUS";

    case 23: return "BiMINUS";

    case 24: return "BiMULT";

    case 25: return "BiSUB";

    case 26: return "BiDIV";

    case 27: return "BiLS";

    case 28: return "BiLE";

    case 29: return "BiGT";

    case 30: return "BiGE";

    case 31: return "BiEQ";

    case 32: return "BiNE";

    case 33: return "BiAND";

    case 34: return "BiOR";

    case 35: return "OpNOT";

    case 36: return "OpNEG";

    default:
              printf("Undefined %d ", macro);
              return "Undefined";
  }
}


#endif
