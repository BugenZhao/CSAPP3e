# BTiny-m
A concurrent pre-threaded toy web server created by Bugen, based on the "TINY Web Server" from CS:APP 3e.  
The codes are naive and definitely UNSAFE and UNDERPERFORMED for any industrial use.
## Usage
- Run:
```shell script
./btiny-m <port>[default: 1018]
```
- Benchmark:
```shell script
wrk -d4 http://localhost:<port>
```
## Todo
- Dynamically increase or decrease the number of threads