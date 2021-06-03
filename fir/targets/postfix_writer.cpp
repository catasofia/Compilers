#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "targets/frame_size_calculator.h"
#include "ast/all.h"  // all.h is automatically generated

//---------------------------------------------------------------------------

void fir::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void fir::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}
void fir::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if(isGlobal()){
    _pf.SDOUBLE(node->value());
  }
  else{
    _pf.DOUBLE(node->value());
  }
}
void fir::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->argument()->accept(this, lvl);
  _pf.NOT();
}
void fir::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  int lbl = ++_lbl;
  node->left()->accept(this, lvl);
  _pf.DUP32();
  _pf.JZ(mklbl(lbl));
  node->right()->accept(this, lvl);
  _pf.AND();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}
void fir::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  int lbl = ++_lbl;
  node->left()->accept(this, lvl);
  _pf.DUP32();
  _pf.JNZ(mklbl(lbl));
  node->right()->accept(this, lvl);
  _pf.OR();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if(isGlobal()){
    _pf.SINT(node->value());
  }
  else{
    _pf.INT(node->value());
  }
}

void fir::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  int lbl1;
  /* generate the string */
  _pf.RODATA(); // strings are DATA readonly
  _pf.ALIGN(); // make sure we are aligned
  _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
  _pf.SSTRING(node->value()); // output string characters

  if(isGlobal()){
    _pf.DATA();
    _pf.SADDR(mklbl(lbl1));
  }
  else{
    _pf.TEXT();
    _pf.ADDR(mklbl(lbl1));
  }
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  if(node->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DNEG(); // 2-complement
  else
    _pf.NEG();
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl);

  if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT) {
    _pf.I2D();
  }

  if(node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
    if(std::dynamic_pointer_cast<cdk::reference_type>(node->type())->referenced()->name() == cdk::TYPE_DOUBLE) {
      _pf.INT(3);
    }
    else {
      _pf.INT(2);
    }
    _pf.SHTL();
  }


  if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT) {
    _pf.I2D();
  } 
  else if(node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    if(std::dynamic_pointer_cast<cdk::reference_type>(node->type())->referenced()->name() == cdk::TYPE_DOUBLE) {
      _pf.INT(3);
    }
    else {
      _pf.INT(2);
    }
    _pf.SHTL();
  }

  if (node->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DADD();
  else
    _pf.ADD();
}

void fir::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT) {
    _pf.I2D();
  }

  else if(node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
    if(std::dynamic_pointer_cast<cdk::reference_type>(node->type())->referenced()->name() == cdk::TYPE_DOUBLE) {
      _pf.INT(3);
    }
    else {
      _pf.INT(2);
    }
    _pf.SHTL();
  }

  node->right()->accept(this, lvl);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT) {
    _pf.I2D();
  } 
  else if(node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    if(std::dynamic_pointer_cast<cdk::reference_type>(node->type())->referenced()->name() == cdk::TYPE_DOUBLE) {
      _pf.INT(3);
    }
    else {
      _pf.INT(2);
    }
    _pf.SHTL();
  }

  if (node->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DSUB();
  else
    _pf.SUB();
}
void fir::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT) {
    _pf.I2D();
  }

  node->right()->accept(this, lvl);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT) {
    _pf.I2D();
  }

  if(node->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DMUL();
  else
    _pf.MUL();
}

void fir::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->left()->type()->name() == cdk::TYPE_INT) {
    _pf.I2D();
  }

  node->right()->accept(this, lvl);
  if (node->type()->name() == cdk::TYPE_DOUBLE && node->right()->type()->name() == cdk::TYPE_INT) {
    _pf.I2D();
  }

  if(node->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DDIV();
  else
    _pf.DIV();
}

void fir::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}

void fir::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl);
  if(node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE){
    _pf.I2D();
  }
  
  node->right()->accept(this, lvl);
  if(node->right()->type()->name() == cdk::TYPE_INT && node->left()->type()->name() == cdk::TYPE_DOUBLE){
    _pf.I2D();
  }

  if(node->left()->type()->name() == cdk::TYPE_DOUBLE || node->left()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DCMP();
  
  else
    _pf.LT();
}

