.PHONY: tests
tests:
	cmake -S . -B build
	cmake --build build

.PHONY: test
test:
	cd build && ctest
	cd ..

.PHONY: clean
clean:
	rm -rf build
