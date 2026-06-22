#include "mlmodelconsumer/translator.hpp"

namespace mlmodelconsumer {

GeometryMod translate(const ModelOutput& out, std::int64_t element_id) {
  // Deterministic mapping over the ordered prediction vector.
  // First three components drive translation; their mean drives scale.
  // No time, no RNG, no unordered iteration.
  GeometryMod mod;
  mod.element_id = element_id;
  mod.is_fallback = false;

  const std::size_t n = out.values.size();
  mod.dx = (n > 0) ? out.values[0] : 0.0;
  mod.dy = (n > 1) ? out.values[1] : 0.0;
  mod.dz = (n > 2) ? out.values[2] : 0.0;

  double sum = 0.0;
  for (double v : out.values) sum += v;
  // Clamp scale to a sane positive range so the result is BIM-usable.
  double scale = (n > 0) ? (1.0 + sum / static_cast<double>(n)) : 1.0;
  if (scale < 0.01) scale = 0.01;
  mod.scale = scale;

  return mod;
}

}  // namespace mlmodelconsumer
