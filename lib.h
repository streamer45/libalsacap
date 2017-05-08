#include <stdint.h>
#include <alsa/asoundlib.h>

#define ERROR(fmt, ...) \
  { \
    fprintf(stderr, "E: %s@%s:%d ", __func__, __FILE__, __LINE__); \
    fprintf(stderr, fmt, ##__VA_ARGS__); \
    fprintf(stderr, "\n"); \
  }

typedef struct alsacap alsacap_t;

typedef enum {
  AC_INT16 = SND_PCM_FORMAT_S16_LE,
  AC_FLOAT = SND_PCM_FORMAT_FLOAT_LE
} ac_format_t;

typedef struct {
  ac_format_t format;
  uint sample_rate;
  uint channels;
} ac_config_t;

alsacap_t *alsacap_open(char *device);
int alsacap_config_set(alsacap_t *cap, ac_config_t *config);
int alsacap_config_get(alsacap_t *cap, ac_config_t *config);
int alsacap_start(alsacap_t *cap);
int alsacap_stop(alsacap_t *cap);
ssize_t alsacap_pcm_read(alsacap_t *cap, uint8_t *data, size_t size);
int alsacap_close(alsacap_t *ac);
