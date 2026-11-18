#ifndef DARKNET_CXX_PORT_PROJECT_RESOURCES_KEEPER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_RESOURCES_KEEPER_CLASS_H

#include <memory>
#include <vector>

namespace darknet_adaptor::adaptors
{

/**
 * @brief Keeps the resources of an adaptor no container of it holds (the
 * device arrays and the cuDNN descriptors of the GPU builds), each released
 * by the deleter of it's pointer with the keeper or on the release() of it.
 */
class ResourcesKeeper
{
 public:
  using Resource = std::shared_ptr<void>;

  virtual ~ResourcesKeeper() = default;

  void keep(Resource resource);

  /// @brief Releases the kept resource of the address, none for the other.
  void release(const void* address);

 private:
  std::vector<Resource> resources;
};

}  // namespace darknet_adaptor::adaptors

#endif  // DARKNET_CXX_PORT_PROJECT_RESOURCES_KEEPER_CLASS_H
