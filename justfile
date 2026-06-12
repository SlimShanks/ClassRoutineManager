set shell := ["bash", "-lc"]

build:
	cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && cmake --build build

run:
	@bin=$(find build -type f -perm -111 | head -n 1); \
	if [ -n "$bin" ]; then "$bin"; else echo "No executable found in build"; fi

clean:
	rm -rf build
