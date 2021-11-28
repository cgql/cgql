function build() {
  cd build
  cmake ../cgql/ -DCMAKE_EXPORT_COMPILE_COMMANDS=1
  make
  cp ./cgql ~/cgql-out/

  chmod 777 ~/cgql-out/cgql
  ~/cgql-out/cgql
}

time build
