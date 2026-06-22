#include "mlmodelconsumer/validation.hpp"

#include <cmath>

namespace mlmodelconsumer {

Result<ModelOutput> validate(const ModelOutput& out,
                             const ValidationPolicy& policy) {
  const std::int64_t dim = static_cast<std::int64_t>(out.values.size());

  // Shape: declared dim must match actual, and lie within policy bounds.
  if (out.expected_dim != dim) {
    return Result<ModelOutput>::failure(
        Status::ValidationFailed, "shape mismatch: declared dim != value count");
  }
  if (dim < policy.min_dim || dim > policy.max_dim) {
    return Result<ModelOutput>::failure(Status::ValidationFailed,
                                        "shape out of policy dim bounds");
  }

  // Sanity + range (ordered iteration -> deterministic first violation).
  for (double v : out.values) {
    if (std::isnan(v) || std::isinf(v)) {
      return Result<ModelOutput>::failure(Status::ValidationFailed,
                                          "non-finite value (NaN/Inf)");
    }
    if (v < policy.min_value || v > policy.max_value) {
      return Result<ModelOutput>::failure(Status::ValidationFailed,
                                          "value out of range");
    }
  }

  return Result<ModelOutput>::success(out);
}

}  // namespace mlmodelconsumer
