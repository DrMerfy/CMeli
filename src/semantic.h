#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "tree.h"

/* Parses the tree from the given node downwards and finds if it leads to one leaf.
 * If it leads to only one leaf it returns the leaf.
 */
node* _is_one_way(node* n) {
  if (!n)
    return NULL;

  // If it has more than one children, it is not one way
  if (n->children_count > 1)
    return NULL;

  if (n->children_count == 0)
    return n;

  _is_one_way(n->children[0]);
}

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
    node* possibleLeaf = _is_one_way(n);
    printf("Possible lead %p\n", possibleLeaf);
    if(!possibleLeaf)
      return;

    if (possibleLeaf->type == TYPECONSTANT) {
      if (possibleLeaf->sym->value == 0)
        warning(ALWAYSFALSE, possibleLeaf->line_number, WHEREIF);
      else
        warning(ALWAYSTRUE, possibleLeaf->line_number, WHEREIF);
    }
  }

}

void analyze_tree() {
  _optimazation(NULL, root, 0);
  _semantical_analysis(root);
}

#endif
