#include <string>
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated
#include <cdk/types/primitive_type.h>

#define ASSERT_UNSPEC { if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; }

//---------------------------------------------------------------------------

void fir::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl);
}

//---------------------------------------------------------------------------

void fir::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) {}

void fir::type_checker::do_data_node(cdk::data_node *const node, int lvl) {}

void fir::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if(!node->argument()->is_typed(cdk::TYPE_INT)){
    throw std::string("Wrong type: must be int!");
  }
}

void fir::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}

void fir::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void fir::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void fir::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
}

void fir::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
}

//---------------------------------------------------------------------------

void fir::type_checker::processUnaryExpression(cdk::unary_operation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
  else if(node->argument()->is_typed(cdk::TYPE_DOUBLE)){
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  else{
    throw std::string("wrong type in argument of unary expression");
  }
}

void fir::type_checker::do_neg_node(cdk::neg_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void fir::type_checker::processBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (!node->left()->is_typed(cdk::TYPE_INT) && !node->left()->is_typed(cdk::TYPE_DOUBLE) && !node->left()->is_typed(cdk::TYPE_POINTER)) {
    throw std::string("wrong type in left argument of binary expression");
  }
  if (!node->right()->is_typed(cdk::TYPE_INT) && !node->right()->is_typed(cdk::TYPE_DOUBLE) && !node->right()->is_typed(cdk::TYPE_POINTER)) {
    throw std::string("wrong type in right argument of binary expression");
  }
  
  if(node->left()->is_typed(cdk::TYPE_INT)){
    if(node->right()->is_typed(cdk::TYPE_INT))
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    if(node->right()->is_typed(cdk::TYPE_DOUBLE))
      node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    if(node->right()->is_typed(cdk::TYPE_POINTER))
      node->type(cdk::primitive_type::create(4, cdk::TYPE_POINTER));
  }
  if(node->left()->is_typed(cdk::TYPE_DOUBLE)){
    if(node->right()->is_typed(cdk::TYPE_INT))
      node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    if(node->right()->is_typed(cdk::TYPE_DOUBLE))
      node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  if(node->left()->is_typed(cdk::TYPE_POINTER)){
    if(node->right()->is_typed(cdk::TYPE_INT))
      node->type(cdk::primitive_type::create(4, cdk::TYPE_POINTER));
  }

}

void fir::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void fir::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void fir::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void fir::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void fir::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void fir::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void fir::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void fir::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void fir::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void fir::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void fir::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void fir::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  std::shared_ptr<fir::symbol> symbol = _symtab.find(id);

  if (symbol != nullptr) {
    node->type(symbol->type());
  } else {
    throw id + " undeclared.";
  }
}

void fir::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  ASSERT_UNSPEC;
  try {
    node->lvalue()->accept(this, lvl);
    node->type(node->lvalue()->type());
  } catch (const std::string &id) {
    throw "undeclared variable '" + id + "'";
  }
}

void fir::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl);
  node->rvalue()->accept(this, lvl);

  if(node->lvalue()->is_typed(cdk::TYPE_DOUBLE)){
    if(node->rvalue()->is_typed(cdk::TYPE_DOUBLE) || node->rvalue()->is_typed(cdk::TYPE_INT)){
      node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    }
    else if(node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {
      node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
      node->rvalue()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    }
    else{
      throw std::string("Wrong assignment to real value.");
    }
  }
  else if(node->lvalue()->is_typed(cdk::TYPE_INT)){
    if(node->rvalue()->is_typed(cdk::TYPE_INT)){
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    }
    else if(node->rvalue()->is_typed(cdk::TYPE_UNSPEC)){
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    }
    else {
      throw std::string("Wrong assignment to int value.");
    }
  }
  else if(node->lvalue()->is_typed(cdk::TYPE_STRING)){
    if(node->rvalue()->is_typed(cdk::TYPE_STRING)){
      node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
    }
    else if(node->rvalue()->is_typed(cdk::TYPE_UNSPEC)){
      node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
    }
    else{
      throw std::string("Wrong assignment to string value.");
    }
  }

  else if(node->lvalue()->is_typed(cdk::TYPE_POINTER)){
    if(node->rvalue()->is_typed(cdk::TYPE_POINTER)){
      node->type(cdk::primitive_type::create(4, cdk::TYPE_POINTER));
    }
    else if(node->rvalue()->is_typed(cdk::TYPE_UNSPEC)){
      node->type(cdk::primitive_type::create(4, cdk::TYPE_POINTER));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_POINTER));
    }
  }
}

