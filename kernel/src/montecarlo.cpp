// montecarlo.cpp
// Estimates pi by Monte Carlo sampling of the unit quarter circle.
//
// This is one work unit. It is stateless and fully deterministic: given the
// same --seed and --samples it always produces the same result, down to the
// exact hit count. That determinism is the whole point. Later, two worker
// replicas running the same chunk MUST agree bit for bit, so any disagreement
// is proof of corruption (a simulated radiation bit-flip), not noise.
//
// Output is a single JSON line so the Python coordinator can parse it cleanly.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>

// splitmix64: a tiny, fast, deterministic PRNG. We avoid <random> so behavior
// is identical across compilers and platforms.
static inline uint64_t splitmix64(uint64_t &state) {
    uint64_t z = (state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

// Map a 64-bit integer to a double in [0, 1).
static inline double to_unit(uint64_t x) {
    return (x >> 11) * (1.0 / 9007199254740992.0); // 2^53
}

int main(int argc, char **argv) {
    uint64_t samples = 1000000ULL;
    uint64_t seed = 1ULL;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--samples" && i + 1 < argc) {
            samples = strtoull(argv[++i], nullptr, 10);
        } else if (a == "--seed" && i + 1 < argc) {
            seed = strtoull(argv[++i], nullptr, 10);
        }
    }
    if (seed == 0) seed = 1; // splitmix64 is fine with 0, but keep seeds nonzero by convention

    uint64_t state = seed;
    uint64_t hits = 0;
    for (uint64_t i = 0; i < samples; ++i) {
        double x = to_unit(splitmix64(state));
        double y = to_unit(splitmix64(state));
        if (x * x + y * y <= 1.0) ++hits;
    }

    double estimate = 4.0 * static_cast<double>(hits) / static_cast<double>(samples);

    // hits is the deterministic signature (checksum) of this chunk.
    printf("{\"seed\": %llu, \"samples\": %llu, \"hits\": %llu, \"estimate\": %.10f}\n",
           static_cast<unsigned long long>(seed),
           static_cast<unsigned long long>(samples),
           static_cast<unsigned long long>(hits),
           estimate);
    return 0;
}
