# Multi-stage build.
# Stage 1 compiles the C++ kernel with a full toolchain.
# Stage 2 copies just the binary into a small runtime image, so the final
# image does not carry the compiler around.

# ---- build stage ----
FROM debian:bookworm-slim AS build
RUN apt-get update && apt-get install -y --no-install-recommends \
    g++ cmake make \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /src
COPY CMakeLists.txt .
COPY src/ ./src/
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build

# ---- runtime stage ----
FROM debian:bookworm-slim
COPY --from=build /src/build/montecarlo /usr/local/bin/montecarlo
ENTRYPOINT ["/usr/local/bin/montecarlo"]
# Default args, overridden by the k8s Job spec.
CMD ["--samples", "1000000", "--seed", "1"]
