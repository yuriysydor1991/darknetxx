#ifndef DARKNETXX_PROJECT_BACKGROUNDTRAINER_CLASS_H
#define DARKNETXX_PROJECT_BACKGROUNDTRAINER_CLASS_H

#include "ILib.h"
#include "ITrainer.h"
#include "LibraryContext.h"

namespace darknetxx_impl
{

class BackgroundTrainer : public darknetxx011::ITrainer
{
 public:
  BackgroundTrainer(darknetxx011::LibraryContextPtr nctx,
                    darknetxx011::ILibPtr ntraining)
      : ctx{nctx}, training{ntraining}
  {
  }

  bool train(const darknetxx011::ATrainingProject&) override { return true; }
  void stop() override {}
  bool running() const override { return false; }

  darknetxx011::LibraryContextPtr ctx;
  darknetxx011::ILibPtr training;
};

}  // namespace darknetxx_impl

#endif  // DARKNETXX_PROJECT_BACKGROUNDTRAINER_CLASS_H
