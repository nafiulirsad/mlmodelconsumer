#pragma once

// Requirement 2: Model versioning -> compatibility check.

#include "mlmodelconsumer/model_output.hpp"

namespace mlmodelconsumer {

// Inclusive [min, max] window of model versions this consumer accepts.
struct SupportedVersion {
  ModelVersion min{0, 0, 0};
  ModelVersion max{0, 0, 0};
};

// Pure predicate: is `v` within the supported window?
// Deterministic, no side effects.
bool is_compatible(const SupportedVersion& supported, const ModelVersion& v);

}  // namespace mlmodelconsumer
