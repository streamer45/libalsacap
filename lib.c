#include <stdio.h>
#include <stdlib.h>

#include "lib.h"

struct alsacap {
  snd_pcm_t *pcm;
  snd_pcm_hw_params_t *hw_params;
  ac_config_t config;
};

alsacap_t *alsacap_open(char *device) {
  int ret;
  snd_pcm_t *pcm;
  alsacap_t *ac;
  ac = calloc(1, sizeof(*ac));
  if (!ac) return NULL;
  ret = snd_pcm_open(&pcm, device, SND_PCM_STREAM_CAPTURE, 0);
  if (ret != 0) {
    ERROR("error opening device: %s", snd_strerror(ret));
    return NULL;
  }
  ac->pcm = pcm;
  ac->hw_params = calloc(1, snd_pcm_hw_params_sizeof());
  if (!ac->hw_params) return NULL;
  ret = snd_pcm_hw_params_any(pcm, ac->hw_params);
  if (ret != 0) {
    ERROR("%s", snd_strerror(ret));
    return NULL;
  }
  ret = snd_pcm_hw_params_set_access(pcm, ac->hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
  if (ret != 0) {
    ERROR("%s", snd_strerror(ret));
    return NULL;
  }
  return ac;
}

int alsacap_config_set(alsacap_t *cap, ac_config_t *config) {
  int ret;
  ret = snd_pcm_hw_params_set_format(cap->pcm, cap->hw_params,
     (snd_pcm_format_t)config->format);
  if (ret != 0) {
    ERROR("%s", snd_strerror(ret));
    return -1;
  }
  ret = snd_pcm_hw_params_set_rate(cap->pcm, cap->hw_params, config->sample_rate, 0);
  if (ret != 0) {
    ERROR("%s", snd_strerror(ret));
    return -2;
  }
  ret = snd_pcm_hw_params_set_channels(cap->pcm, cap->hw_params, config->channels);
  if (ret != 0) {
    ERROR("%s", snd_strerror(ret));
    return -3;
  }
  cap->config = *config;
  return 0;
}

int alsacap_config_get(alsacap_t *cap, ac_config_t *config) {
  if (!cap || !config) return -1;
  *config = cap->config;
  return 0;
}

int alsacap_start(alsacap_t *cap) {
  int ret;
  ret = snd_pcm_hw_params(cap->pcm, cap->hw_params);
  if (ret != 0) {
    ERROR("%s", snd_strerror(ret));
    return -4;
  }
  return 0;
}

ssize_t alsacap_pcm_read(alsacap_t *cap, uint8_t *data, size_t size) {
  int ret;
  size_t nframes;
  size_t format_width;
  size_t channels;
  format_width = snd_pcm_format_width(cap->config.format);
  channels = cap->config.channels;
  nframes = (size * 8) / (format_width * channels);
  ret = snd_pcm_readi(cap->pcm, data, nframes);
  if (ret < 0) {
    ERROR("%s", snd_strerror(ret));
    return -1;
  }
  return ret * (format_width / 8) * channels;
}

int alsacap_close(alsacap_t *ac) {
  int ret;
  if (!ac) return -1;
  if (ac->pcm) {
    ret = snd_pcm_close(ac->pcm);
    if (ret != 0) {
      ERROR("error closing device: %s", snd_strerror(ret));
      return -2;
    }
    ac->pcm = NULL;
  }
  if (ac->hw_params) {
    free(ac->hw_params);
    ac->hw_params = NULL;
  }
  free(ac);
  return 0;
}
