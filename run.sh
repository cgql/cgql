cd build
cmake ../cgql/ -DCMAKE_EXPORT_COMPILE_COMMANDS=1
make
cp ./cgql ~/cgql-out/

if [ $1 -gt 0 ]
then
  chmod 777 ~/cgql-out/cgql
fi
time ~/cgql-out/cgql
