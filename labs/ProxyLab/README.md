# BProxy
A concurrent pre-threaded toy web proxy created by Bugen.  
**BProxy** supports `HTTP` protocol as well as `GET`, `POST` and `HEAD` methods, which enables cache for `GET`.  
## Usage
- Run:
```shell script
./bproxy
./bproxy <port>
```
If `port` is not provided, **BTiny-m** will listen on port `10188`.  

## Results

- Through the web browsing test of Firefox, **BProxy** is proved to be a little bit robust.

```
*** Basic ***
Starting tiny on 2846
Starting proxy on 31659
1: home.html
   Fetching ./tiny/home.html into ./.proxy using the proxy
   Fetching ./tiny/home.html into ./.noproxy directly from Tiny
   Comparing the two files
   Success: Files are identical.
2: csapp.c
   Fetching ./tiny/csapp.c into ./.proxy using the proxy
   Fetching ./tiny/csapp.c into ./.noproxy directly from Tiny
   Comparing the two files
   Success: Files are identical.
3: tiny.c
   Fetching ./tiny/tiny.c into ./.proxy using the proxy
   Fetching ./tiny/tiny.c into ./.noproxy directly from Tiny
   Comparing the two files
   Success: Files are identical.
4: godzilla.jpg
   Fetching ./tiny/godzilla.jpg into ./.proxy using the proxy
   Fetching ./tiny/godzilla.jpg into ./.noproxy directly from Tiny
   Comparing the two files
   Success: Files are identical.
5: tiny
   Fetching ./tiny/tiny into ./.proxy using the proxy
   Fetching ./tiny/tiny into ./.noproxy directly from Tiny
   Comparing the two files
   Success: Files are identical.
Killing tiny and proxy
basicScore: 40/40

*** Concurrency ***
Starting tiny on port 2794
Starting proxy on port 6995
Starting the blocking NOP server on port 7906
Trying to fetch a file from the blocking nop-server
Fetching ./tiny/home.html into ./.noproxy directly from Tiny
Fetching ./tiny/home.html into ./.proxy using the proxy
Checking whether the proxy fetch succeeded
Success: Was able to fetch tiny/home.html from the proxy.
Killing tiny, proxy, and nop-server
concurrencyScore: 15/15

*** Cache ***
Starting tiny on port 30748
Starting proxy on port 29024
Fetching ./tiny/tiny.c into ./.proxy using the proxy
Fetching ./tiny/home.html into ./.proxy using the proxy
Fetching ./tiny/csapp.c into ./.proxy using the proxy
Killing tiny
Fetching a cached copy of ./tiny/home.html into ./.noproxy
Success: Was able to fetch tiny/home.html from the cache.
Killing proxy
cacheScore: 15/15

totalScore: 70/70

```

## Valid sources
```
.
├── CMakeLists.txt
├── proxy.c
├── proxy.h
├── rio_p.c
└── rio_p.h
```
Other sources are provided by CMake project.  
For CS:APP grader, all files are necessary instead.