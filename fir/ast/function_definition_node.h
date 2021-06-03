#ifndef __FIR_AST_FUNCTION_DEFINITION_NODE_H__
#define __FIR_AST_FUNCTION_DEFINITION_NODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/typed_node.h>
#include <cdk/ast/expression_node.h>


namespace fir {

  /**
   * Class for describing function_definition nodes.
   */
  class function_definition_node: public cdk::typed_node {
    bool _isPublic; 
    bool _inUse;   
    std:: string _id;
    cdk::sequence_node *_args;
    block_node *_block;
    block_node *_epilogue;
    prologue_node *_prologue;
    cdk::expression_node *_default_return;

  public:
    inline function_definition_node(int lineno, bool isPublic, bool inUse, std:: string *id, 
      cdk::sequence_node *args, block_node *block, block_node *epilogue, prologue_node *prologue, cdk::expression_node *default_return) :
        cdk::typed_node(lineno), _isPublic(isPublic), _inUse(inUse), _id(*id),
         _args(args), _block(block), _epilogue(epilogue), _prologue(prologue), _default_return(default_return) {
        type(cdk::primitive_type::create(0, cdk::TYPE_VOID));
    }
    inline function_definition_node(int lineno, std::shared_ptr<cdk::basic_type> funType, bool isPublic, bool inUse, std:: string *id, 
      cdk::sequence_node *args, block_node *block, block_node *epilogue, prologue_node *prologue, cdk::expression_node *default_return) :
        cdk::typed_node(lineno), _isPublic(isPublic), _inUse(inUse), _id(*id),
         _args(args), _block(block), _epilogue(epilogue), _prologue(prologue), _default_return(default_return) {
        type(funType);
    }

  public:
    inline bool isPublic() {
      return _isPublic;
    }

    inline bool inUse() {
      return _inUse;
    }

    inline std::string id() {
      return _id;
    }

    inline void setId(std::string id){
      _id = id;
    }

    inline cdk::sequence_node *args() {
      return _args;
    }

    inline block_node *block(){
      return _block;
    }

    inline block_node *epilogue(){
      return _epilogue;
    }

    inline prologue_node *prologue(){
      return _prologue;
    }

    inline cdk::expression_node *default_value(){
      return _default_return;
    }

    cdk::typed_node* arg(size_t i) {
      return dynamic_cast<cdk::typed_node*>(_args->node(i));
    }
    
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_definition_node(this, level);
    }

  };

} // fir

#endif
