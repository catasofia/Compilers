#ifndef __FIR_TARGETS_POSTFIX_WRITER_H__
#define __FIR_TARGETS_POSTFIX_WRITER_H__

#include "targets/basic_ast_visitor.h"

#include <sstream>
#include <cdk/emitters/basic_postfix_emitter.h>
#include <set>

namespace fir {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<fir::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl;
    std::vector<std::string> _restart;
    std::vector<std::string> _leave;
    std::string _currentBodyRetLabel;
    std::set<std::string> _functions_to_declare;
    bool _inFinally = false;
    bool _isGlobal;
    bool _publicVar;
    bool _isArgument;
    int _prologue;
    size_t _funcOffset;
    size_t _varOffset;
    std::string _varLbl;
    std::shared_ptr<fir::symbol> _function;


  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<fir::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0), _currentBodyRetLabel(""), _isGlobal(true), _isArgument(false), _prologue(0), _function(nullptr) {
    }

  public:
    ~postfix_writer() {
      os().flush();
    }

    inline bool inFinally(){
      return _inFinally;
    }

    inline bool isGlobal(){
      return _isGlobal;
    }

    inline void setGlobal(bool global){
      _isGlobal = global;
    }

    inline bool isGlobalVar(){
      return _publicVar;
    }

    inline bool isArgument(){
      return _isArgument;
    }

    inline void isArgument(bool arg){
      _isArgument = arg;
    }

    inline void varOffset(size_t offset){
      _varOffset += offset;
    }
    
    inline void funcOffset(size_t offset){
      _funcOffset -= offset;
    }

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include "ast/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // fir

#endif
