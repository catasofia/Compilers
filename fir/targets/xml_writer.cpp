#include <string>
#include "targets/xml_writer.h"
#include "targets/type_checker.h"
#include "ast/all.h"  // automatically generated

std::string bool_to_string(bool value) {
  if (value) return "true";
  else return "false";
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}
void fir::xml_writer::do_data_node(cdk::data_node * const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}
void fir::xml_writer::do_double_node(cdk::double_node * const node, int lvl) {
  process_literal(node, lvl);
}
void fir::xml_writer::do_not_node(cdk::not_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}
void fir::xml_writer::do_and_node(cdk::and_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_or_node(cdk::or_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  os() << std::string(lvl, ' ') << "<sequence_node size='" << node->size() << "'>" << std::endl;
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  process_literal(node, lvl);
}

void fir::xml_writer::do_string_node(cdk::string_node * const node, int lvl) {
  process_literal(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_unary_operation(cdk::unary_operation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void fir::xml_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_binary_operation(cdk::binary_operation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void fir::xml_writer::do_add_node(cdk::add_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_div_node(cdk::div_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_le_node(cdk::le_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void fir::xml_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  os() << std::string(lvl, ' ') << "<" << node->label() << ">" << node->name() << "</" << node->label() << ">" << std::endl;
}

void fir::xml_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

void fir::xml_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  node->lvalue()->accept(this, lvl);
  reset_new_symbol();

  node->rvalue()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_function_definition_node(fir::function_definition_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  
  os() << std::string(lvl, ' ') << "<" << node->label() << " public='" << bool_to_string(node->isPublic())
  << "' use='" << bool_to_string(node->inUse()) << "' id ='" << node->id() << "'>" << std::endl;

  openTag("args", lvl + 2);
  if(node->args() != nullptr) { 
    node->args()->accept(this, lvl + 4); 
  }
  closeTag("args", lvl + 2);

  openTag("block", lvl + 2);
  if(node->block() != nullptr) { 
    node->block()->accept(this, lvl + 4); 
  }
  closeTag("block", lvl + 2);

  openTag("epilogue", lvl + 2);
  if(node->epilogue() != nullptr) { 
    node->epilogue()->accept(this, lvl + 4); 
  }
  closeTag("epilogue", lvl + 2);

  openTag("prologue", lvl + 2);
  if(node->prologue() != nullptr) { 
    node->prologue()->accept(this, lvl + 4); 
  }
  closeTag("prologue", lvl + 2);

  openTag("default_value", lvl + 2);
  if(node->default_value() != nullptr) { 
    node->default_value()->accept(this, lvl + 4); 
  }
  closeTag("default_value", lvl + 2);

  closeTag(node, lvl); 
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_evaluation_node(fir::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}


//---------------------------------------------------------------------------

void fir::xml_writer::do_read_node(fir::read_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_while_do_node(fir::while_do_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("block", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("block", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void fir::xml_writer::do_if_node(fir::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  closeTag(node, lvl);
}

void fir::xml_writer::do_if_else_node(fir::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->thenblock()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  openTag("else", lvl + 2);
  node->elseblock()->accept(this, lvl + 4);
  closeTag("else", lvl + 2);
  closeTag(node, lvl);
}

//NEW-------------------------------------------------------------------------

void fir::xml_writer::do_leave_node(fir::leave_node *const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  closeTag(node, lvl);
}

void fir::xml_writer::do_restart_node(fir::restart_node *const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  closeTag(node, lvl);
}

void fir::xml_writer::do_return_node(fir::return_node *const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  closeTag(node, lvl);
}

void fir::xml_writer::do_while_do_finally_node(fir::while_do_finally_node *const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);

  openTag("block", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("block", lvl + 2);

  openTag("finally", lvl + 2);
  node->finally()->accept(this, lvl + 4);
  closeTag("finally", lvl + 2);

  closeTag(node, lvl);
}

void fir::xml_writer::do_variable_decl_node(fir::variable_decl_node *const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;

  os() << std::string(lvl, ' ') << "<" << node->label() << " public='" << node->ispublic() 
  << "' use='" << node->isactive() << "' id= " << node->id() << "'>" << std::endl;

  openTag("value", lvl + 2);
  if(node->value() != nullptr){
    node->value()->accept(this, lvl + 4);
  }
  closeTag("value", lvl + 2);
  
  closeTag(node, lvl);
}

void fir::xml_writer::do_function_declaration_node(fir::function_declaration_node *const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;

  os() << std::string(lvl, ' ') << "<" << node->label() << " public='" << node->isPublic() 
  << "' use='" << node->inUse() << "' id='" << node->id() << "'>" << std::endl;

  openTag("args", lvl + 2);
  if(node->args() != nullptr){
    node->args()->accept(this, lvl + 4);
  }
  closeTag("args", lvl + 2);

  closeTag(node, lvl);
}

void fir::xml_writer::do_function_call_node(fir::function_call_node *const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;
  os() << std::string(lvl, ' ') << "<" << node->label() 
  << " id='" << node->id() << "'>" << std::endl;

  openTag("args", lvl + 2);
  if(node->args() != nullptr) {
    node->args()->accept(this, lvl + 4);
  }
  closeTag("args", lvl + 2);

  closeTag(node, lvl);
}

void fir::xml_writer::do_block_node(fir::block_node *const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;
  os() << std::string(lvl, ' ') << "<" << node->label()
  << " isEpilogue='" << bool_to_string(node->isEpilogue()) << "'>" << std::endl;

  openTag("decl", lvl + 2);
  if(node->decl() != nullptr) {
    node->decl()->accept(this, lvl + 4);
  }
  closeTag("decl", lvl + 2);

  openTag("instr", lvl + 2);
  if(node->inst() != nullptr) {
    node->inst()->accept(this, lvl + 4);
  }
  closeTag("instr", lvl + 2);

  closeTag(node, lvl);
}

void fir::xml_writer::do_address_node(fir::address_node *const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->value()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void fir::xml_writer::do_size_of_node(fir::size_of_node *const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->value()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void fir::xml_writer::do_identity_node(fir::identity_node *const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;
  do_unary_operation(node, lvl);
}

void fir::xml_writer::do_index_node(fir::index_node *const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  
  openTag("pointer", lvl + 2);
  node->pointer()->accept(this, lvl + 4);
  closeTag("pointer", lvl + 2);

  openTag("index", lvl + 2);
  node->index()->accept(this, lvl + 4);
  closeTag("index", lvl + 2);

  closeTag(node, lvl);
}

void fir::xml_writer::do_mem_alloc_node(fir::mem_alloc_node *const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;
  do_unary_operation(node, lvl);
}

void fir::xml_writer::do_prologue_node(fir::prologue_node *const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  openTag("decl", lvl + 2);
  if(node->decl() != nullptr) {
    node->decl()->accept(this, lvl + 4);
  }
  closeTag("decl", lvl + 2);

  openTag("instr", lvl + 2);
  if(node->inst() != nullptr) {
    node->inst()->accept(this, lvl + 4);
  }
  closeTag("instr", lvl + 2);

  closeTag(node, lvl);
}

void fir::xml_writer::do_null_node(fir::null_node *const node, int lvl){
  ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  closeTag(node, lvl);
}


void fir::xml_writer::do_write_node(fir::write_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  os() << std::string(lvl, ' ') << "<" << node->label()
  << " new_line='" << bool_to_string(node->newline()) << "'>" << std::endl;

  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}
