#include <json_generator.h>

#include <string>

#include <cassert>

namespace alien {

#define visit_one(node)                     \
do {                                        \
    auto object = nlohmann::json::object(); \
    push();                                 \
    jCurrent_ = &object;                    \
    node->accept(*this);                    \
    pop();                                  \
    jCurrent_->push_back(object);           \
} while (0)

#define visit_begin(name, func)                         \
do {                                                    \
    (*jCurrent_)[#name] = nlohmann::json::func();       \
    push();                                             \
    jCurrent_ = &(*jCurrent_)[#name];                   \
} while (0)

#define visit_end() pop()


void JsonGenerator::generate_json(std::ostream& os, const std::vector<StmtPtr>& program) {
    json_.clear();
    json_["Program"] = nlohmann::json::array();
    push();
    jCurrent_ = &json_["Program"];
    for (const auto& stmt : program)
    {
        visit_one(stmt);
    }
    pop();
    os << json_.dump(4);
}

void JsonGenerator::visit(ClassDecl &decl) {

}

void JsonGenerator::visit(FuncDecl &decl) {
    (*jCurrent_)["Func"] = nlohmann::json::object();
    (*jCurrent_)["Func"]["name"] = decl.name.lexeme_;
    auto jarray = nlohmann::json::array();
    for (const auto p : decl.parameters) {
        jarray.push_back(p.lexeme_);
    }
    (*jCurrent_)["Func"]["parameters"] = jarray;
    (*jCurrent_)["Func"]["body"] = nlohmann::json::object();
    push();
    jCurrent_ = &(*jCurrent_)["Func"]["body"];
    decl.body->accept(*this);
    pop();
}

void JsonGenerator::visit(VarDecl &decl) {
    (*jCurrent_)["Var"] = nlohmann::json::object();
    (*jCurrent_)["Var"]["name"] = decl.name.lexeme_;
    if (decl.initializer) {
        (*jCurrent_)["Var"]["initializer"] = nlohmann::json::object();
        push();
        jCurrent_ = &(*jCurrent_)["Var"]["initializer"];
        decl.initializer->accept(*this);
        pop();
    }
}

void JsonGenerator::visit(BlockStmt &stmts) {
    visit_begin(Block, array);
    for (const auto& stmt : stmts.stmts) {
        visit_one(stmt);
    }
    visit_end();
}

void JsonGenerator::visit(IfStmt &stmt) {
    visit_begin(If, object);
    stmt.condition->accept(*this);
    stmt.thenBranch->accept(*this);
    if (stmt.elseBranch) {
        stmt.elseBranch->accept(*this);
    }
    visit_end();
}

void JsonGenerator::visit(WhileStmt &stmt) {
    visit_begin(While, object);
    stmt.condition->accept(*this);
    stmt.body->accept(*this);
    visit_end();
}

void JsonGenerator::visit(ForStmt &stmt) {

}

void JsonGenerator::visit(PrintStmt &stmt) {

}

void JsonGenerator::visit(ReturnStmt &stmt) {
    visit_begin(Return, object);
    if (stmt.expr) {
        stmt.expr->accept(*this);
    }
    visit_end();
}

void JsonGenerator::visit(ExprStmt& stmt) {
    visit_begin(Exprstmt, object);
    stmt.expr->accept(*this);
    visit_end();
}

void JsonGenerator::visit(Assign& expr) {
    visit_begin(Assign, object);
    (*jCurrent_)["name"] = expr.name.lexeme_;
    visit_begin(value, object);
    expr.value->accept(*this);
    visit_end();
    visit_end();
}

void JsonGenerator::visit(Binary& expr) {
    visit_begin(Binary, object);
    visit_begin(left, object);
    expr.left->accept(*this);
    visit_end();
    (*jCurrent_)["operator"] = expr.op.lexeme_;
    visit_begin(right, object);
    expr.right->accept(*this);
    visit_end();
    visit_end();
}

void JsonGenerator::visit(Call& expr) {
    visit_begin(Call, object);
    visit_begin(callee, object);
    expr.callee->accept(*this);
    visit_end();
    visit_begin(arguments, array);
    for (const auto& exp : expr.arguments) {
        visit_one(exp);
    }
    visit_end();
    visit_end();
}

void JsonGenerator::visit(Get& expr) {

}

void JsonGenerator::visit(Grouping& expr) {
    visit_begin(Grouping, object);
    expr.expr->accept(*this);
    visit_end();
}

void JsonGenerator::visit(Set& expr) {

}

void JsonGenerator::visit(Unary& expr) {
    visit_begin(Unary, object);
    (*jCurrent_)["operator"] = expr.op.lexeme_;
    visit_begin(right, object);
    expr.right->accept(*this);
    visit_end();
    visit_end();
}

void JsonGenerator::visit(Variable& expr) {
    visit_begin(Variable, object);
    (*jCurrent_)["name"] = expr.name.lexeme_;
    visit_end();
}

void JsonGenerator::visit(Logical& expr) {
    visit_begin(Logical, object);
    visit_begin(left, object);
    expr.left->accept(*this);
    visit_end();
    (*jCurrent_)["operator"] = expr.op.lexeme_;
    visit_begin(right, object);
    expr.right->accept(*this);
    visit_end();
    visit_end();
}

void JsonGenerator::visit(Number& expr) {
    (*jCurrent_)["Number"] = expr.value;
}

void JsonGenerator::visit(String& expr) {
    (*jCurrent_)["String"] = expr.str;
}

void JsonGenerator::visit(Literal& expr) {
    std::string value;
    switch (expr.literal) {
    case TOKEN_NIL:
        value = "nil";
        break;
    case TOKEN_FALSE:
        value = "false";
        break;
    case TOKEN_TRUE:
        value = "true";
        break;
    default:
        assert(false);
    }
    (*jCurrent_)["Literal"] = value;
}

void JsonGenerator::visit(This& expr) {

}

} // namespace alien