#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define SAMPLE_RATE 11025
#define DURATION_SECONDS 1
#define FREQUENCY 440.0
#define AMPLITUDE 127.0   /* 8-bit unsigned range is 0–255 */
#define OFFSET 128        /* Center for unsigned 8-bit audio */

void write_wav_header(FILE *f, int sample_rate, int num_samples) {
    uint32_t byte_rate = sample_rate;        // mono, 8-bit
    uint32_t data_size = num_samples;
    uint32_t chunk_size = 36 + data_size;

    /* RIFF header */
    fwrite("RIFF", 1, 4, f);
    fwrite(&chunk_size, 4, 1, f);
    fwrite("WAVE", 1, 4, f);

    /* fmt chunk */
    fwrite("fmt ", 1, 4, f);
    uint32_t subchunk1_size = 16;
    uint16_t audio_format = 1;   // PCM
    uint16_t num_channels = 1;
    uint16_t bits_per_sample = 8;
    uint16_t block_align = num_channels * bits_per_sample / 8;

    fwrite(&subchunk1_size, 4, 1, f);
    fwrite(&audio_format, 2, 1, f);
    fwrite(&num_channels, 2, 1, f);
    fwrite(&sample_rate, 4, 1, f);
    fwrite(&byte_rate, 4, 1, f);
    fwrite(&block_align, 2, 1, f);
    fwrite(&bits_per_sample, 2, 1, f);

    /* data chunk */
    fwrite("data", 1, 4, f);
    fwrite(&data_size, 4, 1, f);
}

int main(void) {
    FILE *f = fopen("sine_440hz.wav", "wb");
    if (!f) {
        perror("fopen");
        return 1;
    }

    int num_samples = SAMPLE_RATE * DURATION_SECONDS;
    write_wav_header(f, SAMPLE_RATE, num_samples);

    for (int i = 0; i < num_samples; i++) {
        double t = (double)i / SAMPLE_RATE;
        double sample = sin(2.0 * M_PI * FREQUENCY * t);
        uint8_t pcm = (uint8_t)(OFFSET + AMPLITUDE * sample);
        fwrite(&pcm, 1, 1, f);
    }

    fclose(f);
    return 0;
}
