# Snake bot
To learn SDL2 basics I wrote a simple snake game. I wasn't satisfied so I wrote a bot playing the game by altering process memory.  
```
./bot [-m]
    -m         Use original binary over instrumented version and inject traps in runtime
```
`/bot/bin` contains instrumented snake binary (int3 patched at 0x00002AF4)