//---------------------------------------------------------------------------

void fir::type_checker::do_function_definition_node(fir::function_definition_node *const node, int lvl) {
  if(node->default_value()){
    node->default_value()->accept(this,lvl);
  }
  const std::string &id = node->id();

  std::shared_ptr<fir::symbol> symbol = _symtab.find(id);

  if(!symbol){
    if(node->id() == "fir"){
      node->setId("_main");
    }
    else if(node->id() == "_main"){
      node->setId("._main");
    }
  
    std::vector<std::shared_ptr<cdk::basic_type>> type_args;
      
    if(node->args() != NULL){
      for(size_t i = 0; i < node->args()->size(); i++){
        type_args.push_back(node->arg(i)->type());
      }
    }

    symbol = std::make_shared<fir::symbol>(node->type(), node->id(), -(node->type()->size()), node->isPublic(), node->inUse(), true, type_args);
    _symtab.insert(node->id(), symbol);
  }
  else if(node->args() && (symbol->args().size() == node->args()->size())){
    cdk::sequence_node *args = node->args();
    std::vector<std::shared_ptr<cdk::basic_type>> type_args = symbol->args();

    for(size_t i = 0; i < args->size(); i++){
      if(symbol->args().at(i) != type_args.at(i)){
        throw "wrong type of arguments";
      }
    }
  }
}

void fir::type_checker::do_evaluation_node(fir::evaluation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void fir::type_checker::do_read_node(fir::read_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(0, cdk::TYPE_UNSPEC));
}

//---------------------------------------------------------------------------

