#include "mlmodelconsumer/bridge.hpp"

#include "mlmodelconsumer/translator.hpp"

namespace mlmodelconsumer {

InferenceBridge::InferenceBridge(BridgeConfig config)
    : config_(std::move(config)) {}

GeometryMod InferenceBridge::consume(IModelSource& source,
                                     std::int64_t element_id) {
  // (1) Ingestion.
  Result<ModelOutput> fetched = source.fetch();
  if (!fetched.ok()) {
    return safe_fallback(element_id);  // (5) fail closed
  }

  const ModelOutput& out = fetched.value();

  // (2) Version compatibility.
  if (!is_compatible(config_.supported, out.version)) {
    return safe_fallback(element_id);  // (5)
  }

  // (3) Validation.
  Result<ModelOutput> validated = validate(out, config_.policy);
  if (!validated.ok()) {
    return safe_fallback(element_id);  // (5)
  }

  // (4) Deterministic translation.
  return translate(validated.value(), element_id);
}

}  // namespace mlmodelconsumer
