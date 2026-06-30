# Single entrypoint for the project.
# On Windows/Git Bash you can also just run the underlying commands by hand;
# see the README quickstart.

CLUSTER := regolith
IMAGE := regolith-kernel:dev

.PHONY: up build demo down clean

up:        ## create the local k3d cluster
	k3d cluster create --config deploy/k3d-config.yaml

build:     ## build the kernel image and load it into the cluster
	docker build -t $(IMAGE) ./kernel
	k3d image import $(IMAGE) -c $(CLUSTER)

demo:      ## run one job through the coordinator
	python orchestrator/coordinator.py

down:      ## delete the cluster
	k3d cluster delete $(CLUSTER)

clean:     ## remove the leftover job
	kubectl delete job montecarlo --ignore-not-found
