## Photobook cpp exercise

This project should be able to generate a pdf photobook based on a folder structure filled with photos.

##Build and Run
Create a ```build``` folder
```cmake -DPRINTER_TYPE=StandardPrinter -DLOG_LEVEL=Debug ..```
```make -j8```

```./PhotoBook```

#Tests
The tests are generated only when ```LOG_LEVEL=Debug```
```./pbtests```

#Static analysis
CMake with ```STATIC_CHECK=true```

## Boost modules
The boost uuid module is extracted using [bcp](https://www.boost.org/doc/libs/1_82_0/tools/bcp/doc/html/index.html) tool.
```bcp --namespace-alias detail uuid random integer tti function_types```
