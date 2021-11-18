cd build
cmake ../cgql/
make
cp ./cgql ~/cgql-out/

if [ $1 -gt 0 ]
then
  chmod 777 ~/cgql-out/cgql
fi
time ~/cgql-out/cgql
