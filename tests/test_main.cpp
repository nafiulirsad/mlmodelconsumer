// Minimal smoke test: no external framework, just <cassert>.
// Exercises each of the five requirements.

#include <cassert>
#include <cmath>
#include <cstdio>
#include <memory>
#include <vector>

#include "mlmodelconsumer/bridge.hpp"
#include "mlmodelconsumer/fallback.hpp"
#include "mlmodelconsumer/model_source.hpp"
#include "mlmodelconsumer/translator.hpp"
#include "mlmodelconsumer/validation.hpp"
#include "mlmodelconsumer/version_check.hpp"

using namespace mlmodelconsumer;

namespace {

BridgeConfig make_config() {
  BridgeConfig c;
  c.supported.min = ModelVersion{1, 0, 0};
  c.supported.max = ModelVersion{1, 9, 9};
  c.policy.min_dim = 1;
  c.policy.max_dim = 16;
  c.policy.min_value = -100.0;
  c.policy.max_value = 100.0;
  return c;
}

ModelOutput make_good_output() {
  ModelOutput o;
  o.version = ModelVersion{1, 2, 0};
  o.model_id = "test";
  o.values = {2.0, 4.0, 6.0};
  o.expected_dim = 3;
  return o;
}

// Source backed by a pre-built output (for in-memory testing).
class CannedSource final : public IModelSource {
 public:
  explicit CannedSource(Result<ModelOutput> r) : r_(std::move(r)) {}
  Result<ModelOutput> fetch() override { return r_; }

 private:
  Result<ModelOutput> r_;
};

void test_version_check() {
  SupportedVersion s{ModelVersion{1, 0, 0}, ModelVersion{1, 9, 9}};
  assert(is_compatible(s, ModelVersion{1, 2, 0}));
  assert(!is_compatible(s, ModelVersion{0, 9, 9}));
  assert(!is_compatible(s, ModelVersion{2, 0, 0}));
}

void test_validation() {
  ValidationPolicy p;
  p.min_dim = 1;
  p.max_dim = 8;
  p.min_value = -10.0;
  p.max_value = 10.0;

  assert(validate(make_good_output(), p).ok());

  ModelOutput bad_shape = make_good_output();
  bad_shape.expected_dim = 99;  // mismatch
  assert(!validate(bad_shape, p).ok());

  ModelOutput bad_range = make_good_output();
  bad_range.values = {1000.0};
  bad_range.expected_dim = 1;
  assert(!validate(bad_range, p).ok());

  ModelOutput bad_nan = make_good_output();
  bad_nan.values = {std::nan("")};
  bad_nan.expected_dim = 1;
  assert(!validate(bad_nan, p).ok());
}

void test_deterministic_translate() {
  ModelOutput o = make_good_output();
  GeometryMod a = translate(o, 42);
  GeometryMod b = translate(o, 42);
  // Pure for fixed input: identical results.
  assert(a.dx == b.dx && a.dy == b.dy && a.dz == b.dz && a.scale == b.scale);
  assert(a.element_id == 42);
  assert(!a.is_fallback);
  assert(a.dx == 2.0 && a.dy == 4.0 && a.dz == 6.0);
}

void test_happy_path_bridge() {
  InferenceBridge bridge(make_config());
  CannedSource src(Result<ModelOutput>::success(make_good_output()));
  GeometryMod mod = bridge.consume(src, 7);
  assert(!mod.is_fallback);
  assert(mod.element_id == 7);
}

void test_fallback_on_failure() {
  InferenceBridge bridge(make_config());

  // Ingestion failure.
  CannedSource bad(
      Result<ModelOutput>::failure(Status::ReadError, "boom"));
  GeometryMod m1 = bridge.consume(bad, 1);
  assert(m1.is_fallback && m1.scale == 1.0 && m1.dx == 0.0);

  // Incompatible version.
  ModelOutput wrong_ver = make_good_output();
  wrong_ver.version = ModelVersion{5, 0, 0};
  CannedSource verSrc(Result<ModelOutput>::success(wrong_ver));
  GeometryMod m2 = bridge.consume(verSrc, 2);
  assert(m2.is_fallback);

  // Validation failure.
  ModelOutput invalid = make_good_output();
  invalid.expected_dim = 99;
  CannedSource valSrc(Result<ModelOutput>::success(invalid));
  GeometryMod m3 = bridge.consume(valSrc, 3);
  assert(m3.is_fallback);
}

void test_local_inference() {
  auto engine = std::make_shared<StubLocalInference>(ModelVersion{1, 0, 0});
  LocalInferenceSource src(engine, std::vector<double>{1.0, 2.0, 3.0});
  InferenceBridge bridge(make_config());
  GeometryMod mod = bridge.consume(src, 9);
  assert(!mod.is_fallback);

  // Empty features -> inference failure -> fallback.
  LocalInferenceSource emptySrc(engine, std::vector<double>{});
  GeometryMod fb = bridge.consume(emptySrc, 10);
  assert(fb.is_fallback);
}

}  // namespace

int main() {
  test_version_check();
  test_validation();
  test_deterministic_translate();
  test_happy_path_bridge();
  test_fallback_on_failure();
  test_local_inference();
  std::puts("all smoke tests passed");
  return 0;
}
