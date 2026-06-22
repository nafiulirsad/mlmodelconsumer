#pragma once

// Requirement 4: Deterministic translation -> prediction to geometry mod.
// translate() is PURE for a fixed input: no clock, no RNG, no unordered
// container iteration. Same ModelOutput in => byte-identical GeometryMod out.

#include "mlmodelconsumer/geometry.hpp"
#include "mlmodelconsumer/model_output.hpp"

namespace mlmodelconsumer {

// Maps a validated ModelOutput to a GeometryMod for `element_id`.
// Deterministic by construction (fixed arithmetic over the ordered vector).
GeometryMod translate(const ModelOutput& out, std::int64_t element_id);

}  // namespace mlmodelconsumer
