BUILD_DIR ?= build

.PHONY: configure build run clean

configure:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

build: configure
	cmake --build $(BUILD_DIR)

run: build
	@bin=$$(find $(BUILD_DIR) -type f -perm -111 | head -n 1); \
	if [ -n "$$bin" ]; then "$$bin"; else echo "No executable found in $(BUILD_DIR)"; fi

clean:
	rm -rf $(BUILD_DIR)
