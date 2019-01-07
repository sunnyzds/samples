#include "common.h"

namespace cfmini {

void GlobalInit(int* pargc, char*** pargv) {
  // Google flags.
  ::google::ParseCommandLineFlags(pargc, pargv, true);
  // Google logging.
  ::google::InitGoogleLogging(*(pargv)[0]);
  // Provide a backtrace on segfault.
  ::google::InstallFailureSignalHandler();
}

};
