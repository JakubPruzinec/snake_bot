#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/reg.h>
#include <sys/personality.h>

#include "utils.h"
#include "bot.h"
#include "mem_manager.h"

#define GAME_WIDTH	20
#define GAME_HEIGHT	15

#define INST_TRAP	0xCC
#define INST_NOP	0x90

#define XOR_INST_ADDR	0x555555556af4

/* snake direction */
typedef enum
{
	UP = 0,
	DO = 1,
	LE = 2,
	RI = 3
} dir_t;

struct coords
{
	uint64_t x;
	uint64_t y;
} __attribute__((packed));

struct game
{
	uint32_t snakeDir;
	uint32_t declSnakeDir;
	uint64_t listHead;
	uint64_t listTail;
	uint64_t snakeLen;
	struct coords food;
	uint64_t gameWidth;
	uint64_t gameHeight;
	struct coords snakeHead;		// additional
} __attribute__((packed));


static dir_t moveMap[GAME_HEIGHT][GAME_WIDTH] =
{
	{DO, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE, LE},
	{DO, RI, DO, RI, DO, RI, DO, RI, DO, RI, DO, RI, DO, RI, DO, RI, DO, RI, DO, UP},
	{DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP},
	{DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP},
	{DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP},
	{DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP},
	{DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP},
	{DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP},
	{DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP},
	{DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP},
	{DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP},
	{DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP},
	{DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP},
	{DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP, DO, UP},
	{RI, UP, RI, UP, RI, UP, RI, UP, RI, UP, RI, UP, RI, UP, RI, UP, RI, UP, RI, UP}
};

/**
 * Moves snake in certain direction
 * runBot helper function
 * @param gameStruct Structure with game variables
 * @param childPid PID of child process
 * @param gameClassAddr Address of original game struct in child process memory space
 * @return Success flag
 */
static bool moveSnake(struct game *gameStruct, pid_t childPid, void *gameClassAddr);


void runBot(pid_t childPid, bool injectTrapsInMemory)
{
	int status = 0;
	
	/* wait for execv interrupt */
	RET_ERR(waitpid(childPid, &status, 0) < 0, "WAITPID failed");
	RET_ERR(!WIFSTOPPED(status) || WSTOPSIG(status) != SIGTRAP, "Child stopped for unknown reason");

	/* release child */
	RET_ERR(ptrace(PTRACE_CONT, childPid, NULL, NULL) < 0, "PTRACE_CONT failed");

	if (injectTrapsInMemory)
	{
		usleep(200); // assume game execv is done by now
		RET_ERR(kill(childPid, SIGUSR1) != 0, "Failed to stop child for injection");
		RET_ERR(waitpid(childPid, &status, 0) < 0, "WAITPID failed");
		RET_ERR(!WIFSTOPPED(status) || WSTOPSIG(status) != SIGUSR1, "Child stopped for unknown reason");

		uint8_t patchInstructions[] = {INST_TRAP, INST_NOP};
		RET_ERR(!writeCodeMemory(patchInstructions, sizeof(patchInstructions), childPid, (void *)XOR_INST_ADDR),
				"Failed to write to memory while injecting instructions");

		RET_ERR(ptrace(PTRACE_CONT, childPid, NULL, NULL) < 0, "PTRACE_CONT failed");
	}

	while (1)
	{
		int64_t gameClassAddr = 0;
		struct game gameStruct;
		/* wait for Game::Tick int3 */
		RET_ERR(waitpid(childPid, &status, 0) < 0, "WAITPID failed");
		RET_ERR(!WIFSTOPPED(status) || WSTOPSIG(status) != SIGTRAP, "Child stopped for unknown reason");
		/* well, there was xor eax, eax... */
		RET_ERR(!setRegister(childPid, RAX, 0x0), "setRegister failed");
		/* this * is passed through rdi */
		RET_ERR(!getRegister(&gameClassAddr, childPid, RDI), "getRegister failed");
		/* read Game variables, snakeHead coords do NOT follow gameHeightY, so don't parse them */
		RET_ERR(!readMemory((uint8_t *)&gameStruct, sizeof(gameStruct) - sizeof(gameStruct.snakeHead),
				childPid, (void *)gameClassAddr), "readMemory failed");
		/* listTail points to list node with snakeHead; its coordinates lay at offset 16 */
		RET_ERR(!readMemory((uint8_t *)&gameStruct.snakeHead, sizeof(gameStruct.snakeHead),
				childPid, (void *)(gameStruct.listTail + 0x10)), "readMemory failed");

		printf("snakeDir: %u, declSnakeDir: %u, snakeLen: %lu, food.x: %lu, food.y: %lu, "
				"gameWidth: %lu, gameHeight: %lu, snakeHead.x: %lu, snakeHead.y: %lu\n",
				gameStruct.snakeDir, gameStruct.declSnakeDir, gameStruct.snakeLen, gameStruct.food.x,
				gameStruct.food.y, gameStruct.gameWidth, gameStruct.gameHeight,
				gameStruct.snakeHead.x, gameStruct.snakeHead.y);

		RET_ERR(!moveSnake(&gameStruct, childPid, (void *)gameClassAddr), "moveSnake failed");

		/* release child */
		RET_ERR(ptrace(PTRACE_CONT, childPid, NULL, NULL) < 0, "PTRACE_CONT failed");
	}

error:
	kill(childPid, SIGKILL);
	exit(1);
}

void runSnake(const char *file)
{

	char *args[] = {NULL};
	RET_ERR(personality(ADDR_NO_RANDOMIZE) < 0, "Failed to turn of ASLR");
	RET_ERR(ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0, "TRACE_ME failed");
	RET_ERR(execv(file, args) < 0, "EXECV failed");

error:
	exit(1);
}

bool moveSnake(struct game *gameStruct, pid_t childPid, void *gameClassAddr)
{
	gameStruct->declSnakeDir = moveMap[gameStruct->snakeHead.y][gameStruct->snakeHead.x];
	RET_ERR(!writeMemory((uint8_t *)gameStruct, sizeof(*gameStruct), childPid, gameClassAddr),
			"writeMemory failed");

	return true;
error:
	return false;
}