void fir::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);

  if(node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE){
    _pf.I2D();
  }
  
  if(node->right()->type()->name() == cdk::TYPE_INT && node->left()->type()->name() == cdk::TYPE_DOUBLE){
    _pf.I2D();
  }

  if(node->left()->type()->name() == cdk::TYPE_DOUBLE || node->left()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DCMP();
  
  else
    _pf.LE();
}

void fir::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl);
  if(node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE){
    _pf.I2D();
  }
  
  node->right()->accept(this, lvl);
  if(node->right()->type()->name() == cdk::TYPE_INT && node->left()->type()->name() == cdk::TYPE_DOUBLE){
    _pf.I2D();
  }

  if(node->left()->type()->name() == cdk::TYPE_DOUBLE || node->left()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DCMP();
  
  else
    _pf.GE();
}

void fir::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl);
  if(node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE){
    _pf.I2D();
  }
  
  node->right()->accept(this, lvl);
  if(node->right()->type()->name() == cdk::TYPE_INT && node->left()->type()->name() == cdk::TYPE_DOUBLE){
    _pf.I2D();
  }

  if(node->left()->type()->name() == cdk::TYPE_DOUBLE || node->left()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DCMP();
  
  else
    _pf.GT();
}

void fir::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl);
  if(node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE){
    _pf.I2D();
  }
  
  node->right()->accept(this, lvl);
  if(node->right()->type()->name() == cdk::TYPE_INT && node->left()->type()->name() == cdk::TYPE_DOUBLE){
    _pf.I2D();
  }

  if(node->left()->type()->name() == cdk::TYPE_DOUBLE || node->left()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DCMP();
  
  else
    _pf.NE();
}

void fir::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->left()->accept(this, lvl);
  if(node->left()->type()->name() == cdk::TYPE_INT && node->right()->type()->name() == cdk::TYPE_DOUBLE){
    _pf.I2D();
  }
  
  node->right()->accept(this, lvl);
  if(node->right()->type()->name() == cdk::TYPE_INT && node->left()->type()->name() == cdk::TYPE_DOUBLE){
    _pf.I2D();
  }

  if(node->left()->type()->name() == cdk::TYPE_DOUBLE || node->left()->type()->name() == cdk::TYPE_DOUBLE)
    _pf.DCMP();
  
  else
    _pf.EQ();
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  std::shared_ptr<fir::symbol> symb = _symtab.find(node->name());

  if(!symb->is_function() && symb->is_public()){
    _pf.ADDR(symb->id());
  }
  else{
    _pf.LOCAL(symb->offset());
  }
}

void fir::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
  if(!(node->type()->name() == cdk::TYPE_DOUBLE))
    _pf.LDINT();
  else
    _pf.LDDOUBLE();
}

