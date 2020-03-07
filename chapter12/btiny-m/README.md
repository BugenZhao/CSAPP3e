# BTiny-m
A concurrent pre-threaded toy web server created by Bugen, based on the "TINY Web Server" from CS:APP 3e.  
**BTiny-m** supports `GET`, `POST` and `HEAD` methods.  
The codes are naive and definitely UNSAFE and UNDERPERFORMED for any industrial use.
## Usage
- Run:
```shell script
./btiny-m
./btiny-m <port> [-d]
```
If `port` is not provided, **BTiny-m** will listen on port `1018`.  
If `-d` is set, each thread of server will sleep for 1 sec before processing the request, which is used for **BProxy** cache debugging.

- Benchmark:
```shell script
wrk -d4 http://localhost:<port>
```
## Todo
- Dynamically increase or decrease the number of threads