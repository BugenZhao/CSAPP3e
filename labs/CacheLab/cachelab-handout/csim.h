//
// Created by Bugen Zhao on 2020/2/11.
//

#ifndef CSAPP_CSIM_H
#define CSAPP_CSIM_H

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *optString = "hvs:E:b:t:";
struct globalArgs_t {
    const char *prog;
    int verbosity;
    int s;
    int E;
    int b;
    char traceFile[80];
    int count;
} globalArgs;

typedef unsigned long long ull;
typedef ull add_t;
static const int w = sizeof(add_t) * 8;

typedef struct {
    ull timestamp;  // 最近一次访问时钟
    add_t tag;      // 标记位
    int valid;      // 有效位
    char *block;    // No use
} line_t;
typedef line_t *set_t;
typedef set_t *cache_t;

struct {
    cache_t cache;
    int setNum;
    int lineNum;
    int blockSize;
    int lineSize;
} CACHE;

ull TIMESTAMP;
ull MISS;
ull HIT;
ull EVICTION;

void printUsage();

void getArgs(int argc, char **argv);

void initCache();

void parse();

unsigned int process(const char *op, add_t ct, add_t ci, add_t co);

#endif //CSAPP_CSIM_H
