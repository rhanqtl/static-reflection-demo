#include "serde/serialize.h"

#include <gtest/gtest.h>

TEST(Serialization, _) {
  serde::ASTSaver saver{"."};
  saver.save();
}
