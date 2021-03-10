//
// Created by Alan Huang on 2/24/21.
//

#ifndef ALIEN_TYPEDEF_H
#define ALIEN_TYPEDEF_H

#include <memory>

namespace alien {

class Expr;
class Stmt;
using StmtPtr = std::unique_ptr<Stmt>;
using ExprPtr = std::unique_ptr<Expr>;

}
#endif //ALIEN_TYPEDEF_H