void fir::type_checker::do_while_do_node(fir::while_do_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

void fir::type_checker::do_if_node(fir::if_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

void fir::type_checker::do_if_else_node(fir::if_else_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

//NEW----------------------------------------------------------------------

void fir::type_checker::do_leave_node(fir::leave_node *const node, int lvl) {
}

void fir::type_checker::do_restart_node(fir::restart_node *const node, int lvl) {
}

void fir::type_checker::do_return_node(fir::return_node *const node, int lvl) {}

void fir::type_checker::do_while_do_finally_node(fir::while_do_finally_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

void fir::type_checker::do_write_node(fir::write_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void fir::type_checker::do_variable_decl_node(fir::variable_decl_node *const node, int lvl) {
  if(node->value() != nullptr){
    node->value()->accept(this, lvl + 2);
    if(node->is_typed(cdk::TYPE_INT)){
      if(!node->value()->is_typed(cdk::TYPE_INT))
        throw std::string("Wrong type for value. Must be int!");
    }

    else if(node->is_typed(cdk::TYPE_DOUBLE)){
      if(!node->value()->is_typed(cdk::TYPE_DOUBLE) && !node->value()->is_typed(cdk::TYPE_INT))
        throw std::string("Wrong type for value. Must be double or int!");
    }

    else if(node->is_typed(cdk::TYPE_STRING)){
      if(!node->value()->is_typed(cdk::TYPE_STRING))
        throw std::string("Wrong type for value. Must be string!");
    }

    else if(node->is_typed(cdk::TYPE_POINTER)){
      if(!node->value()->is_typed(cdk::TYPE_POINTER)){
        throw std::string("Wrong type for value. Must be pointer!");
      }
    }

    else{
      throw std::string("Unknown type.");
    }
  }

  if(!_symtab.find(node->id())){
    _symtab.insert(node->id(), std::make_shared<fir::symbol>(node->type(), node->id(), -(node->type()->size()), node->ispublic(), node->isactive(), false));
    _parent->set_new_symbol(std::make_shared<fir::symbol>(node->type(), node->id(), -(node->type()->size()), node->ispublic(), node->isactive(), false));
  }
}

void fir::type_checker::do_function_declaration_node(fir::function_declaration_node *const node, int lvl){
  if(node->id() == "fir"){
    node->setId("_main");
  }
  else if(node->id() == "_main"){
    node->setId("._main");
  }
  
  std::shared_ptr<fir::symbol> symbol = _symtab.find(node->id());
  if(symbol){
    throw std::string(node->id() + " already declared.");
  }

  else{
    std::vector<std::shared_ptr<cdk::basic_type>> type_args;
    
    if(node->args() != NULL){
      for(size_t i = 0; i < node->args()->size(); i++){
        type_args.push_back(node->arg(i)->type());
      }
    }
    symbol = std::make_shared<fir::symbol>(node->type(), node->id(), -(node->type()->size()), node->isPublic(), node->inUse(), true, type_args);
    _symtab.insert(node->id(), symbol);
  }

}

void fir::type_checker::do_function_call_node(fir::function_call_node *const node, int lvl){
  ASSERT_UNSPEC;
  
  const std::string &id = node->id();

  std::shared_ptr<fir::symbol> symbol = _symtab.find(id);

  if(symbol == nullptr)
    throw std::string("symbol " + id + " undeclared.");

  if(symbol->is_function()){
    std::vector<std::shared_ptr<cdk::basic_type>> type_args = symbol->args();
    cdk::expression_node *arg;

    if(node->args() != NULL){
      if(type_args.size() != node->args()->size()){
        throw "wrong number of arguments in function " + id;
      }

      for(size_t i = 0; i < type_args.size(); i++){
        
        arg = (cdk::expression_node*) node->args()->node(i);
        arg->accept(this, lvl);

        if((arg->type()->name() & type_args.at(i)->name()) == 0){
          if(arg->type()->name() == cdk::TYPE_INT && type_args.at(i)->name() == cdk::TYPE_DOUBLE)
            continue;
          else
            throw "different types of arguments in function " + id;
        }
      }      
    }
  }
  else if(!(symbol->is_function())){
    throw id + " is a variable";
  }
}

void fir::type_checker::do_block_node(fir::block_node *const node, int lvl){}

void fir::type_checker::do_address_node(fir::address_node *const node, int lvl){
  ASSERT_UNSPEC;
  node->value()->accept(this, lvl + 2);
  node->type(cdk::reference_type::create(4, node->value()->type()));
}

void fir::type_checker::do_size_of_node(fir::size_of_node *const node, int lvl){
  ASSERT_UNSPEC;
  node->value()->accept(this, lvl + 2);
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void fir::type_checker::do_identity_node(fir::identity_node *const node, int lvl){
  processUnaryExpression(node, lvl);
}

void fir::type_checker::do_index_node(fir::index_node *const node, int lvl){
  ASSERT_UNSPEC;

  node->pointer()->accept(this, lvl);

  if(!node->pointer()->is_typed(cdk::TYPE_POINTER)){
    throw std::string("Wrong type for base in pointer.");
  }

  node->index()->accept(this, lvl);
  if(!node->index()->is_typed(cdk::TYPE_INT)){
    throw std::string("Wrong type for index in pointer.");
  }

  node->type(std::dynamic_pointer_cast<cdk::reference_type>(node->pointer()->type())->referenced());
}

void fir::type_checker::do_mem_alloc_node(fir::mem_alloc_node *const node, int lvl){
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  
  if(!node->argument()->is_typed(cdk::TYPE_INT)){
    throw std::string("wrong type! Must be int!");
  }
  node->type(cdk::primitive_type::create(4, cdk::TYPE_POINTER));
}

void fir::type_checker::do_prologue_node(fir::prologue_node *const node, int lvl){}

void fir::type_checker::do_null_node(fir::null_node *const node, int lvl){
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_POINTER));
}
