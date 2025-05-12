#-
.PHONY: pv
pv:
	export VCPKG_ROOT=/Users/geovision/Documents/GitHub/vcpkg
	export PATH=$VCPKG_ROOT:$PATH

#- Build with vcpkg
.PHONY: buildv
buildv:
	cmake --preset=default
	cmake --build build

#-Build with vcpkg + Xcode?
.PHONY: build_xcode
build_xcode:
	cmake --preset=default -B build_xcode -G Xcode
	cmake --build build_xcode
#-
# (Ninja) ./build/app_src/gv_ff_app
# (Xcode) ./build/app_src/Debug/gv_ff_app
.PHONY: run
run:
	./build/app_src/Debug/gv_ff_app
	
#- clean for macOS
#-
.PHONY: clean
clean:
	rm -rf build_cmake
	rm -rf build_xcode
	rm -rf build
	clear
