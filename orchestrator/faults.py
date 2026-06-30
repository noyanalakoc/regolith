"""
Week 3 placeholder.

Fault injection lives here:
  - crash faults: kill worker pods mid-run (Kubernetes notices and reschedules)
  - silent data corruption: flip bits in worker output with probability p
    (Kubernetes does NOT notice; the pod stays healthy and lies)
Empty for now on purpose.
"""
