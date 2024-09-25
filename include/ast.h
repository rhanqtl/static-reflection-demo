#ifndef AST__H
#define AST__H

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

//===---------------------------------------------------------------===//
// This file defines AST nodes of a minimalist language for constraint //
// programming.                                                        //
//===---------------------------------------------------------------===//

enum class NodeKind {
  // Decls

  // Exprs

  // Stmts

};

// Use 'struct' for simplicity.
struct Node {
  const NodeKind kind;
  Node *const upper;
  // SourceRange omitted for simplicity.

  Node(NodeKind kind, Node *upper = nullptr) : kind{kind}, upper{upper} {}
  virtual ~Node() noexcept = default;
};

#endif  // AST__H
