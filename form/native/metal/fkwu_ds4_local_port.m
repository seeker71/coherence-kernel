#import <Foundation/Foundation.h>

#include "/Users/ursmuff/models/ds4-engine/ds4.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char *out;
    long long cap;
    long long used;
    int *tokens;
    int token_cap;
    int token_count;
    int progress_events;
} fk_ds4_observation;

static void fk_ds4_append(fk_ds4_observation *o, const char *fmt, ...) {
    if (o->used >= o->cap - 1) return;
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(o->out + o->used, (size_t)(o->cap - o->used), fmt, args);
    va_end(args);
    if (n < 0) return;
    o->used += n < o->cap - o->used ? n : o->cap - o->used - 1;
}

static double fk_ds4_now_ms(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec * 1000.0 + (double)t.tv_nsec / 1000000.0;
}

static char *fk_ds4_copy(const char *bytes, long long len) {
    if (len < 0) return NULL;
    char *copy = malloc((size_t)len + 1);
    if (copy == NULL) return NULL;
    memcpy(copy, bytes, (size_t)len);
    copy[len] = 0;
    return copy;
}

static void fk_ds4_emit(void *ud, int token) {
    fk_ds4_observation *o = ud;
    if (o->token_count < o->token_cap) o->tokens[o->token_count++] = token;
}

static void fk_ds4_done(void *ud) {
    fk_ds4_observation *o = ud;
    fk_ds4_append(o, "TRACE stage.generation.done=1\n");
}

static void fk_ds4_progress(void *ud, const char *event, int current, int total) {
    fk_ds4_observation *o = ud;
    o->progress_events++;
    fk_ds4_append(o, "TRACE stage.progress.event=%s current=%d total=%d\n",
                  event == NULL ? "unknown" : event, current, total);
}

long long fk_local_model_generate_external(
    const char *model, long long model_len,
    const char *prompt_csv, long long prompt_len,
    long long n_predict, char *out, long long cap) {
    @autoreleasepool {
        if (cap <= 0 || n_predict <= 0 || n_predict > 4096) return -1;
        fk_ds4_observation observation = {
            .out = out, .cap = cap, .used = 0,
            .tokens = calloc((size_t)n_predict, sizeof(int)),
            .token_cap = (int)n_predict, .token_count = 0, .progress_events = 0,
        };
        char *model_path = fk_ds4_copy(model, model_len);
        char *csv = fk_ds4_copy(prompt_csv, prompt_len);
        if (observation.tokens == NULL || model_path == NULL || csv == NULL) {
            free(observation.tokens);
            free(model_path);
            free(csv);
            return -1;
        }

        ds4_tokens prompt = {0};
        char *cursor = csv;
        while (*cursor != 0) {
            char *end = NULL;
            long token = strtol(cursor, &end, 10);
            if (end == cursor || token < 0 || token > INT32_MAX) {
                fk_ds4_append(&observation,
                    "FAIL local-ds4 prompt-token-csv offset=%ld\n",
                    (long)(cursor - csv));
                goto finish;
            }
            ds4_tokens_push(&prompt, (int)token);
            cursor = end;
            if (*cursor == ',') cursor++;
            else if (*cursor != 0) {
                fk_ds4_append(&observation,
                    "FAIL local-ds4 prompt-token-separator offset=%ld\n",
                    (long)(cursor - csv));
                goto finish;
            }
        }
        if (prompt.len == 0) {
            fk_ds4_append(&observation, "NOTHING local-ds4 prompt-tokens=0\n");
            goto finish;
        }

        double open_started = fk_ds4_now_ms();
        ds4_engine_options options = {0};
        options.model_path = model_path;
        options.backend = DS4_BACKEND_METAL;
        options.n_threads = 1;
        options.context_size = prompt.len + (int)n_predict + 16;
        options.power_percent = 100;
        options.quality = true;
        ds4_engine *engine = NULL;
        if (ds4_engine_open(&engine, &options) != 0 || engine == NULL) {
            fk_ds4_append(&observation,
                "FAIL local-ds4 engine-open\n"
                "rewitness-offer=inspect-model-shape-and-metal-memory\n");
            goto finish;
        }
        double opened_ms = fk_ds4_now_ms() - open_started;
        fk_ds4_append(&observation,
            "TRACE membrane.gguf-to-local-engine needed=1 "
            "source=form-model-resource destination=ds4-engine "
            "shape=%llu-bytes data=local-model-pages elapsed-ms=%.3f "
            "copy=mmap-owned-by-engine native-without-crossing="
            "candidate-form-owned-gguf-arena freshness=this-invocation outcome=success\n",
            (unsigned long long)ds4_engine_model_bytes(engine), opened_ms);
        fk_ds4_append(&observation,
            "TRACE dependency.layers=gguf:block-count:%d\n"
            "TRACE dependency.vocab=gguf:vocab-size:%d\n"
            "TRACE request.prompt-token-count=%d\n"
            "TRACE request.n-predict=%lld\n",
            ds4_engine_layer_count(engine), ds4_engine_vocab_size(engine),
            prompt.len, n_predict);

        double generation_started = fk_ds4_now_ms();
        int rc = ds4_engine_generate_argmax(
            engine, &prompt, (int)n_predict, options.context_size,
            fk_ds4_emit, fk_ds4_done, &observation,
            fk_ds4_progress, &observation);
        double generation_ms = fk_ds4_now_ms() - generation_started;
        if (rc != 0) {
            fk_ds4_append(&observation,
                "FAIL local-ds4 generation rc=%d elapsed-ms=%.3f\n"
                "rewitness-offer=inspect-last-progress-and-engine-observation\n",
                rc, generation_ms);
            ds4_engine_close(engine);
            goto finish;
        }
        fk_ds4_append(&observation, "DS4-GENERATED-TOKEN-IDS ");
        for (int i = 0; i < observation.token_count; i++) {
            fk_ds4_append(&observation, i == 0 ? "%d" : ",%d", observation.tokens[i]);
        }
        fk_ds4_append(&observation,
            "\nTRACE result.stop=engine-complete\n"
            "TRACE membrane.local-engine-to-form-result needed=1 "
            "source=ds4-engine destination=form-tokenizer "
            "shape=%d-u32-token-ids data=selected-token-ids elapsed-ms=%.3f "
            "copy=callback-values native-without-crossing="
            "candidate-form-owned-session freshness=this-invocation outcome=success\n"
            "ACK 1 local-in-process-generation prompt=%d generated=%d "
            "remote=0 network=0 host-exec=0 shell=0 swift=0 temp=0\n",
            observation.token_count, generation_ms,
            prompt.len, observation.token_count);
        ds4_engine_close(engine);

finish:
        ds4_tokens_free(&prompt);
        free(observation.tokens);
        free(model_path);
        free(csv);
        if (observation.used < observation.cap) observation.out[observation.used] = 0;
        return observation.used;
    }
}
