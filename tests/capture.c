#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "lib.h"

/* Captures raw audio from an alsa input device and outputs it to stdin */

int test() {
  int ret;
  ac_config_t config;
  alsacap_t *ac;
  uint8_t *buffer;
  size_t buffer_sz;
  size_t nsamples;
  memset(&config, 0, sizeof(config));
  ac = alsacap_open("hw:0,0");
  if (!ac) return -1;
  config.sample_rate = 48000;
  config.channels = 2;
  config.format = AC_INT16;
  ret = alsacap_config_set(ac, &config);
  if (ret != 0) return -2;
  alsacap_start(ac);
  nsamples = 2048;
  buffer_sz = nsamples * config.channels * sizeof(int16_t);
  buffer = calloc(1, buffer_sz);
  for (;;) {
    ret = alsacap_pcm_read(ac, buffer, buffer_sz);
    if (ret < 0) return ret;
    ret = write(1, buffer, ret);
    if (ret < 0) return -4;
  }
  ret = alsacap_close(ac);
  if (ret != 0) return -5;
  free(buffer);
  return 0;
}

int main() {
  int ret;
  ret = test();
  if (ret != 0) {
    ERROR("Failure! %d", ret);
    return ret;
  }
  return 0;
}
