#ifndef __FIR_AST_FUNCTION_DECLARATION_NODE_H__
#define __FIR_AST_FUNCTION_DECLARATION_NODE_H__

#include <cdk/ast/basic_node.h>
#include <cdk/ast/sequence_node.h>


namespace fir {

  /**
   * Class for describing function declaration nodes.
   */
  class function_declaration_node: public cdk::typed_node {
    bool _isPublic; 
    bool _inUse;
    std:: string _id;
    cdk:: sequence_node *_args;


  public:
    inline function_declaration_node(int lineno, bool isPublic, bool inUse,
      std::shared_ptr<cdk::basic_type> fType, std::string *id, cdk::sequence_node *args):
        cdk::typed_node(lineno), _isPublic(isPublic), _inUse(inUse),
        _id(*id), _args(args) {
          type(fType);
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

    cdk::typed_node* arg(size_t i) {
      return dynamic_cast<cdk::typed_node*>(_args->node(i));
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_declaration_node(this, level);
    }

  };

} // fir

#endif
