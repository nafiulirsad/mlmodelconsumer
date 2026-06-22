#include "mlmodelconsumer/model_source.hpp"

#include <fstream>
#include <sstream>
#include <string>

namespace mlmodelconsumer {

namespace {

// Trim leading/trailing ASCII whitespace (deterministic).
std::string trim(const std::string& s) {
  std::size_t b = 0;
  std::size_t e = s.size();
  while (b < e && (s[b] == ' ' || s[b] == '\t' || s[b] == '\r')) ++b;
  while (e > b && (s[e - 1] == ' ' || s[e - 1] == '\t' || s[e - 1] == '\r')) --e;
  return s.substr(b, e - b);
}

bool parse_version(const std::string& v, ModelVersion& out) {
  std::uint32_t parts[3] = {0, 0, 0};
  std::size_t idx = 0;
  std::stringstream ss(v);
  std::string tok;
  while (std::getline(ss, tok, '.') && idx < 3) {
    try {
      parts[idx++] = static_cast<std::uint32_t>(std::stoul(trim(tok)));
    } catch (...) {
      return false;
    }
  }
  if (idx != 3) return false;
  out.major = parts[0];
  out.minor = parts[1];
  out.patch = parts[2];
  return true;
}

}  // namespace

FileModelReader::FileModelReader(std::string path) : path_(std::move(path)) {}

Result<ModelOutput> FileModelReader::fetch() {
  std::ifstream in(path_);
  if (!in.is_open()) {
    return Result<ModelOutput>::failure(Status::ReadError,
                                        "cannot open file: " + path_);
  }

  ModelOutput out;
  bool have_version = false;
  std::string line;
  while (std::getline(in, line)) {
    line = trim(line);
    if (line.empty() || line[0] == '#') continue;

    const auto eq = line.find('=');
    if (eq == std::string::npos) {
      return Result<ModelOutput>::failure(Status::ParseError,
                                          "malformed line: " + line);
    }
    const std::string key = trim(line.substr(0, eq));
    const std::string val = trim(line.substr(eq + 1));

    if (key == "version") {
      if (!parse_version(val, out.version)) {
        return Result<ModelOutput>::failure(Status::ParseError,
                                            "bad version: " + val);
      }
      have_version = true;
    } else if (key == "model_id") {
      out.model_id = val;
    } else if (key == "dim") {
      try {
        out.expected_dim = static_cast<std::int64_t>(std::stoll(val));
      } catch (...) {
        return Result<ModelOutput>::failure(Status::ParseError,
                                            "bad dim: " + val);
      }
    } else if (key == "values") {
      std::stringstream ss(val);
      std::string tok;
      while (std::getline(ss, tok, ',')) {
        tok = trim(tok);
        if (tok.empty()) continue;
        try {
          out.values.push_back(std::stod(tok));
        } catch (...) {
          return Result<ModelOutput>::failure(Status::ParseError,
                                              "bad value: " + tok);
        }
      }
    }
    // Unknown keys are ignored (forward-compatible).
  }

  if (!have_version) {
    return Result<ModelOutput>::failure(Status::ParseError,
                                        "missing version field");
  }
  return Result<ModelOutput>::success(std::move(out));
}

}  // namespace mlmodelconsumer
