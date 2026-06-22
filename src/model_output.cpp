#include "mlmodelconsumer/version_check.hpp"

namespace mlmodelconsumer {

bool is_compatible(const SupportedVersion& supported, const ModelVersion& v) {
  // Inclusive window check; deterministic.
  if (v < supported.min) return false;
  if (supported.max < v) return false;
  return true;
}

}  // namespace mlmodelconsumer
