#ifndef __FIR_AST_ADDRESS_NODE_H__
#define __FIR_AST_ADDRESS_NODE_H__

#include <cdk/ast/unary_operation_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/variable_node.h>

namespace fir {
    /**
   * Class for describing address nodes.
   */

  class address_node: public cdk::expression_node {
    cdk::lvalue_node *_value;

  public:
    inline address_node(int lineno, cdk::lvalue_node *value) :
        cdk::expression_node(lineno), _value(value) {
    }

  public:
    inline cdk::lvalue_node *value(){
      return _value;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_address_node(this, level);
    }

  };

} // fir

#endif
