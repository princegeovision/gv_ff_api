#-
.PHONY: pv
pv:
	export VCPKG_ROOT=/Users/geovision/Documents/GitHub/vcpkg
	export PATH=$VCPKG_ROOT:$PATH

#-
.PHONY: buildv
buildv:
	cmake --preset=default
	cmake --build build

#-
.PHONY: run
run:
	./build/app_src/gv_ff_app
	
#- clean for macOS
#-
.PHONY: clean
clean:
	rm -rf build_cmake
	rm -rf build
	clear
