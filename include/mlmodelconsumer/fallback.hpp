#pragma once

// Requirement 5: Fallback logic. On missing/malformed input or inference
// failure the bridge fails CLOSED -> a safe, defined, no-op-ish GeometryMod.

#include "mlmodelconsumer/geometry.hpp"

namespace mlmodelconsumer {

// The safe fallback: identity transform on the element, flagged is_fallback.
// Deterministic and side-effect free.
GeometryMod safe_fallback(std::int64_t element_id);

}  // namespace mlmodelconsumer
