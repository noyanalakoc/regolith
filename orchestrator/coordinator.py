"""
Week 1 coordinator.

Launches a single Monte Carlo job on the Kubernetes cluster, waits for it to
finish, then reads the result out of the pod logs and prints the pi estimate.

It talks to the cluster by shelling out to kubectl. That is deliberate: you can
run every one of these commands by hand to debug, and there are no Python
dependencies to install yet. The richer Python Kubernetes client comes later.

Usage:
    python orchestrator/coordinator.py
"""

import json
import subprocess
import sys

JOB_NAME = "montecarlo"
JOB_MANIFEST = "deploy/job.yaml"


def run(cmd):
    """Run a command and return the completed process (stdout/stderr captured)."""
    return subprocess.run(cmd, capture_output=True, text=True)


def main():
    # Remove any leftover job from a previous run so apply starts clean.
    run(["kubectl", "delete", "job", JOB_NAME, "--ignore-not-found"])

    # Launch the job.
    applied = run(["kubectl", "apply", "-f", JOB_MANIFEST])
    if applied.returncode != 0:
        print("failed to apply job:\n" + applied.stderr, file=sys.stderr)
        sys.exit(1)
    print("job launched, waiting for it to complete...")

    # Block until the job reports complete (or we time out).
    waited = run(["kubectl", "wait", "--for=condition=complete",
                  f"job/{JOB_NAME}", "--timeout=120s"])
    if waited.returncode != 0:
        print("job did not complete in time:\n" + waited.stderr, file=sys.stderr)
        print("inspect with: kubectl describe job/" + JOB_NAME, file=sys.stderr)
        sys.exit(1)

    # The kernel printed one JSON line. Read it from the pod logs.
    logs = run(["kubectl", "logs", f"job/{JOB_NAME}"]).stdout.strip()
    print("raw worker output: " + logs)

    result = json.loads(logs)
    print()
    print(f"pi estimate : {result['estimate']}")
    print(f"samples     : {result['samples']}")
    print(f"hits        : {result['hits']}  (deterministic checksum for this chunk)")


if __name__ == "__main__":
    main()
