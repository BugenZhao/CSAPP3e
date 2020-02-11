#include "cachelab.h"
#include "csim.h"

void printUsage() {
    const char *prog = globalArgs.prog;
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n"
           "Options:\n"
           "  -h         Print this help message.\n"
           "  -v         Optional verbose flag.\n"
           "  -s <num>   Number of set index bits.\n"
           "  -E <num>   Number of lines per set.\n"
           "  -b <num>   Number of block offset bits.\n"
           "  -t <file>  Trace file.\n"
           "\n"
           "Examples:\n"
           "  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n"
           "  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n",
           prog, prog, prog);
}

void getArgs(int argc, char **argv) {
    globalArgs.prog = argv[0];
    globalArgs.verbosity = 0;
    globalArgs.s = -1;
    globalArgs.E = -1;
    globalArgs.count = 0;

    int opt = -1;
    while ((opt = getopt(argc, argv, optString)) != -1) {
        switch (opt) {
            case 'h':
                printUsage();
                exit(0);
            case 'v':
                globalArgs.verbosity = 1;
                break;
            case 's':
                ++globalArgs.count;
                globalArgs.s = atoi(optarg);
                break;
            case 'E':
                ++globalArgs.count;
                globalArgs.E = atoi(optarg);
                break;
            case 'b':
                ++globalArgs.count;
                globalArgs.b = atoi(optarg);
                break;
            case 't':
                ++globalArgs.count;
                strcpy(globalArgs.traceFile, optarg);
                break;
            default:
                printUsage();
                exit(-1);
        }
    }
    switch (globalArgs.count) {
        default:
            printf("%s: Missing required command line argument\n",
                   globalArgs.prog);
        case 0:
            printUsage();
            exit(-1);
        case 4:
            break;
    }
}

void initCache() {
    CACHE.setNum = 1 << globalArgs.s;
    CACHE.lineNum = globalArgs.E;
    CACHE.blockSize = 1 << globalArgs.b;
    CACHE.cache = calloc(CACHE.setNum, sizeof(set_t));
    for (int i = 0; i < CACHE.setNum; ++i) {
        CACHE.cache[i] = calloc(CACHE.lineNum, sizeof(line_t));
        for (int j = 0; j < CACHE.lineNum; ++j) {
            CACHE.cache[i][j].block = malloc(CACHE.blockSize);
        }
    }
}

void parse() {
    FILE *fp = fopen(globalArgs.traceFile, "r");

    char op[5];
    add_t address;
    int size;

    while (fscanf(fp, "%s %llx,%d", op, &address, &size) != EOF) {
        add_t co, ci, ct;
        co = address & ((1UL << globalArgs.b) - 1);
        ci = (address & ((1UL << (globalArgs.b + globalArgs.s)) - 1)) >> globalArgs.b;
        ct = address >> (globalArgs.b + globalArgs.s);
        ++TIMESTAMP;
        unsigned ret = process(op, ct, ci, co);
        if (globalArgs.verbosity && ret != -1) {
            printf("%s %llx,%d", op, address, size);
            if (ret & 0b010U) printf(" miss");
            if (ret & 0b001U) printf(" eviction");
            if (ret & 0b100U) printf(" hit");
            printf("\n");
        }
    }
}

unsigned int process(const char *op, add_t ct, add_t ci, add_t co) {
    set_t set = CACHE.cache[ci];
    switch (op[0]) {
        case 'S':
        case 'L': {
            int lru = 0;
            ull minTimestamp = -1;
            int nv = -1;

            for (int i = 0; i < CACHE.lineNum; ++i) {
                if (set[i].timestamp < minTimestamp) {
                    minTimestamp = set[i].timestamp;
                    lru = i;
                }
                if (set[i].tag == ct && set[i].valid) {
                    ++HIT;
                    set[i].timestamp = TIMESTAMP;
                    return 0b100;
                }
                if (!set[i].valid) nv = i;
            }

            ++MISS;
            if (nv != -1) {
                set[nv].valid = 1;
                set[nv].tag = ct;
                set[nv].timestamp = TIMESTAMP;
                return 0b010;
            } else {
                ++EVICTION;
                set[lru].tag = ct;
                set[lru].timestamp = TIMESTAMP;
                return 0b011;
            }
        }
        case 'M': {
            unsigned int ret1 = process("L", ct, ci, co);
            unsigned int ret2 = process("S", ct, ci, co);
            return ret1 | ret2;
        }
        default:
            return -1;
    }
}

int main(int argc, char **argv) {
    getArgs(argc, argv);
    initCache();
    parse();
    printSummary(HIT, MISS, EVICTION);
    return 0;
}
