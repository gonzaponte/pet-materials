# -*-Makefile-*-

# Needed to make `"$@"` usable in recipes
set positional-arguments := true

default:
  just run --beam-on 10

test *REST:
  meson setup      build/pet-materials-tests test
  meson compile -C build/pet-materials-tests
  meson install -C build/pet-materials-tests
  sh install/pet-materials-tests/run-each-test-in-separate-process.sh "$@"

catch2-demo *REST:
  echo "$@"
  meson setup      build/pet-materials-tests test
  meson compile -C build/pet-materials-tests
  meson install -C build/pet-materials-tests
  install/pet-materials-tests/bin/catch2-demo-test "$@"

build:
  meson setup build/pet-materials src
  meson compile -C build/pet-materials

install: build
  meson install -C build/pet-materials

run *ARGS: install
  #!/usr/bin/env sh
  sh execute-with-nixgl-if-needed.sh ./install/pet-materials/bin/pet-materials "$@"
  exit $?

clean:
  rm build install -rf
