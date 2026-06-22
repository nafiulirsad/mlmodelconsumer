#pragma once

// Requirement 1: Model output ingestion.
//   - IModelSource is the abstract ingestion interface.
//   - FileModelReader reads an output from disk.
//   - ILocalInference is the local-inference interface; LocalInferenceSource
//     adapts an inference engine to the same ingestion contract.

#include <memory>
#include <string>
#include <vector>

#include "mlmodelconsumer/model_output.hpp"
#include "mlmodelconsumer/result.hpp"

namespace mlmodelconsumer {

// Abstract source of model outputs.
class IModelSource {
 public:
  virtual ~IModelSource() = default;
  virtual Result<ModelOutput> fetch() = 0;
};

// File-based reader: parses a simple, deterministic text format from disk.
// Format (one field per line):
//   version=<maj>.<min>.<patch>
//   model_id=<string>
//   dim=<int>
//   values=<d0>,<d1>,...
class FileModelReader final : public IModelSource {
 public:
  explicit FileModelReader(std::string path);
  Result<ModelOutput> fetch() override;

 private:
  std::string path_;
};

// Local-inference interface: given an ordered feature vector, produce a
// ModelOutput. Implementations must be deterministic for fixed input.
class ILocalInference {
 public:
  virtual ~ILocalInference() = default;
  virtual Result<ModelOutput> infer(const std::vector<double>& features) = 0;
};

// Adapts an ILocalInference + fixed feature vector into an IModelSource so the
// bridge can treat file reads and local inference uniformly.
class LocalInferenceSource final : public IModelSource {
 public:
  LocalInferenceSource(std::shared_ptr<ILocalInference> engine,
                       std::vector<double> features);
  Result<ModelOutput> fetch() override;

 private:
  std::shared_ptr<ILocalInference> engine_;
  std::vector<double> features_;
};

// A trivial deterministic stub engine (identity-ish): exists so the project
// compiles and the test has something concrete to exercise.
class StubLocalInference final : public ILocalInference {
 public:
  explicit StubLocalInference(ModelVersion version);
  Result<ModelOutput> infer(const std::vector<double>& features) override;

 private:
  ModelVersion version_;
};

}  // namespace mlmodelconsumer
