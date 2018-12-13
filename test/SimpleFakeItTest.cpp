//
// Created by Nguyen Tran on 6/25/18.
//
#include <string>
#include "fakeit.hpp"
#include <iostream>

using namespace fakeit;

struct SomeInterface {
  virtual ~SomeInterface() = default;

  virtual int foo(int) = 0;

  virtual int bar(std::string) = 0;
};

TEST_CASE("FAKEIT test", "[fakeit]") {
// Instantiate a mock object.
Mock<SomeInterface> mock;

// Setup mock behavior.
When(Method(mock, foo)).Return(1); // Method mock.foo will return 1 once.

// Fetch the mock instance.
auto &i = mock.get();

// Will print "1".
std::cout << i.foo(0);

// Verify method mock.foo was invoked.
Verify(Method(mock, foo));

// Verify method mock.foo was invoked with specific arguments.
Verify(Method(mock, foo).Using(0));

}