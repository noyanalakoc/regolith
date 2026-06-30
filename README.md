# Regolith

A fault-tolerant distributed compute orchestrator for edge clusters under failure,
framed around the lunar edge-computing reliability problem.

> Status: early. Week 1 spine is in place. The resilience strategies and the
> headline benchmark land in the weeks below. This README describes the target
> and is honest about what exists today.

## The problem

Kubernetes already recovers from crash faults. A pod dies, it reschedules, the
job continues. What Kubernetes does not catch is silent data corruption: a node
stays healthy and returns a wrong answer. On Earth that is rare. In a radiation
environment, single-event upsets (bit flips) make it routine, and a healthy pod
quietly returning a wrong number is the dangerous case, because nothing alarms.

Regolith runs a real distributed compute job on a real cluster, injects both
failure modes (crashes and corruption), and measures how different resilience
strategies trade correctness against cost.

## What it does

- Runs a deterministic compute kernel as containers on a real (local) Kubernetes cluster.
- Injects crash faults (kills pods) and silent corruption (flips output bits).
- Implements and benchmarks resilience strategies: none, checkpoint-restart, and replication with voting.
- Produces a single figure: correctness and overhead versus fault rate, per strategy.

The expected result, and the point of the whole thing: checkpoint-restart stays
fast but returns wrong answers under corruption (it checkpoints corrupted
partials), while replication stays correct at a multiple of the compute cost.
A clean reliability-versus-cost trade-off.

## Quickstart

Requires Docker Desktop, k3d, and kubectl. Python 3 for the coordinator.

```bash
# 1. create the local cluster
k3d cluster create --config deploy/k3d-config.yaml

# 2. build the kernel image and load it into the cluster
docker build -t regolith-kernel:dev ./kernel
k3d image import regolith-kernel:dev -c regolith

# 3. run one job and read the result
python orchestrator/coordinator.py

# 4. tear down when done
k3d cluster delete regolith
```

You should see a pi estimate printed from a container running on the cluster.

## Architecture

A C++ kernel computes one work unit (Monte Carlo estimation of pi). It is
stateless and deterministic: the same seed gives the same result, exactly, so
replicas of a chunk must agree, and a disagreement is proof of corruption. A
Python coordinator partitions the job, dispatches chunks to worker pods,
aggregates, and runs the chosen resilience strategy. See `docs/architecture.md`.

## Roadmap

- Week 1: kernel, container, single job on k3d, coordinator, CI. (done)
- Week 2: distribute the job across multiple worker pods, baseline timing.
- Week 3: inject crash and bit-flip faults with tunable rates.
- Week 4: checkpoint-restart and replication-voting, plus the headline figure.
- Week 5: infrastructure-as-code provisioning (Terraform plus Ansible).

## What this is, and what it is not

It is a working demonstration and benchmark of corruption-tolerant distributed
compute, on commodity hardware, with the radiation environment simulated. It is
a research and portfolio tool and a prototype for resilient edge infrastructure.

It is not flight software, it does not model real spacecraft hardware, and it is
not a substitute for radiation-hardened systems. The lunar framing is the design
motivation, not a claim of flight readiness.

## License

MIT.
