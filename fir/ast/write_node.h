#ifndef __FIR_AST_WRITE_NODE_H__
#define __FIR_AST_WRITE_NODE_H__

#include <cdk/ast/sequence_node.h>

namespace fir {

  /**
   * Class for describing write nodes.
   */
  class write_node: public cdk::basic_node {
    cdk::sequence_node *_argument;
    bool _newline;

  public:
    inline write_node(int lineno, cdk::sequence_node *argument, bool newline) :
        cdk::basic_node(lineno), _argument(argument), _newline(newline) {
    }

  public:
    inline cdk::sequence_node *argument() {
      return _argument;
    }

    inline bool newline(){
      return _newline;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_write_node(this, level);
    }

  };

} // fir

#endif
