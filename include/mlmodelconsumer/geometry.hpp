#pragma once

// Domain target: BIM geometry modification.
// A GeometryMod is the deterministic product of translating a prediction.

#include <cstdint>
#include <string>

namespace mlmodelconsumer {

// 3D translation/scale applied to a BIM element, plus the target element id.
struct GeometryMod {
  std::int64_t element_id{0};
  double dx{0.0};
  double dy{0.0};
  double dz{0.0};
  double scale{1.0};
  bool is_fallback{false};  // true when produced by the safe fallback path
};

}  // namespace mlmodelconsumer
