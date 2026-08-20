/* fk-mlx-carrier.c — MLX as an organ of THIS fkwu, not a second binary.
 *
 * Strong symbols for mlx_status / mlx_add. Weak stubs in runtime/fkwu-uni.c
 * answer mlx_linked=false when this file is not linked. Shrink target: the
 * Form walker owns the call; this carrier is checkout-witness, like Metal.
 */
#include "mlx/c/mlx.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

static long long fk_mlx_dispatch = 0;
static char fk_mlx_err[256] = "none";

static void fk_mlx_seterr(const char *m) {
    snprintf(fk_mlx_err, sizeof(fk_mlx_err), "%s", m ? m : "none");
}

long long fk_mlx_add_external(long long a, long long b) {
    mlx_device gpu = mlx_device_new_type(MLX_GPU, 0);
    bool gpu_ok = 0;
    mlx_device_is_available(&gpu_ok, gpu);
    mlx_stream s = gpu_ok ? mlx_default_gpu_stream_new() : mlx_default_cpu_stream_new();
    mlx_array xa = mlx_array_new_int((int)a);
    mlx_array xb = mlx_array_new_int((int)b);
    mlx_array xc = mlx_array_new();
    int rc = mlx_add(&xc, xa, xb, s);
    int ev = (rc == 0) ? mlx_array_eval(xc) : -1;
    int32_t v = 0;
    int it = (ev == 0) ? mlx_array_item_int32(&v, xc) : -1;
    mlx_array_free(xa);
    mlx_array_free(xb);
    mlx_array_free(xc);
    mlx_stream_free(s);
    mlx_device_free(gpu);
    if (rc != 0 || ev != 0 || it != 0) {
        fk_mlx_seterr("mlx_add eval/item failed");
        return 0;
    }
    fk_mlx_seterr("none");
    fk_mlx_dispatch++;
    return (long long)v;
}

long long fk_mlx_status_external(char *out, long long cap) {
    if (out == 0 || cap <= 0) {
        return 0;
    }
    bool metal = 0;
    mlx_metal_is_available(&metal);
    mlx_device gpu = mlx_device_new_type(MLX_GPU, 0);
    bool gpu_ok = 0;
    mlx_device_is_available(&gpu_ok, gpu);
    mlx_string ver = mlx_string_new();
    mlx_version(&ver);
    const char *vs = mlx_string_data(ver);
    if (vs == 0) {
        vs = "";
    }
    int n = snprintf(out, (size_t)cap,
        "mlx_owner=fkwu-form-cli\n"
        "mlx_linked=true\n"
        "mlx_metal_available=%s\n"
        "mlx_gpu_available=%s\n"
        "mlx_device=%s\n"
        "mlx_version=%s\n"
        "mlx_dispatch=%lld\n"
        "last_error=%s\n",
        metal ? "true" : "false",
        gpu_ok ? "true" : "false",
        gpu_ok ? "gpu" : "cpu",
        vs,
        fk_mlx_dispatch,
        fk_mlx_err);
    mlx_string_free(ver);
    mlx_device_free(gpu);
    if (n < 0) {
        return 0;
    }
    return (long long)n;
}
