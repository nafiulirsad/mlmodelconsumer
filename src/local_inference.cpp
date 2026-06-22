#include "mlmodelconsumer/model_source.hpp"

namespace mlmodelconsumer {

LocalInferenceSource::LocalInferenceSource(
    std::shared_ptr<ILocalInference> engine, std::vector<double> features)
    : engine_(std::move(engine)), features_(std::move(features)) {}

Result<ModelOutput> LocalInferenceSource::fetch() {
  if (!engine_) {
    return Result<ModelOutput>::failure(Status::InferenceFailed,
                                        "null inference engine");
  }
  return engine_->infer(features_);
}

StubLocalInference::StubLocalInference(ModelVersion version)
    : version_(version) {}

Result<ModelOutput> StubLocalInference::infer(
    const std::vector<double>& features) {
  // Deterministic stub: echoes features with a fixed affine transform.
  // No clock, no RNG, ordered iteration only.
  if (features.empty()) {
    return Result<ModelOutput>::failure(Status::InferenceFailed,
                                        "empty feature vector");
  }
  ModelOutput out;
  out.version = version_;
  out.model_id = "stub-local";
  out.expected_dim = static_cast<std::int64_t>(features.size());
  out.values.reserve(features.size());
  for (double f : features) {
    out.values.push_back(f * 0.5 + 1.0);
  }
  return Result<ModelOutput>::success(std::move(out));
}

}  // namespace mlmodelconsumer
