#pragma once

// Requirement 1 (ingestion data) + Requirement 2 (versioning):
// The ModelOutput carries the raw prediction payload AND the version metadata
// that the compatibility check operates on.

#include <cstdint>
#include <string>
#include <vector>

namespace mlmodelconsumer {

// Semantic-ish version of the model that produced an output.
// Compatibility is decided against the consumer's SupportedVersion range.
struct ModelVersion {
  std::uint32_t major{0};
  std::uint32_t minor{0};
  std::uint32_t patch{0};

  // Deterministic ordering helper (no time / no RNG).
  bool operator==(const ModelVersion& o) const {
    return major == o.major && minor == o.minor && patch == o.patch;
  }
  bool operator<(const ModelVersion& o) const {
    if (major != o.major) return major < o.major;
    if (minor != o.minor) return minor < o.minor;
    return patch < o.patch;
  }
};

// A single raw prediction emitted by the model.
// `values` is an ordered vector (NOT an unordered container) so that any
// downstream iteration is deterministic for a fixed input.
struct ModelOutput {
  ModelVersion version{};
  std::string model_id;          // logical model name
  std::vector<double> values;    // ordered prediction vector
  std::int64_t expected_dim{0};  // declared dimensionality (for shape check)
};

}  // namespace mlmodelconsumer
