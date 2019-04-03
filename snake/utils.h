#pragma once

/**
 * Writes error message and jumps to error label
 */
#define RETERR(expr, ...)	do {if ((expr)) {fprintf(stderr, __VA_ARGS__); goto error;}} while (0)