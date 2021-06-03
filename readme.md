# CSV 读取速度测试

## 准备

```
set VCPKG_DEFAULT_TRIPLET = x64-windows
.\vcpkg\vcpkg install celero fast-cpp-csv-parser rapidcsv boost-iostreams
```

## 运行

运行吧

## 结果

i5-8400 release: 

```
Celero
Timer resolution: 0.100000 us
|     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |   RAM (bytes)   |
|:--------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|
|CSVTEST         | Baseline        |            Null |               3 |               3 |         1.00000 |    641145.66667 |            1.56 |          761856 |
|CSVTEST         | VINCE_CSV       |            Null |               3 |               3 |        11.01614 |   7062952.33333 |            0.14 |         3244032 |
|CSVTEST         | FAST_CSV_PARSER |            Null |               3 |               3 |         2.62605 |   1683683.66667 |            0.59 |         3375104 |
|CSVTEST         | RAPIDCSV        |            Null |               3 |               3 | SEH exception Unknown exception code.
Completed in 00:02:05.977905
```

另，对于Python如下代码有：
```python
import mmap
import time
import ctypes

t = time.time()
f = open(r"C:/Users/myuan/source/repos/csv-test/COX3.txt", "r")

mm = mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ)

res = ctypes.c_uint64(0)

while line := mm.readline():
    x, y, UMI_count = line.decode('utf-8').split('\t')
    res.value += int(x)

print(time.time() - t, res)

```

```
17.071086168289185 c_ulonglong(831502993036)
```