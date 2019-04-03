#define _GNU_SOURCE

#include <errno.h>
#include <sys/ptrace.h>
#include <sys/uio.h>

#include "mem_manager.h"
#include "utils.h"


bool getRegister(int64_t *res, pid_t pid, uint64_t reg)
{
	errno = 0;
	int64_t regVal = ptrace(PTRACE_PEEKUSER, pid, (void *)(reg * 8), NULL);
	if (errno != 0)
		return false;

	*res = regVal;
	return true;
}

bool setRegister(pid_t pid, uint64_t reg, int64_t val)
{
	return ptrace(PTRACE_POKEUSER, pid, (void *)(reg * 8), (void *)val) >= 0;
}

bool readMemory(uint8_t *dst, size_t nBytes, pid_t pid, void *addr)
{
	struct iovec local[1] = {{dst, nBytes}};
	struct iovec remote[1] = {{addr, nBytes}};
	ssize_t status =  process_vm_readv(pid, local, 1, remote, 1, 0);

	return (status < 0) ? false : ((size_t)status == nBytes);
}

bool writeMemory(uint8_t *src, size_t nBytes, pid_t pid, void *addr)
{
	struct iovec local[1] = {{src, nBytes}};
	struct iovec remote[1] = {{addr, nBytes}};
	ssize_t status = process_vm_writev(pid, local, 1, remote, 1, 0);

	return (status < 0) ? false : ((size_t)status == nBytes);
}

bool writeCodeMemory(uint8_t *src, size_t nBytes, pid_t pid, void *addr)
{
	size_t nWords = nBytes / 8;
	uint64_t *dstIter = (uint64_t *)addr;
	uint64_t *srcIter = (uint64_t *)src;
	for (size_t i = 0; i < nWords; i++, dstIter++, srcIter++)
		RET_ERR(ptrace(PTRACE_POKETEXT, pid, (void *)dstIter, (void *)srcIter) < 0,
				"PTRACE failed");

	size_t restBytes = nBytes % 8;
	if (!restBytes)
		return true;

	uint64_t origData = 0;
	RET_ERR(!readMemory((void *)&origData, sizeof(origData), pid, dstIter), "READMEMORY failed");

	for (size_t i = 0; i < restBytes; i++)
		((uint8_t *)&origData)[i] = ((uint8_t *)srcIter)[i];

	RET_ERR(ptrace(PTRACE_POKETEXT, pid, (void *)dstIter, (void *)(uint64_t)origData) < 0,
			"PTRACE failed");

	return true;
error:
	return false;
}