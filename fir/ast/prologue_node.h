#ifndef __FIR_AST_PROLOGUE_NODE_H__
#define __FIR_AST_PROLOGUE_NODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/ast/sequence_node.h>

namespace fir {

  /**
   * Class for describing prologue nodes.
   */
  class prologue_node: public cdk::basic_node {
    cdk::sequence_node *_decl, *_inst;


  public:
    inline prologue_node(int lineno, cdk::sequence_node *decl, cdk::sequence_node *inst):
        cdk::basic_node(lineno), _decl(decl), _inst(inst) {
    }

  public:
    inline cdk::sequence_node *decl() {
      return _decl;
    }

    inline cdk::sequence_node *inst() {
      return _inst;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_prologue_node(this, level);
    }

  };

} // fir

#endif
