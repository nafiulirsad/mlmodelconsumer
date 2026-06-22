#pragma once

// Lightweight result type used across the consumer's internal boundaries.
// (The bridge fails *closed*: callers must inspect ok() before using value().)

#include <string>
#include <utility>

namespace mlmodelconsumer {

enum class Status {
  Ok = 0,
  ReadError,        // Requirement 1: ingestion failure
  ParseError,       // Requirement 1: malformed payload
  IncompatibleVersion,  // Requirement 2
  ValidationFailed,     // Requirement 3
  InferenceFailed,      // Requirement 1 (local inference)
};

template <typename T>
class Result {
 public:
  static Result success(T value) {
    return Result(Status::Ok, std::move(value), std::string());
  }
  static Result failure(Status s, std::string message) {
    return Result(s, T{}, std::move(message));
  }

  bool ok() const { return status_ == Status::Ok; }
  Status status() const { return status_; }
  const std::string& message() const { return message_; }
  const T& value() const { return value_; }
  T& value() { return value_; }

 private:
  Result(Status s, T value, std::string msg)
      : status_(s), value_(std::move(value)), message_(std::move(msg)) {}

  Status status_;
  T value_;
  std::string message_;
};

}  // namespace mlmodelconsumer
