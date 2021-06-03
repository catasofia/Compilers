#ifndef __FIR_AST_SIZE_OF_NODE_H__
#define __FIR_AST_SIZE_OF_NODE_H__

#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>

namespace fir {
    /**
   * Class for describing size_of nodes.
   */

  class size_of_node: public cdk::expression_node {
    cdk::expression_node *_val;

  public:
    inline size_of_node(int lineno, cdk::expression_node *value) :
        cdk::expression_node(lineno), _val(value) {
    }

  public:

    inline cdk::expression_node *value(){
      return _val;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_size_of_node(this, level);
    }

  };

} // fir

#endif
 