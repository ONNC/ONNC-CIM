#ifndef TARGET_CLANG_GEN_SERVICE_LIBRARY_PASS_H_INCLUDED
#define TARGET_CLANG_GEN_SERVICE_LIBRARY_PASS_H_INCLUDED

#include "CLangMeta.h"

#include <onnc/Core/CustomPass.h>
#include <onnc/Support/Path.h>

namespace onnc {

class CLangGenServiceLibraryPass final : public CustomPass<CLangGenServiceLibraryPass>
{
  using stream_type = std::ostream;

public:
  CLangGenServiceLibraryPass() = default;
  CLangGenServiceLibraryPass(const CLangMeta& meta, Path outputFile)
    : meta{meta}
    , outputFile{std::move(outputFile)}
  {}

  CLangGenServiceLibraryPass(const CLangGenServiceLibraryPass&) = delete;
  CLangGenServiceLibraryPass(CLangGenServiceLibraryPass&&)      = delete;
  CLangGenServiceLibraryPass& operator=(const CLangGenServiceLibraryPass&) = delete;
  CLangGenServiceLibraryPass& operator=(CLangGenServiceLibraryPass&&) = delete;
  ~CLangGenServiceLibraryPass()                                       = default;

  ReturnType runOnModule(Module& module) override;
  void       addModelMainDefinition(stream_type& stream, const Module& module);

private:
  std::size_t getInternalMemorySize() const;

private:
  const CLangMeta& meta;
  const Path       outputFile;
};

} // namespace onnc

#endif
