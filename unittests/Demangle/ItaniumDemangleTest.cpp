//===------------------ ItaniumDemangleTest.cpp ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/Demangle/ItaniumDemangle.h"
#include "llvm/Support/Allocator.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <cstdlib>
#include <vector>

using namespace llvm;
using namespace llvm::itanium_demangle;

namespace {
class TestAllocator {
  BumpPtrAllocator Alloc;

public:
  void reset() { Alloc.Reset(); }

  template <typename T, typename... Args> T *makeNode(Args &&... args) {
    return new (Alloc.Allocate(sizeof(T), alignof(T)))
        T(std::forward<Args>(args)...);
  }

  void *allocateNodeArray(size_t sz) {
    return Alloc.Allocate(sizeof(Node *) * sz, alignof(Node *));
  }
};
} // namespace

TEST(ItaniumDemangle, MethodOverride) {
  struct TestParser : AbstractManglingParser<TestParser, TestAllocator> {
    std::vector<char> Types;

    TestParser(const char *Str)
        : AbstractManglingParser(Str, Str + strlen(Str)) {}

    Node *parseType() {
      Types.push_back(*First);
      return AbstractManglingParser<TestParser, TestAllocator>::parseType();
    }
  };

  TestParser Parser("_Z1fIiEjl");
  ASSERT_NE(nullptr, Parser.parse());
  EXPECT_THAT(Parser.Types, testing::ElementsAre('i', 'j', 'l'));
}
