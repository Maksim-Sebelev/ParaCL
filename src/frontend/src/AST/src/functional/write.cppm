module;

#include <boost/json.hpp>
#include <boost/json/object.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>

export module ast_write;

export import ast;

import node_type_erasure;
import ast_nodes;
import node_traits;

namespace ParaCL::frontend::ast
{
namespace node
{

export using writable = boost::json::value();

auto
write(
    BasicNode const& node
) -> decltype(visit<boost::json::value>(node))
{ return visit<boost::json::value>(node); }

boost::json::value
write(
    CodeLocation const& location
)
{
  auto&& obj          = boost::json::object{};
  obj["file"]         = location.file();
  obj["line_begin"]   = location.line_begin();
  obj["line_end"]     = location.line_end();
  obj["column_begin"] = location.column_begin();
  obj["column_end"]   = location.column_end();
  obj["code_excerpt"] = location.code_excerpt();

  return obj;
}

void
set_location(
    boost::json::object& obj, CodeLocation const& location
)
{ obj["location"] = write(location); }

namespace visit_specializations
{

template <>
boost::json::value
visit(
    NumberLiteral const& node
)
{
  auto&& obj  = boost::json::object{};
  obj["kind"] = traits::get_node_info<NumberLiteral, traits::NAME>();
  obj[traits::get_node_info<NumberLiteral, traits::FIELD, 0>()] = node.value();
  set_location(obj, node.location());
  return obj;
}

template <>
boost::json::value
visit(
    StringLiteral const& node
)
{
  auto&& obj  = boost::json::object{};
  obj["kind"] = traits::get_node_info<StringLiteral, traits::NAME>();
  obj[traits::get_node_info<StringLiteral, traits::FIELD, 0>()] =
      std::string{node.value()};
  set_location(obj, node.location());

  return obj;
}

template <>
boost::json::value
visit(
    Variable const& node
)
{
  auto&& obj  = boost::json::object{};
  obj["kind"] = traits::get_node_info<Variable, traits::NAME>();
  obj[traits::get_node_info<Variable, traits::FIELD, 0>()] =
      std::string{node.name()};
  set_location(obj, node.location());

  return obj;
}

template <>
boost::json::value
visit(
    Scan const& node
)
{
  auto&& obj  = boost::json::object{};
  obj["kind"] = traits::get_node_info<Scan, traits::NAME>();
  set_location(obj, node.location());

  return obj;
}

template <>
boost::json::value
visit(
    Print const& node
)
{
  auto&& obj  = boost::json::object{};
  obj["kind"] = traits::get_node_info<Print, traits::NAME>();

  auto&& args_arr = boost::json::array{};
  for (auto&& arg : node) args_arr.emplace_back(write(arg));

  obj[traits::get_node_info<Print, traits::FIELD, 0>()] = std::move(args_arr);
  set_location(obj, node.location());

  return obj;
}

template <>
boost::json::value
visit(
    UnaryOperator const& node
)
{
  auto&& obj  = boost::json::object{};
  obj["kind"] = traits::get_node_info<UnaryOperator, traits::NAME>();

  auto&& op = std::string_view{};
  switch (node.type())
  {
    case UnaryOperator::UnaryOperatorT::MINUS:
      op = traits::get_node_info<
          UnaryOperator,
          traits::OPERATOR_NAME,
          UnaryOperator::MINUS
      >();
      break;
    case UnaryOperator::UnaryOperatorT::PLUS:
      op = traits::get_node_info<
          UnaryOperator,
          traits::OPERATOR_NAME,
          UnaryOperator::PLUS
      >();
      break;
    case UnaryOperator::UnaryOperatorT::NOT:
      op = traits::get_node_info<
          UnaryOperator,
          traits::OPERATOR_NAME,
          UnaryOperator::NOT
      >();
      break;
  }
  obj[traits::get_node_info<UnaryOperator, traits::FIELD, 0>()] = op;
  obj[traits::get_node_info<UnaryOperator, traits::FIELD, 1>()] =
      write(node.arg());
  set_location(obj, node.location());

  return obj;
}

template <>
boost::json::value
visit(
    BinaryOperator const& node
)
{
  auto&& obj  = boost::json::object{};
  obj["kind"] = traits::get_node_info<BinaryOperator, traits::NAME>();

  auto&& op = std::string_view{};
  using OpT = BinaryOperator::BinaryOperatorT;
  switch (node.type())
  {
    case OpT::ADD:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::ADD
      >();
      break;
    case OpT::SUB:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::SUB
      >();
      break;
    case OpT::MUL:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::MUL
      >();
      break;
    case OpT::DIV:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::DIV
      >();
      break;
    case OpT::REM:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::REM
      >();
      break;
    case OpT::AND:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::AND
      >();
      break;
    case OpT::OR:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::OR
      >();
      break;
    case OpT::ISAB:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::ISAB
      >();
      break;
    case OpT::ISABE:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::ISABE
      >();
      break;
    case OpT::ISLS:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::ISLS
      >();
      break;
    case OpT::ISLSE:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::ISLSE
      >();
      break;
    case OpT::ISEQ:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::ISEQ
      >();
      break;
    case OpT::ISNE:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::ISNE
      >();
      break;
    case OpT::ASGN:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::ASGN
      >();
      break;
    case OpT::ADDASGN:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::ADDASGN
      >();
      break;
    case OpT::SUBASGN:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::SUBASGN
      >();
      break;
    case OpT::MULASGN:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::MULASGN
      >();
      break;
    case OpT::DIVASGN:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::DIVASGN
      >();
      break;
    case OpT::REMASGN:
      op = traits::get_node_info<
          BinaryOperator,
          traits::OPERATOR_NAME,
          BinaryOperator::REMASGN
      >();
      break;
  }
  obj[traits::get_node_info<BinaryOperator, traits::FIELD, 0>()] = op;
  obj[traits::get_node_info<BinaryOperator, traits::FIELD, 1>()] =
      write(node.larg());
  obj[traits::get_node_info<BinaryOperator, traits::FIELD, 2>()] =
      write(node.rarg());
  set_location(obj, node.location());

  return obj;
}

template <>
boost::json::value
visit(
    While const& node
)
{
  auto&& obj  = boost::json::object{};
  obj["kind"] = traits::get_node_info<While, traits::NAME>();
  obj[traits::get_node_info<While, traits::FIELD, 0>()] =
      write(node.condition());
  obj[traits::get_node_info<While, traits::FIELD, 1>()] = write(node.body());
  set_location(obj, node.location());

  return obj;
}

template <>
boost::json::value
visit(
    If const& node
)
{
  auto&& obj  = boost::json::object{};
  obj["kind"] = traits::get_node_info<If, traits::NAME>();
  obj[traits::get_node_info<If, traits::FIELD, 0>()] = write(node.condition());
  obj[traits::get_node_info<If, traits::FIELD, 1>()] = write(node.body());
  set_location(obj, node.location());

  return obj;
}

template <>
boost::json::value
visit(
    Else const& node
)
{
  auto&& obj  = boost::json::object{};
  obj["kind"] = traits::get_node_info<Else, traits::NAME>();
  obj[traits::get_node_info<Else, traits::FIELD, 0>()] = write(node.body());
  set_location(obj, node.location());

  return obj;
}

template <>
boost::json::value
visit(
    Condition const& node
)
{
  auto&& obj  = boost::json::object{};
  obj["kind"] = traits::get_node_info<Condition, traits::NAME>();

  auto&& ifs_arr = boost::json::array{};
  for (auto&& if_node : node.get_ifs()) ifs_arr.push_back(write(if_node));

  obj[traits::get_node_info<Condition, traits::FIELD, 0>()] =
      std::move(ifs_arr);

  if (node.has_else())
    obj[traits::get_node_info<Condition, traits::FIELD, 1>()] =
        write(node.get_else());

  set_location(obj, node.location());

  return obj;
}

template <>
boost::json::value
visit(
    Scope const& node
)
{
  auto&& obj  = boost::json::object{};
  obj["kind"] = traits::get_node_info<Scope, traits::NAME>();

  auto&& stmts = boost::json::array{};

  for (auto&& stmt : node) stmts.emplace_back(write(stmt));

  obj[traits::get_node_info<Scope, traits::FIELD, 0>()] = std::move(stmts);
  set_location(obj, node.location());

  return obj;
}

template <>
boost::json::value
visit(
    Return const& node
)
{
  auto&& obj  = boost::json::object{};
  obj["kind"] = traits::get_node_info<Return, traits::NAME>();

  obj[traits::get_node_info<Return, traits::FIELD, 0>()] =
      write(node.expression());
  set_location(obj, node.location());

  return obj;
}

template <>
boost::json::value
visit(
    FunctionDeclaration const& node
)
{
  auto&& obj  = boost::json::object{};
  obj["kind"] = traits::get_node_info<FunctionDeclaration, traits::NAME>();

  obj[traits::get_node_info<FunctionDeclaration, traits::FIELD, 0>()] =
      node.name();

  auto&& args = boost::json::array{};
  args.reserve(node.args().size());

  for (auto&& arg : node.args())
    args.emplace_back(static_cast<Variable>(arg).name());

  obj[traits::get_node_info<FunctionDeclaration, traits::FIELD, 1>()] =
      std::move(args);

  obj[traits::get_node_info<FunctionDeclaration, traits::FIELD, 2>()] =
      write(node.body());
  set_location(obj, node.location());

  return obj;
}

template <>
boost::json::value
visit(
    FunctionCall const& node
)
{
  auto&& obj  = boost::json::object{};
  obj["kind"] = traits::get_node_info<FunctionCall, traits::NAME>();

  obj[traits::get_node_info<FunctionCall, traits::FIELD, 0>()] = node.name();

  auto&& args = boost::json::array{};
  for (auto&& arg : node.args()) args.push_back(write(arg));

  obj[traits::get_node_info<FunctionCall, traits::FIELD, 1>()] =
      std::move(args);

  set_location(obj, node.location());

  return obj;
}

} /* namespace visit_specializations */
} /* namespace node */

boost::json::value
write(
    AST const& ast
)
{
  auto&& obj  = boost::json::object{};
  obj["kind"] = "AST";

  obj["root"] = write(ast.root());

  return obj;
}

export
void
write(
    AST const& ast, std::filesystem::path const& file
)
{
  auto&& json_ast = write(ast);

  auto&& out = std::ofstream{file};

  if (out.fail())
    throw std::runtime_error(
        "No such file: " + file.string() + ".\nFailed write ast in json format."
    );

  out << boost::json::serialize(json_ast);

  out.close();
}

} /* namespace ParaCL::frontend::ast */
