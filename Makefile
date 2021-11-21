IMAGE_NAME=saheru/fbthrift-benchmark

build-docker:
	docker build -m 4G -t ${IMAGE_NAME} .

build-and-copy-docker: container_id=$(shell docker create ${IMAGE_NAME})
build-and-copy-docker: build-docker
	docker cp ${container_id}:/app/src/build ./src/build
	docker rm -v ${container_id}
	cp ./src/build/compile_commands.json ./compile_commands.json
	sed -i 's/\/app\/src/\.\/src/g' compile_commands.json