void fir::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->rvalue()->accept(this, lvl); 
  if(node->is_typed(cdk::TYPE_DOUBLE)){
    if(node->rvalue()->is_typed(cdk::TYPE_INT)){
      _pf.I2D();
    }
    _pf.DUP64();
  }
  else
    _pf.DUP32();

  node->lvalue()->accept(this, lvl);
  if(node->lvalue()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.STDOUBLE();
  }
  else {
    _pf.STINT();
  }
  
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_function_definition_node(fir::function_definition_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  _isGlobal = false;

  _pf.TEXT();
  _pf.ALIGN();
  _function = _symtab.find(node->id());
  if(_function==nullptr) throw std::string(node->id() + " not found!");

  _functions_to_declare.erase(_function->id()); 

  _currentBodyRetLabel = mklbl(++_lbl);
  _varOffset = 8; 
  _symtab.push(); 

  if (node->args()->size() > 0) {
    isArgument(true);
    for (size_t ix = 0; ix < node->args()->size(); ix++) {
      cdk::basic_node *arg = node->args()->node(ix);
      if (arg == nullptr) break; 
      arg->accept(this, 0); 
    }
    isArgument(false);
  }


  if (node->isPublic()) {
    _pf.GLOBAL(node->id(), _pf.FUNC());
  }
  _pf.LABEL(node->id());

  frame_size_calculator lsc(_compiler, _symtab, _function);
  node->accept(&lsc, lvl);
  _pf.ENTER(lsc.localsize());
  
  _funcOffset = 0;


  if(node->default_value() != nullptr){
    node->default_value()->accept(this, lvl);
    if(node->default_value()->is_typed(cdk::TYPE_INT) || node->default_value()->is_typed(cdk::TYPE_STRING)
    || node->default_value()->is_typed(cdk::TYPE_POINTER)) {
      _pf.LOCAL(-4);
      _pf.STINT();
    }
    else if(node->default_value()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.LOCAL(-8);
    _pf.STDOUBLE();
    }
  }

  if(node->block() != nullptr){
    node->block()->accept(this, lvl + 4);
  }
  if(node->prologue() != nullptr){
    _prologue = 1;
    node->prologue()->accept(this, lvl + 4);
    _prologue = 0;
  }
  if(node->epilogue() != nullptr)
    node->epilogue()->accept(this, lvl + 4);
  

  _pf.LABEL(_currentBodyRetLabel);


  if(node->id() == "_main"){
    _pf.INT(0);
    _pf.STFVAL32();
  }
  _isGlobal = true;
  _pf.LEAVE();
  _pf.RET();

  _symtab.pop();

  _pf.EXTERN("argc");
  _pf.EXTERN("printi");
  _pf.EXTERN("printd");
  _pf.EXTERN("prints");
  _pf.EXTERN("println");
  _pf.EXTERN("readi");
  _pf.EXTERN("readd");
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_evaluation_node(fir::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  if (node->argument()->is_typed(cdk::TYPE_INT)) {
    _pf.TRASH(4); // delete the evaluated value
  } else if (node->argument()->is_typed(cdk::TYPE_STRING)) {
    _pf.TRASH(4); // delete the evaluated value's address
  } else if (node->argument()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.TRASH(8); // delete the evaluated value's address
  } else if (node->argument()->is_typed(cdk::TYPE_POINTER)) {
    _pf.TRASH(4); // delete the evaluated value's address
  } else if (node->argument()->is_typed(cdk::TYPE_VOID)) {
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_read_node(fir::read_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if (node->is_typed(cdk::TYPE_INT)) {
    _functions_to_declare.insert("readi");
     _pf.CALL("readi");
     _pf.LDFVAL32();
  }
  else if (node->is_typed(cdk::TYPE_DOUBLE)) {
      _functions_to_declare.insert("readd");
     _pf.CALL("readd");
     _pf.LDFVAL64();
  }
  else{
    throw std::string("wrong type to read node");
  }
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_while_do_node(fir::while_do_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  std::string while_cond = mklbl(++_lbl);
  _restart.push_back(while_cond);
  std::string while_end = mklbl(++_lbl);
  _leave.push_back(while_end);

  _pf.LABEL(while_cond);
  node->condition()->accept(this, lvl);
  _pf.JZ(while_end);
  node->block()->accept(this, lvl + 2);
  _pf.JMP(while_cond);
  _pf.LABEL(while_end);

  _restart.pop_back();
  _leave.pop_back();
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_if_node(fir::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void fir::postfix_writer::do_if_else_node(fir::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1 = lbl2));
}

//NEW------------------------------------------------------------------------

void fir::postfix_writer::do_leave_node(fir::leave_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if(inFinally()){
    throw std::string("Impossible to use leave in finally");
  }
  else if(_leave.empty()){
    throw std::string("Not in a cycle");
  }
  else{
    if(node->level() == -1){
      _pf.JMP(_leave.back());
    }
    else if((unsigned)node->level() > _leave.size()){
      throw std::string("There aren't that many cycles");
    }
    else{
      _pf.JMP(_leave.at(node->level()-1));
    }
  }
}

void fir::postfix_writer::do_restart_node(fir::restart_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if(inFinally()){
    throw std::string("Impossible to use leave in finally");
  }
  else if(_restart.empty()){
    throw std::string("Not in a cycle");
  }
  else{
    if(node->level() == -1){
      _pf.JMP(_restart.back());
    }
    else if((unsigned)node->level() > _restart.size()){
      throw std::string("There aren't that many cycles");
    }
    else{
      _pf.JMP(_restart.at(node->level()-1));
    }
  }
}


void fir::postfix_writer::do_return_node(fir::return_node * const node, int lvl) {
  if(_function != nullptr){
    if(_function->is_typed(cdk::TYPE_INT) || _function->is_typed(cdk::TYPE_STRING) ||
      _function->is_typed(cdk::TYPE_POINTER)){
        _pf.STFVAL32();
    }
    else if(_function->is_typed(cdk::TYPE_DOUBLE)){
      _pf.STFVAL64();
    }
    else{
      throw std::string("Unknown return type");
    }

    _pf.LEAVE();
    _pf.RET();
  }else{
    throw std::string("Return not inside a function");
  }
}

void fir::postfix_writer::do_while_do_finally_node(fir::while_do_finally_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  std::string while_cond = mklbl(++_lbl);
  _restart.push_back(while_cond);
  std::string while_end = mklbl(++_lbl);
  _leave.push_back(while_end);
  std::string while_finally = mklbl(++_lbl);

  _pf.LABEL(while_cond);
  node->condition()->accept(this, lvl);
  _pf.JZ(while_finally);
  node->block()->accept(this, lvl + 2);
  _pf.JMP(while_cond);

  _pf.LABEL(while_finally);
  _inFinally = true;
  node->finally()->accept(this, lvl+2);
  _inFinally = false;
  _pf.JMP(while_end);

  _pf.LABEL(while_end);

  _restart.pop_back();
  _leave.pop_back();
}

void fir::postfix_writer::do_write_node(fir::write_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value to print
  for(size_t i = 0; i < node->argument()->size(); i++){
    cdk::expression_node *expression = dynamic_cast<cdk::expression_node *>(node->argument()->node(i));
    
    if (expression->is_typed(cdk::TYPE_INT)) {
      _functions_to_declare.insert("printi");
      _pf.CALL("printi");
      _pf.TRASH(4); // delete the printed value
    } else if (expression->is_typed(cdk::TYPE_STRING)) {
      _functions_to_declare.insert("prints");
      _pf.CALL("prints");
      _pf.TRASH(4); // delete the printed value's address
    } else if (expression->is_typed(cdk::TYPE_DOUBLE)){
      _functions_to_declare.insert("printd");
      _pf.CALL("printd");
      _pf.TRASH(8);
    } else {
      std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
      exit(1);
    }
  }

  if(node->newline())
    _pf.CALL("println"); // print a newline
}

void fir::postfix_writer::do_variable_decl_node(fir::variable_decl_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS; 
  _publicVar = node->ispublic();

  if(_prologue == 1)
    _publicVar = true;

  std::shared_ptr<fir::symbol> symb = _symtab.find(node->id());
  
  if(symb != nullptr && isArgument()){
    symb->offset(_varOffset);
    varOffset(symb->type()->size());
    symb->is_public(false);
  }
  else if(symb != nullptr && _isGlobal){
    symb->is_public(true);
    symb->offset(0);
    if(_publicVar)
      _pf.GLOBAL(node->id(), _pf.OBJ());
    _varLbl = node->id();
    if(!node->value()){
      _pf.BSS();
      _pf.ALIGN();
      _pf.LABEL(node->id());
      _pf.SBYTE(node->type()->size());
    }
    else{
      _pf.DATA();
      _pf.ALIGN();
      _pf.LABEL(node->id());

      if(node->is_typed(cdk::TYPE_DOUBLE) && node->value()->is_typed(cdk::TYPE_INT)){
        cdk::integer_node *intNode = dynamic_cast<cdk::integer_node*>(node->value());
        cdk::double_node* i2dNode = new cdk::double_node(intNode->lineno(), intNode->value());
        i2dNode->accept(this, lvl);
      } else{
        node->value()->accept(this, lvl);
      }
    }
  }
  
  else if(symb != nullptr){
    symb->is_public(false);
    funcOffset(node->type()->size());
    symb->offset(_funcOffset);

  if(symb != nullptr && node->value()){
    node->value()->accept(this, lvl);

    if(node->is_typed(cdk::TYPE_DOUBLE) && node->value()->is_typed(cdk::TYPE_INT)){
      _pf.I2D();
    }

    if(node->is_typed(cdk::TYPE_DOUBLE)){
      _pf.DUP64();
      _pf.LOCAL(symb->offset());
      _pf.STDOUBLE();
    }
    else if(node->is_typed(cdk::TYPE_INT)){
      _pf.DUP32();
      _pf.LOCAL(symb->offset());
      _pf.STINT();
    }
    else if(node->is_typed(cdk::TYPE_POINTER)){
      if (std::dynamic_pointer_cast<cdk::reference_type>(node->type())->referenced()->name() == cdk::TYPE_DOUBLE){
        _pf.DUP64();
        _pf.LOCAL(symb->offset());
        _pf.STDOUBLE();
      }
      else if(std::dynamic_pointer_cast<cdk::reference_type>(node->type())->referenced()->name() == cdk::TYPE_INT){
        _pf.DUP32();
        _pf.LOCAL(symb->offset());
        _pf.STINT();
      }
    }
    else if(node->is_typed(cdk::TYPE_STRING)){
      _pf.LOCAL(symb->offset());
    }
  }
  }
}

void fir::postfix_writer::do_function_declaration_node(fir::function_declaration_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  if (!isGlobal()) {
      throw std::string(node->lineno() + "cannot declare function in body or in args");
      return;
    };

    if (!new_symbol()) return;

    auto function = new_symbol();
    _functions_to_declare.insert(function->id());
    reset_new_symbol();
    
    if(node->inUse()){
      _pf.EXTERN(node->id());
    }
}

void fir::postfix_writer::do_function_call_node(fir::function_call_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  auto symbol = _symtab.find(node->id());

  size_t args_size = 0;
  if(node->args()->size() > 0){
    for(int i = (node->args()->size())-1; i >= 0; i--){
      cdk::expression_node *arg = dynamic_cast<cdk::expression_node*>(node->args()->node(i));
      arg->accept(this, lvl+2);

      if(symbol->args().at(i)->name() == cdk::TYPE_DOUBLE && arg->is_typed(cdk::TYPE_INT)){
        _pf.I2D();
      }
      args_size+=symbol->args().at(i)->size();
    }
  }
  _pf.CALL(node->id());

  if(args_size != 0){
    _pf.TRASH(args_size);
  }

  if(symbol->is_typed(cdk::TYPE_INT) || symbol->is_typed(cdk::TYPE_POINTER) || symbol->is_typed(cdk::TYPE_STRING)){
    _pf.LDFVAL32();
  }
  else if(symbol->is_typed(cdk::TYPE_DOUBLE)){
    _pf.LDFVAL64();
  }
  if(symbol->is_typed(cdk::TYPE_INT))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  if(symbol->is_typed(cdk::TYPE_STRING))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
  if(symbol->is_typed(cdk::TYPE_VOID))
    node->type(cdk::primitive_type::create(0, cdk::TYPE_VOID));
  if(symbol->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  if(symbol->is_typed(cdk::TYPE_POINTER))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_POINTER));

}

void fir::postfix_writer::do_block_node(fir::block_node * const node, int lvl) {
  _symtab.push();

  if(node->decl()){
    node->decl()->accept(this, lvl+2);
  }

  if(node->inst()){
    node->inst()->accept(this, lvl+2);
  }

  _symtab.pop();
}

void fir::postfix_writer::do_address_node(fir::address_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->value()->accept(this, lvl);
}

void fir::postfix_writer::do_size_of_node(fir::size_of_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  _pf.INT(node->value()->type()->size());
}

void fir::postfix_writer::do_identity_node(fir::identity_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->argument()->accept(this, lvl);
}

void fir::postfix_writer::do_index_node(fir::index_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->pointer()->accept(this, lvl);
  node->index()->accept(this,lvl);

  _pf.INT(node->type()->size());
  _pf.MUL();
  _pf.ADD();
}

void fir::postfix_writer::do_mem_alloc_node(fir::mem_alloc_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);
  _pf.INT(3);
  _pf.SHTL();
  _pf.ALLOC();
  _pf.SP();
}

void fir::postfix_writer::do_prologue_node(fir::prologue_node * const node, int lvl) {
  _symtab.push();

  if(node->decl()){
    node->decl()->accept(this, lvl+2);
  }

  if(node->inst()){
    node->inst()->accept(this, lvl+2);
  }

  _symtab.pop();
}

void fir::postfix_writer::do_null_node(fir::null_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if(isGlobal()){
    _pf.SINT(0);
  }else{
    _pf.INT(0);
  }
}