#pragma once

// Requirement 3: Output validation -> shape / range / sanity before use.

#include "mlmodelconsumer/model_output.hpp"
#include "mlmodelconsumer/result.hpp"

namespace mlmodelconsumer {

// Bounds applied during validation. Closed interval [min_value, max_value].
struct ValidationPolicy {
  std::int64_t min_dim{1};
  std::int64_t max_dim{1024};
  double min_value{-1.0e9};
  double max_value{1.0e9};
};

// Pure check. Returns Ok, or a ValidationFailed result describing the first
// violated rule (shape mismatch, out-of-range value, or NaN/Inf insanity).
Result<ModelOutput> validate(const ModelOutput& out,
                             const ValidationPolicy& policy);

}  // namespace mlmodelconsumer
