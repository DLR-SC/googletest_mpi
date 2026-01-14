// Copyright 2005, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// The Google C++ Testing and Mocking Framework (Google Test)
//
// This file defines the AssertionResult type.

#include "gtest/gtest-assertion-result.h"

#include <string>
#include <utility>

#include "gtest/gtest-message.h"

namespace testing {

// AssertionResult constructors.
// Used in EXPECT_TRUE/FALSE(assertion_result).
AssertionResult::AssertionResult(const AssertionResult& other)
    : success_(other.success_), globalResultsDiffer_(other.globalResultsDiffer_),
      message_(other.message_ != nullptr
                   ? new ::std::string(*other.message_)
                   : static_cast< ::std::string*>(nullptr)) {}

// Swaps two AssertionResults.
void AssertionResult::swap(AssertionResult& other) {
  using std::swap;
  swap(success_, other.success_);
  swap(globalResultsDiffer_, other.globalResultsDiffer_);
  swap(message_, other.message_);
}

// checks that all MPI processes have the same v
bool AssertionResult::boolIdenticalOnMPIprocs(bool v)
{
#ifdef GTEST_HAS_MPI
  int localSuccess = v;
  int globalAndV, globalOrV;
  bool mpiErr = false;
  mpiErr = mpiErr || (MPI_Allreduce(&localSuccess, &globalAndV, 1, MPI_INT, MPI_LAND, internal::GTEST_MPI_COMM_WORLD) != MPI_SUCCESS);
  mpiErr = mpiErr || (MPI_Allreduce(&localSuccess, &globalOrV, 1, MPI_INT, MPI_LOR, internal::GTEST_MPI_COMM_WORLD) != MPI_SUCCESS);
  if( mpiErr )
  {
      GTEST_LOG_(ERROR)
        << "Error in MPI_Allreduce (should return MPI_SUCCESS)!";
      return false;
  }
  else
  {
    return globalAndV == globalOrV;
  }
#endif
}

// Returns the assertion's negation. Used with EXPECT/ASSERT_FALSE.
AssertionResult AssertionResult::operator!() const {
  AssertionResult negation(!success_, false);
  negation.globalResultsDiffer_ = globalResultsDiffer_;
  if (message_ != nullptr) negation << *message_;
  return negation;
}

// Makes a successful assertion result.
AssertionResult AssertionSuccess(bool global) {
  return AssertionResult(true, global);
}

// Makes a failed assertion result.
AssertionResult AssertionFailure(bool global) {
  return AssertionResult(false, global);
}

// Makes a failed assertion result with the given failure message.
// Deprecated; use AssertionFailure() << message.
AssertionResult AssertionFailure(const Message& message) {
  return AssertionFailure() << message;
}

}  // namespace testing
