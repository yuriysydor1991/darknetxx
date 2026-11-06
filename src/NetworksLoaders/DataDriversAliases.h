#ifndef DARKNET_CXX_PORT_PROJECT_DATA_DRIVERS_ALIASES_H
#define DARKNET_CXX_PORT_PROJECT_DATA_DRIVERS_ALIASES_H

// The one and only place in the whole project that names the namespace of the
// darknetxx-datadrivers library. Everything else - the weights documents, the
// formats and their readers and writers, the network sources, the cfg sections
// and the converter - is reached through the dxxdd alias declared below, so
// bumping the library to a namespace of the next version stays a one line
// change here.
//
// The header is included through the versioned
// <DarknetxxDataDrivers-0.12/...> directory the library installs, and not
// through the plain <DxxDataDriversFacade.h> the unprefixed include root would
// also resolve: the prefix pins the library version this translation unit
// talks to, and the generic names sitting on that same root - CfgSection.h,
// WeightsDocument.h - are the ones a busy include path collides with.
//
// DxxDataDriversFacade.h transitively pulls in every other public header of
// the library (the documents, the formats registry, the network sources, the
// cfg sections and structure, the YOLO4 folder layout and the converter), so a
// single include is enough.
#include <DarknetxxDataDrivers-0.12/DxxDataDriversFacade.h>

/// @brief The darknetxx-datadrivers library namespace, whose name carries the
/// library major and minor version numbers.
namespace dxxdd = DarknetxxDataDrivers012;

#endif  // DARKNET_CXX_PORT_PROJECT_DATA_DRIVERS_ALIASES_H
