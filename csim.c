// name: 兰鸥羽 Ouyu Lan
// loginID: 5140309001

#include "cachelab.h"
#include <getopt.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <stdio.h>
#include <math.h>

#define MAXFILEBUF 1024
#define MACHINEBIT 64

typedef struct 
{
	unsigned long long tag;
	int valid;
	int cycle;
} Line;

typedef struct 
{
	Line *lines;
} Set;

typedef struct 
{
	int setBitNum, assocNum, bloBitNum;
	int hitNum, missNum, evictNum;
	Set *sets;
} Cache;

// initial a cache
Cache *cache_init(int setBitNum, int assocNum, int bloBitNum) {
	Cache *cache;
	Line *line;

	// allocate the space
	cache = (Cache*)malloc(sizeof(Cache));
	cache->sets = (Set*)malloc((1 << setBitNum) * sizeof(Set));
	int i = 0;
	for (i = 0; i < (1 << setBitNum); ++i) {
		cache->sets[i].lines = (Line*)malloc((assocNum) * sizeof(Line));
		// initial the attribute in one line
		for(int j = 0;j<assocNum;j++) {
			line = &cache->sets[i].lines[j];
			line->cycle = 0;
			line->valid = 0;
			line->tag = 0;
		}
	}

	// initial the attributes
	cache->setBitNum = setBitNum;
	cache->assocNum = assocNum;
	cache->bloBitNum = bloBitNum;
	cache->hitNum = 0;
	cache->missNum = 0;
	cache->evictNum = 0;

	return cache;
}

// destroy the cache
void cache_destroy(Cache *cache) {
	int i = 0;
	for (i = 0; i < (1 << cache->setBitNum); ++i) {
		free(cache->sets[i].lines);
	}
	free(cache->sets);
	free(cache);
}

// update the cycles in the set
void set_cycle(Cache *cache, Set *set, Line *line) {
	line->cycle = 0;
	int i = 0;
	for (i = 0; i < cache->assocNum; ++i) {
		set->lines[i].cycle += 1;
	}
}

// deal with when cache miss happens
void cache_miss(Cache *cache, Set *set, unsigned long long tag, int vFlag) {
	cache->missNum += 1;
	if (vFlag) printf(" miss");

	Line *line;
	int i = 0, ifFound = 0, maxCycle = 0, maxIndex = 0;

	// search in the whole set
	for (i = 0; i < cache->assocNum; ++i) {
		line = &set->lines[i];
		// find a empty line
		if (line->valid == 0) { 
			line->valid = 1;
			line->tag = tag;
			set_cycle(cache, set, line);
			ifFound = 1;
			break;
		}
		// to find the least recent used one
		if (line->cycle > maxCycle) {
			maxCycle = line->cycle;
			maxIndex = i;
		}
	}

	// replace the least recent used one
	if (!ifFound) {
		cache->evictNum += 1;
		if (vFlag) 
			printf(" eviction");
		line = &set->lines[maxIndex];
		line->valid = 1;
		line->tag = tag;
		set_cycle(cache, set, line);
	}
}

// load/store/modify date in the cache
void cache_op(Cache *cache, unsigned long long address, int vFlag) {
	// extract the tag for the line from the address
	int tag = address >> cache->bloBitNum;
	int index = tag & ((1 << cache->setBitNum) - 1);
	tag = tag >> cache->setBitNum;

	// search for the address
	Set *set;
	Line *line;
	set = &cache->sets[index];
	int i = 0, ifFound = 0;
	for (i = 0; i < cache->assocNum; ++i) {
		line = &set->lines[i];
		// hit
		if (line->valid && line->tag == tag) {
			ifFound = 1;
			set_cycle(cache, set, line);
			cache->hitNum += 1;
			if (vFlag) printf(" hit");
			break;
		}
	}
	// miss
	if (!ifFound) {
		cache_miss(cache, set, tag, vFlag);
	}
}

int main(int argc, char *argv[])
{
	int opt;
	int setBitNum = 0, assocNum = 0, bloBitNum = 0, vFlag = 0;
	char *traceFile = NULL;

	// check the inputs
	while ((opt = getopt(argc, argv, "vs:E:b:t:")) != -1) {
		switch (opt) {
			case 'v':
				vFlag = 1;
				break;
			case 's':
				setBitNum = atoi(optarg);
				break;
			case 'E':
				assocNum = atoi(optarg);
				break;
			case 'b':
				bloBitNum = atoi(optarg);
				break;
			case 't':
				traceFile = optarg;
				break;
			default:
				fprintf(stderr, "Usage: %s [-hv] -s <s> -E <E> -b <b> -t <traceFile>\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}
	if (!(setBitNum && assocNum && bloBitNum && traceFile)) {
		fprintf(stderr, "Lack argument!\n");
		exit(EXIT_FAILURE);
	}
	if (optind > argc) {
		fprintf(stderr, "Redundant argument!\n");
		exit(EXIT_FAILURE);
	}

	// initial a cache according to the inputs
	Cache *cache = cache_init(setBitNum, assocNum, bloBitNum);

	// read the trace file
	FILE *tfile;
	char operation;
	unsigned long long address;
	int size;
	if ((tfile = fopen(traceFile, "r")) == NULL) {
		fprintf(stderr, "Cannot open the file!\n");
		exit(EXIT_FAILURE);
	}

	while (!feof(tfile)) {
		while(fscanf(tfile, " %c %llx,%d", &operation, &address, &size) == 3) {
			if (vFlag) 
				printf("%c %llx,%d", operation, address, size);
			switch (operation) {
				case 'L':
					cache_op(cache, address, vFlag);
					break;
				case 'S':
					cache_op(cache, address, vFlag);
					break;
				case 'M':
					cache_op(cache, address, vFlag);
					cache_op(cache, address, vFlag);
					break;
				default:
					break;
			}
			if (vFlag) printf("\n");
		}
	}

    printSummary(cache->hitNum, cache->missNum, cache->evictNum);

    fclose(tfile);
    cache_destroy(cache);
    return 0;
}
