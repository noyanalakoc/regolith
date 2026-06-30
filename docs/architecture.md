# Architecture

(Fill in as the project grows. This is your scratch space for the diagram and
the design decisions, and it doubles as raw material for a thesis proposal.)

## The idea in one paragraph

Kubernetes already recovers from crash faults: when a pod dies, it reschedules.
What it does not catch is silent data corruption, where a node stays healthy but
returns a wrong number. That is exactly the radiation-induced bit-flip problem in
a space compute environment. Regolith runs a real distributed compute job on a
real cluster, injects both failure modes, and benchmarks resilience strategies
against them.

## Components

- kernel: a deterministic C++ Monte Carlo work unit (estimates pi).
- coordinator: partitions the job, dispatches chunks, aggregates, records metrics.
- faults: crash injection (kill pods) and corruption injection (flip output bits).
- strategies: none / checkpoint-restart / replication-plus-voting.

## Why determinism matters

The kernel returns the same hit count for the same seed. Replicas of one chunk
must agree exactly, so any disagreement is corruption, not noise. No fuzzy
voting needed.

## Diagram

(TODO: add the forward-to-aft data flow: coordinator -> worker pods -> aggregate.)
