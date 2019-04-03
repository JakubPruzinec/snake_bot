#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "bot.h"

/*
 * ./bot [-m]
 * 		-m			inject traps in memory
 */
int main(int argc, char *argv[], char *envp[])
{
	/* parse arguments */
	bool injTraps = (argc > 1 && strcmp(argv[1], "-m") == 0);
	const char *snakeBinary = (injTraps) ? "./bin/snakeOriginal" : "./bin/snakeModifiedForBot";

	pid_t pid = fork();

	if (pid == 0)
		runSnake(snakeBinary);
	else if (pid >= 0)
		runBot(pid, injTraps);
	else
	{
		fprintf(stderr, "FORK failed\n");
		exit(1);
	}

	return 0;
	(void)envp;
}

