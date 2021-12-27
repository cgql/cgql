echo "Running benchmarks"
echo "Please wait... this might take a while"

python3 -m timeit "__import__('os').system('./build/tests/cgqlTests')"

echo "Benchmarking done"
