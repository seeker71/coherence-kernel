// stdlib-standing-oracle.cpp — the host's C++ standard library doing the work
// form/form-stdlib/stdlib-standing.bml asks of the body's core.fk recipes, measured
// the same way: process CPU time, the first 32 calls of each family from a cold
// start, then the median of five 512-call batches after 2048 warm calls. The
// sequence is 1..19 built at run time; results fold into a printed sink so the
// optimizer keeps every call. Prints one line per family: name cold_ns warm_ns.
#include <algorithm>
#include <charconv>
#include <cstdio>
#include <ctime>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

static long long cpu_ns() {
    timespec t;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);
    return t.tv_sec * 1000000000LL + t.tv_nsec;
}
template <class T> static inline void keep(T const& v) { asm volatile("" : : "r,m"(v) : "memory"); }

static std::vector<long> xs;
static std::string digits;

static long accumulate_(long) { return std::accumulate(xs.begin(), xs.end(), 0L, [](long a, long b) { return a + b; }); }
static long transform_(long) {
    std::vector<long> out(xs.size());
    std::transform(xs.begin(), xs.end(), out.begin(), [](long x) { return x * 2; });
    keep(out.data());
    return static_cast<long>(out.size());
}
static long copy_if_(long) {
    std::vector<long> out;
    std::copy_if(xs.begin(), xs.end(), std::back_inserter(out), [](long x) { return x % 2 == 0; });
    keep(out.data());
    return static_cast<long>(out.size());
}
static long iota_take_(long) {
    std::vector<long> r(19);
    std::iota(r.begin(), r.end(), 0L);
    std::vector<long> t(r.begin(), r.begin() + 5);
    keep(t.data());
    return static_cast<long>(t.size());
}
static long reduce_(long) { return std::accumulate(xs.begin(), xs.end(), 0L) + *std::max_element(xs.begin(), xs.end()); }
static long to_string_(long n) { return static_cast<long>(std::to_string(n + 123456789).size()); }
static long stoi_(long) { return std::stol(digits); }
static long to_chars_(long n) {
    char b[64];
    auto r = std::to_chars(b, b + sizeof b, 0.1 + static_cast<double>(n) * 0.001);
    keep(b);
    return static_cast<long>(r.ptr - b);
}

static long run(long (*f)(long), long calls, long base) {
    long t = 0;
    for (long i = 0; i < calls; i++) {
        asm volatile("" : : : "memory");  // each call reads its inputs afresh: nothing hoists out of the loop
        long v = f(base + i);
        keep(v);
        t += v;
    }
    return t;
}

int main(int argc, char**) {
    for (long i = 1; i <= 19; i++) xs.push_back(i + (argc - 1));
    digits = std::string("123456789");
    keep(xs.data());
    timespec res;
    clock_getres(CLOCK_PROCESS_CPUTIME_ID, &res);
    std::printf("clock-res-ns %ld 32\n", res.tv_nsec);
    struct family { const char* name; long (*f)(long); };
    const family fams[] = {{"accumulate", accumulate_}, {"transform", transform_}, {"copy_if", copy_if_},
                           {"iota+take", iota_take_},   {"reduce", reduce_},       {"to_string", to_string_},
                           {"stoi", stoi_},             {"to_chars", to_chars_}};
    long sink = 0;
    for (const family& fam : fams) {
        long long t0 = cpu_ns();
        sink += run(fam.f, 32, 0);
        long long cold = (cpu_ns() - t0) / 32;
        sink += run(fam.f, 2048, 32);
        long long w[5];
        for (int k = 0; k < 5; k++) {
            long long a = cpu_ns();
            sink += run(fam.f, 512, 4096 + k * 512);
            w[k] = (cpu_ns() - a) / 512;
        }
        std::sort(w, w + 5);
        std::printf("%s %lld %lld\n", fam.name, cold, w[2]);
    }
    std::printf("sink %ld\n", sink);
    return 0;
}
