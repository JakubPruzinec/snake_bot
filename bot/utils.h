#pragma once

#include <stdio.h>
#define RET_ERR(cond, ...) if (cond) {fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); goto error;}