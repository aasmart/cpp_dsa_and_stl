.PHONY: tests
tests:
	cmake -S . -B build
	cmake --build build

.PHONY: test
test:
	cd build && ctest --output-on-failure
	cd ..

.PHONY: clean
clean:
	rm -rf build
