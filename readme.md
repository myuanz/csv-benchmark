# CSV 读取速度测试

## 准备

```
set VCPKG_DEFAULT_TRIPLET = x64-windows
.\vcpkg\vcpkg install celero fast-cpp-csv-parser rapidcsv boost-iostreams folly
```

## 运行

运行吧

## 结果

i7-7700 release: 

自上到下分别为：
- 带先验知识的解析器
- 带先验知识的带OMP的解析器
- 带先验知识的带字符串的解析器
- 带先验知识的带字符串的带OMP的解析器
- VINCE_CSV库
- FAST_CSV_PARSER库
- RAPIDCSV库

```
Celero
Timer resolution: 0.100000 us
|     Group      |   Experiment    |   Prob. Space   |     Samples     |   Iterations    |    Baseline     |  us/Iteration   | Iterations/sec  |   RAM (bytes)   |
|:--------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|:---------------:|
|CSVTEST         | Baseline        |            Null |               3 |               3 |         1.00000 |    670740.00000 |            1.49 |          774144 |
|CSVTEST         | NAIVE_WITH_OMP  |            Null |               5 |               5 |         0.24368 |    163444.00000 |            6.12 |         2379776 |
|CSVTEST         | NAIVE_WITH_CHAR |            Null |               3 |               3 |         2.05309 |   1377089.66667 |            0.73 |         2379776 |
|CSVTEST         | NAIVE_WITH_COMP |            Null |               3 |               3 |         0.81300 |    545310.66667 |            1.83 |         2109440 |
|CSVTEST         | VINCE_CSV       |            Null |               3 |               3 |         9.49166 |   6366433.66667 |            0.16 |         6524928 |
|CSVTEST         | FAST_CSV_PARSER |            Null |               3 |               3 |         2.27451 |   1525607.66667 |            0.66 |         6524928 |
|CSVTEST         | RAPIDCSV        |            Null |               3 |               3 | SEH exception Unknown exception code.
Completed in 00:02:27.752923
```

## Python
对于Python如下代码有：
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

## 其他

- 代码未考虑OMP切分折行
- 代码未考虑工程化
