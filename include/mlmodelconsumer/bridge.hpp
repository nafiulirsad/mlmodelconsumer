#pragma once

// Orchestration: ties together ingestion (1), version check (2),
// validation (3), deterministic translation (4) and fail-closed fallback (5).
//
// consume() ALWAYS returns a usable GeometryMod. On any failure it returns the
// safe fallback (is_fallback == true) instead of throwing or returning garbage.

#include "mlmodelconsumer/fallback.hpp"
#include "mlmodelconsumer/geometry.hpp"
#include "mlmodelconsumer/model_source.hpp"
#include "mlmodelconsumer/validation.hpp"
#include "mlmodelconsumer/version_check.hpp"

namespace mlmodelconsumer {

struct BridgeConfig {
  SupportedVersion supported{};
  ValidationPolicy policy{};
};

class InferenceBridge {
 public:
  explicit InferenceBridge(BridgeConfig config);

  // Pulls one output from `source`, runs the full pipeline, and returns a
  // GeometryMod for `element_id`. Never throws; fails closed to safe_fallback.
  GeometryMod consume(IModelSource& source, std::int64_t element_id);

 private:
  BridgeConfig config_;
};

}  // namespace mlmodelconsumer
