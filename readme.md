# CSV 读取速度测试

## 准备

```
.\vcpkg\vcpkg install celero fast-cpp-csv-parser rapidcsv
```

## RUN


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