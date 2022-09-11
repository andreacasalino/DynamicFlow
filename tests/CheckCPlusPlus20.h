#pragma once

#include <DynamicFlow/TypeTraits.h>

#include <iostream>

#define CHECK_CPLUSPLUS_20                                                     \
  if constexpr (!flw::CPLUPLUS_20_ENABLED) {                                   \
    std::cout << "\u001b[33;1m=============== SKIPPED "                        \
              << " ===============\u001b[0m" << std::endl;                     \
    return;                                                                    \
  }
