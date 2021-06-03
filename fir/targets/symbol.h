#ifndef __FIR_TARGETS_SYMBOL_H__
#define __FIR_TARGETS_SYMBOL_H__

#include <string>
#include <memory>
#include <cdk/types/basic_type.h>

namespace fir {

  class symbol {
    std::shared_ptr<cdk::basic_type> _type;
    std::string _id;
    int _offset;
    bool _isPublic;
    bool _inUse;
    bool _isFunction;
    std::vector<std::shared_ptr<cdk::basic_type>> _args;
    

  public:
    symbol(std::shared_ptr<cdk::basic_type> type, const std::string &id, int offset, bool isPublic,
    bool inUse, bool isFunction, std::vector<std::shared_ptr<cdk::basic_type>> args):
        _type(type), _id(id), _offset(offset), _isPublic(isPublic), _inUse(inUse), _isFunction(isFunction), _args(args) {
    }

    symbol(std::shared_ptr<cdk::basic_type> type, const std::string &id, int offset, bool isPublic,
    bool inUse, bool isFunction):
        _type(type), _id(id), _offset(offset), _isPublic(isPublic), _inUse(inUse), _isFunction(isFunction){
    }

    virtual ~symbol() {
      // EMPTY
    }

    std::shared_ptr<cdk::basic_type> type() const {
      return _type;
    }
    bool is_typed(cdk::typename_type id) const {
      return _type->name() == id;
    }
    const std::string &id() const {
      return _id;
    }
    bool is_function() {
      return _isFunction;
    }
    bool in_use() {
      return _inUse;
    }
    void in_use(bool use) {
      _inUse = use;
    }
    bool is_public() {
      return _isPublic;
    }
    void is_public(bool ispublic) {
      _isPublic = ispublic;
    }
    int offset(){
      return _offset;
    }
    void offset(int offset){
      _offset = offset;
    }
    std::vector<std::shared_ptr<cdk::basic_type>> args(){
      return _args;
    }

  };

} // fir

#endif
