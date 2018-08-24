#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "tree.h"


void _optimazation(node* parent, node* n, int child_no) {
  if (!n)
    return;

  for(int i = 0; i < n->children_count; i++) {
    _optimazation(n, n->children[i], i);
  }

  if (n->sym)
    return;

  if (isBinaryOperator(n->type)) {
    // Check if the binary operator consists of two constants
    // then do operation here
    if (n->children_count == 2) {
      node* possibleLeaf1 = _is_one_way(n->children[0]);
      node* possibleLeaf2 = _is_one_way(n->children[1]);
      if (!possibleLeaf1 || !possibleLeaf2)
        return;

      if (possibleLeaf1->type == TYPECONSTANT && possibleLeaf2->type == TYPECONSTANT) {
        int v1 = possibleLeaf1->sym->value;
        int v2 = possibleLeaf2->sym->value;

        int level = 1;
        int value;
        switch (n->type) {
          case BiPLUS:
            value = v1 + v2;
            break;
          case BiMINUS:
            value = v1 - v2;
            break;
          case BiMULT:
            value = v1 * v2;
            level = 2;
            break;
          case BiSUB:
            if (v2 == 0) {
              error(DIVBYZERO, line_number, NULL);
              break;
            }
            level = 2;
            value = v1 / v2;
            break;
          case BiDIV:
            value = v1 % v2;
            level = 2;
            break;
          case BiLS:
            value = v1 < v2;
            level = 0;
            break;
          case BiLE:
            value = v1 <= v2;
            level = 0;
            break;
          case BiGT:
            value = v1 > v2;
            level = 0;
            break;
          case BiGE:
            value = v1 >= v2;
            level = 0;
            break;
          case BiEQ:
            value = v1 == v2;
            level = 0;
            break;
          case BiNE:
            value = v1 != v2;
            level = 0;
            break;
          case BiAND:
            value = v1 && v2;
            level = 0;
            break;
          case BiOR:
            value = v1 || v2;
            level = 0;
            break;
        }

        if (value > abs(MAX_INT_SIZE))
          printf("Warn\n");
        // Clean
        delete_from_node_backward(n);

        // Create and link new node
        if (level == 0)
          n = add_node(VBinop1, add_node(VBinop2, add_node(VFactor, create_node_constant(value))));
        else if (level == 1)
          n = add_node(VBinop2, add_node(VFactor, create_node_constant(value)));
        else
          n = add_node(VFactor, create_node_constant(value));
        parent->children[child_no] = n;
      }
    }
  }

}

void _semantical_analysis(node* n) {

  for(int i = 0; i < n->children_count; i++) {
    _semantical_analysis(n->children[i]);
  }

  if (n->sym)
    return;


  if (n->type == VIF) {
    // Check for constant condition
    node* possibleLeaf = _is_one_way(get_condition(n));
    if (!possibleLeaf)
      return;

    if (possibleLeaf->type == TYPECONSTANT) {
      if (possibleLeaf->sym->value == 0)
        warning(ALWAYSFALSE, possibleLeaf->line_number, WHEREIF);
      else
        warning(ALWAYSTRUE, possibleLeaf->line_number, WHEREIF);
    }
  }

  if (n->type == VWHILE) {
    // Check for constant condition
    node* possibleLeaf = _is_one_way(get_condition(n));
    if (!possibleLeaf)
      return;

    if (possibleLeaf->type == TYPECONSTANT) {
      // Check for break statement
      if (!has_break(n) && possibleLeaf->sym->value > 0)
        warning(ALWAYSTRUE, n->line_number, WHEREWHILE);
      if (possibleLeaf->sym->value == 0)
        warning(ALWAYSFALSE, n->line_number, WHEREWHILE);
    }
  }

  if (n->type == VFOR) {
    node* for_related_var;
    node* assigment = n->children[0];
    node* condition = n->children[1];
    node* operation = n->children[2];

    bool hasBreak = has_break(n);
    // Check assigment
    if (assigment == NULL)
      return;

    if (assigment->type == VPRINT)
      warning(PRINTINASSIGMENT, assigment->line_number, 0);

    for_related_var = get_var_from_assigment(assigment);
    if (for_related_var->sym == NULL)
      return;

    // Check condition
    node* nos[10];
    node** nodes = get_vars(condition, nos);

    bool ok = false;
    for (int i = 0; i < ind; i++) {
      if (!strcmp(nodes[i]->sym->name,for_related_var->sym->name))
        ok = true;
    }

    if (!ok)
      warning(UNRELATEDCONDITION, condition->line_number, 0);
    // Clean up
    ind = 0;

    if (!hasBreak && !ok)
      warning(ALWAYSTRUE, condition->line_number, WHEREFOR);

    // Check for constant condition
    node* possibleLeaf = _is_one_way(condition);

    if (possibleLeaf) {
      if (possibleLeaf->type == TYPECONSTANT) {
        // Check for break statement
        if (!has_break && possibleLeaf->sym->value > 0)
          warning(ALWAYSTRUE, n->line_number, WHEREFOR);
        if (possibleLeaf->sym->value == 0)
          warning(ALWAYSFALSE, n->line_number, WHEREFOR);
      }
    }

    // Check operation
    nodes = get_vars(operation, nos);

    ok = false;
    for (int i = 0; i < ind; i++) {
      if (!strcmp(nodes[i]->sym->name,for_related_var->sym->name))
        ok = true;
    }

    if (!ok)
      warning(UNRELATEDOPERATION, condition->line_number, 0);
    // Clean up
    ind = 0;

    if (!hasBreak && !ok)
      warning(ALWAYSTRUE, condition->line_number, WHEREFOR);

    // Check for constant condition
    possibleLeaf = _is_one_way(operation);
    if (possibleLeaf) {

      if (possibleLeaf->type == TYPECONSTANT) {
        // Check for break statement
        if (!hasBreak && possibleLeaf->sym->value > 0)
          warning(ALWAYSTRUE, n->line_number, WHEREFOR);
        if (possibleLeaf->sym->value == 0)
          warning(ALWAYSFALSE, n->line_number, WHEREFOR);
      }
    }

    // Check for easy issues
    if (operation->type == VPRINT){
      warning(PRINTOPERATION, operation->line_number, 0);
      if (!hasBreak)
        warning(ALWAYSTRUE, n->line_number, WHEREFOR);
    }else if (operation->children[0]->type == AssigEQ) {
      warning(ASSIGMENTOPERATION, operation->line_number, 0);
      if (!hasBreak)
        warning(ALWAYSTRUE, n->line_number, WHEREFOR);
    }

  }
}


void analyze_tree() {
  _optimazation(NULL, root, 0);
  _semantical_analysis(root);
}

#endif
