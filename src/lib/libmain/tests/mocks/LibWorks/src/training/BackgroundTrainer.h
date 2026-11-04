#ifndef DARKNETXX_PROJECT_BACKGROUNDTRAINER_CLASS_H
#define DARKNETXX_PROJECT_BACKGROUNDTRAINER_CLASS_H

#include "ILib.h"
#include "ITrainer.h"
#include "LibraryContext.h"

namespace darknetxx_impl
{

class BackgroundTrainer : public darknetxx012::ITrainer
{
 public:
  BackgroundTrainer(darknetxx012::LibraryContextPtr nctx,
                    darknetxx012::ILibPtr ntraining)
      : ctx{nctx}, training{ntraining}
  {
  }

  bool train(const darknetxx012::ATrainingProject&) override { return true; }
  void stop() override {}
  bool running() const override { return false; }

  darknetxx012::LibraryContextPtr ctx;
  darknetxx012::ILibPtr training;
};

}  // namespace darknetxx_impl

#endif  // DARKNETXX_PROJECT_BACKGROUNDTRAINER_CLASS_H
