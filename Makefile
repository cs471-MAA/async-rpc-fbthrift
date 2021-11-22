IMAGE_NAME=saheru/fbthrift-benchmark

build-docker:
	docker build -m 4G -t ${IMAGE_NAME} .

build-and-copy-docker: build-docker copy-docker-build

copy-docker-build:
	rm -f ./compile_commands.json
	rm -fr ./src/build
	container_id=$$(docker create ${IMAGE_NAME}) \
	&& docker cp $${container_id}:/app/src/build ./src/build \
	&& docker cp $${container_id}:/app/src/dep/if/gen-cpp2/. ./src/dep/if/gen-cpp2; \
	docker rm -fv $${container_id}
	cp ./src/build/compile_commands.json ./compile_commands.json
	sed -i 's/\/app\/src/\.\/src/g' compile_commands.json
	# change our build files so '/app/src' becomes the corresponding directory on this machine
	DIR=$$(echo "$${PWD}/src" | sed 's/\//\\\//g; s/\./\\\./g'); for f in $$(find src/build -type f -print); do if file $$f | egrep '(ASCII)|(JSON)'; then sed -i 's/\/app\/src/'"$$DIR"'/g' $$f; fi; done