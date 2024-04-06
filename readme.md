```
mkdir build && cd build
cmake ..
make -j

./REST <n_cores>
```

for benchmarks install https://k6.io/docs/get-started/installation/, then run the benchmark with `k6 run  --summary-trend-stats="min,med,avg,max,p(99),p(99.9),p(99.99)" bench2.js`
