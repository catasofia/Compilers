#ifndef __FIR_AST_VARIABLE_DECL_H__
#define __FIR_AST_VARIABLE_DECL_H__

#include <cdk/ast/basic_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/typed_node.h>

namespace fir {
  /**
   * Class for describing variable declaration nodes.
  */
  class variable_decl_node: public cdk::typed_node {
    bool _ispublic;
    bool _isactive; //to know if it is active (global variable or inside its function)
    std::string _id;
    cdk::expression_node *_value;

  public:
    inline variable_decl_node(int lineno, bool ispublic, bool isactive, const std::string &id,
      std::shared_ptr<cdk::basic_type> vType, cdk::expression_node *value) :
      cdk::typed_node(lineno), _ispublic(ispublic), _isactive(isactive), _id(id), _value(value){
        type(vType);
    }

  public:
    inline bool ispublic(){
      return _ispublic;
    } 

    inline bool isactive(){
      return _isactive;
    }

    inline std::string& id() {
      return _id;
    }

    inline cdk::expression_node *value(){
      return _value;
    }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_variable_decl_node(this, level);
    }

  };

} // fir

#endif
