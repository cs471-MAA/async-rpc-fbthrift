default: build-services build-server


build-services:
	docker build -t saheru/fbthrift-benchmark .

build-server:
	docker build -t frontend-server frontend-server/