#pragma once

#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * Get register
 * @param res Result variable
 * @param pid PID of target process
 * @param reg Register type defined in <sys/reg.h>
 * @return Success flag
 */
bool getRegister(int64_t *res, pid_t pid, uint64_t reg);

/**
 * Set register
 * @param pid PID of target process
 * @param reg Register type defined in <sys/reg.h>
 * @param val Value to set
 * @return Success flag
 */
bool setRegister(pid_t pid, uint64_t reg, int64_t val);

/**
 * Read memory
 * @param dst Destination array
 * @param nBytes Number of bytes to read
 * @param pid PID of target process
 * @param addr Address to read data from
 * @return Success flag
 */
bool readMemory(uint8_t *dst, size_t nBytes, pid_t pid, void *addr);

/**
 * Write memory
 * @param src Source array
 * @param nBytes Number of bytes to write
 * @param pid PID of target process
 * @param addr Address to write data to
 * @return Success flag
 */
bool writeMemory(uint8_t *src, size_t nBytes, pid_t pid, void *addr);

/**
 * Write code memory
 * @param src Source array
 * @param nBytes Number of bytes to write
 * @param pid PID of target process
 * @param addr Address to write data to
 * @return Success flag
 */
bool writeCodeMemory(uint8_t *src, size_t nBytes, pid_t pid, void *addr);