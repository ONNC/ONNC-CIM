//===- ONNCApp.cpp ----------------------------------------------------===//
//
//                             The ONNC Project
//
// See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "ONNCApp.h"

#include <onnc/Target/TargetSelect.h>
#include <onnc/Target/TargetRegistry.h>
#include <onnc/Target/TargetBackend.h>
#include <onnc/Target/TargetOptions.h>
#include <onnc/IRReader/ONNXReader.h>
#include <onnc/IR/Module.h>
#include <onnc/IR/ONNXUtils.h>
#include <onnc/Core/PassManager.h>
#include <onnc/ADT/Color.h>
#include <onnc/Support/IOStream.h>
#include <onnc/Transforms/Optimizations/OptimizationOptions.h>

#include <cstdlib>

#include <memory>
#include <string>

using namespace onnc;

//===----------------------------------------------------------------------===//
// ONNCApp
//===----------------------------------------------------------------------===//
ONNCApp::ONNCApp(int pArgc, char* pArgv[])
  : onnc::CoreApplication(pArgc, pArgv),
    m_Options() {
  InitializeAllPlatforms();
  InitializeAllBackends();
}

ONNCApp::~ONNCApp()
{
}

int ONNCApp::compile()
{
  onnc::onnx::Reader reader;
  Module module;
  SystemError err = reader.parse(options().input(), module);
  if (!err.isGood()) {
    // TODO: show error message
    return EXIT_FAILURE;
  }

  std::string error;
  std::string quadruple;
  options().quadruple().canonical(quadruple);
  const onnc::Target* target = TargetRegistry::Lookup(quadruple, error);
  if (nullptr == target) {
    errs() << Color::RED << "Error" << Color::RESET
           << ": can not found target `" << quadruple << "`: " << error
           << std::endl;
    return EXIT_FAILURE;
  }

  OptimizationOptions optOptions;

  PassManager pm;
  const auto backend = std::unique_ptr<TargetBackend>(target->createBackend(options().target()));
  backend->addTensorSel(pm);
  backend->addOnncIrOptimization(pm, optOptions);
  backend->addTensorSched(pm);
  backend->addMemAlloc(pm);
  backend->addCodeEmit(pm, options().output());

  pm.run(module);
  return EXIT_SUCCESS;
}
