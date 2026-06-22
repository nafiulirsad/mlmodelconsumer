# MLModelConsumer — C++ Inference Bridge for BIM

A C++17 bridge that consumes ML model outputs (file-based or local inference)
and translates predictions into **deterministic** BIM geometry modifications,
with versioning, validation, and fail-closed fallback.

## Build

Requires CMake >= 3.16 and a C++17 compiler.

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure   # or: ./build/test_main
```

## Layout

```
CMakeLists.txt                  # builds lib `mlmodelconsumer` + `test_main`
include/mlmodelconsumer/*.hpp   # public domain interfaces
src/*.cpp                       # implementations
tests/test_main.cpp             # assert-based smoke test (no framework)
```

## Requirement mapping

| # | Requirement | Where |
|---|-------------|-------|
| 1 | Model output ingestion (file + local inference) | `model_source.hpp`: `IModelSource`, `FileModelReader`, `ILocalInference`, `LocalInferenceSource`, `StubLocalInference` (`src/file_reader.cpp`, `src/local_inference.cpp`) |
| 2 | Model versioning + compatibility check | `model_output.hpp`: `ModelVersion`; `version_check.hpp`: `SupportedVersion`, `is_compatible()` (`src/model_output.cpp`) |
| 3 | Output validation (shape / range / sanity) | `validation.hpp`: `ValidationPolicy`, `validate()` (`src/validation.cpp`) |
| 4 | Deterministic translation (prediction -> geometry) | `translator.hpp`: `translate()` — pure for fixed input, ordered iteration only, no clock/RNG (`src/translator.cpp`) |
| 5 | Fallback logic (fail closed) | `fallback.hpp`: `safe_fallback()`; orchestrated in `bridge.hpp`/`src/bridge.cpp` — any failure returns the safe identity `GeometryMod` with `is_fallback == true` |

## Determinism contract

`translate()` is pure: the same `ModelOutput` always yields a byte-identical
`GeometryMod`. No wall-clock time, no random number generation, and no iteration
over unordered containers — prediction data is held in an ordered
`std::vector<double>`.

## Fail-closed contract

`InferenceBridge::consume()` never throws and never returns garbage. On read
error, parse error, incompatible version, validation failure, or inference
failure it returns `safe_fallback()` (identity transform, `is_fallback = true`).

## File model format

`FileModelReader` parses a simple line-based format:

```
version=1.2.0
model_id=my-model
dim=3
values=2.0,4.0,6.0
```

Unknown keys are ignored for forward compatibility.
