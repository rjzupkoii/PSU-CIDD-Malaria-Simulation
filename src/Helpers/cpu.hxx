/*
 * cpu.h
 *
 * Retrieve the vendor reported CPU Brand String (ex., Intel(R) Core(TM) i7-8565U CPU @ 1.80GHz).
 */
#ifndef POMS_CPU_HXX
#define POMS_CPU_HXX

#ifdef _MSC_VER
#include <intrin.h>
#else
#include <cpuid.h>
#endif

static inline void cpuid(uint32_t level, int32_t output[4]) {
  memset(output, 0, sizeof(int32_t) * 4);

#   ifdef _MSC_VER
  __cpuid(output, static_cast<int>(level));
#   else
  __cpuid_count(level, 0, output[0], output[1], output[2], output[3]);
#   endif
}

// Retrieve the CPU Brand String for the CPU that we are currently running on. This will be a human friend string, so it
// should only be used for basic diagnostic information.
static std::string cpu_brand_string() {
  #define VENDOR_ID (0)
  #define EAX_Reg (0)

  // Define our buffers
  int32_t cpu_info[4] = { 0 };
  char buf[64] = { 0 };
  char out[64 + 6] = { 0 };

  // Call the CPU and determine what there is to retrieve
  cpuid(VENDOR_ID, cpu_info);
  if (cpu_info[EAX_Reg] >= 4) {
    for (uint32_t i = 0; i < 4; i++) {
      cpuid(0x80000002 + i, cpu_info);
      memcpy(buf + (i * 16), cpu_info, sizeof(cpu_info));
    }
  }

  // Parse the results into the output buffer
  size_t pos = 0;
  const size_t size = strlen(buf);
  for (size_t i = 0; i < size; ++i) {
    if (buf[i] == ' ' && ((pos > 0 && out[pos - 1] == ' ') || pos == 0)) {
      continue;
    }
    out[pos++] = buf[i];
  }
  if (pos > 0 && out[pos - 1] == ' ') {
    out[pos - 1] = '\0';
  }
  return out;
}

#endif
