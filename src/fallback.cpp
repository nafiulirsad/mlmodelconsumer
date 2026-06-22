#include "mlmodelconsumer/fallback.hpp"

namespace mlmodelconsumer {

GeometryMod safe_fallback(std::int64_t element_id) {
  // Fail closed: identity transform, no displacement, no scaling.
  GeometryMod mod;
  mod.element_id = element_id;
  mod.dx = 0.0;
  mod.dy = 0.0;
  mod.dz = 0.0;
  mod.scale = 1.0;
  mod.is_fallback = true;
  return mod;
}

}  // namespace mlmodelconsumer
