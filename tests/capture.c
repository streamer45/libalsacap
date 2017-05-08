#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lib.h"

int test() {
  int ret;
  ac_config_t config;
  alsacap_t *ac;
  uint8_t *buffer;
  size_t buffer_sz;
  memset(&config, 0, sizeof(config));
  ac = alsacap_open("hw:0,0");
  if (!ac) return -1;
  config.sample_rate = 48000;
  config.channels = 2;
  config.format = AC_INT16;
  ret = alsacap_config_set(ac, &config);
  if (ret != 0) return -2;
  alsacap_start(ac);
  buffer_sz = 2400 * config.channels * 2;
  buffer = malloc(buffer_sz);
  for (int i = 0; i < 200; ++i) {
    ret = alsacap_pcm_read(ac, buffer, buffer_sz);
    if (ret != buffer_sz) return -3;
    write(1, buffer, ret);
  }
  ret = alsacap_close(ac);
  if (ret != 0) return -4;
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
