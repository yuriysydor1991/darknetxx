#ifndef DARKNET_CXX_PORT_PROJECT_WEIGHTS_REPORT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_WEIGHTS_REPORT_CLASS_H

#include <string>
#include <vector>

namespace darknet_adaptor::loaders
{

/**
 * @brief The findings of the validation of a weights file or a weights
 * document: the errors, which keep the file from being loaded or converted,
 * and the warnings about the file which loads, but not the way its author
 * might have expected (the truncated arrays, the data beyond the network
 * arrays and the like).
 */
class WeightsReport
{
 public:
  virtual ~WeightsReport() = default;
  WeightsReport() = default;

  void error(const std::string& message);
  void warning(const std::string& message);

  /// @brief Appends the findings of the other report.
  void merge(const WeightsReport& other);

  /// @brief True for no errors, the warnings aside.
  bool ok() const;

  const std::vector<std::string>& errors() const;
  const std::vector<std::string>& warnings() const;

  /// @brief Logs every finding of the subject (the file path, for example).
  void log(const std::string& subject) const;

  /// @brief The first error, the empty text for none.
  std::string first_error() const;

 private:
  std::vector<std::string> merrors;
  std::vector<std::string> mwarnings;
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_WEIGHTS_REPORT_CLASS_H
