#ifndef __FIR_AST_BLOCK_NODE_H__
#define __FIR_AST_BLOCK_NODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/ast/sequence_node.h>

namespace fir {

  /**
   * Class for describing block nodes.
   */
  class block_node: public cdk::basic_node {
    cdk::sequence_node *_decl, *_inst;
    bool _isEpilogue;


  public:
    inline block_node(int lineno, cdk::sequence_node *decl, cdk::sequence_node *inst, bool isEpilogue):
        cdk::basic_node(lineno), _decl(decl), _inst(inst), _isEpilogue(isEpilogue) {
    }

  public:
    inline cdk::sequence_node *decl() {
      return _decl;
    }

    inline cdk::sequence_node *inst() {
      return _inst;
    }

    inline bool isEpilogue(){
      return _isEpilogue;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_block_node(this, level);
    }

  };

} // fir

#endif
