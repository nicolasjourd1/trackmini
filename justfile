build_dir := "build"
build_clang_dir := "build-clang"

build profile="debug":
    cmake -B {{ build_dir }} -DCMAKE_BUILD_TYPE={{ if profile == "release" { "Release" } else { "Debug" } }} -G Ninja
    cmake --build {{ build_dir }} --parallel

run: build
    ./{{ build_dir }}/src/trackmini

test: build
    ctest --test-dir {{ build_dir }} --output-on-failure

clean:
    rm -rf {{ build_dir }} {{ build_clang_dir }}

build-clang:
    cmake -B {{ build_clang_dir }} -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -G Ninja
    cmake --build {{ build_clang_dir }} --parallel
