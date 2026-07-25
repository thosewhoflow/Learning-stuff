# My malloc implementation

A simple allocator using `mmap` written in C


## Progress

### Done:
- allocate mutiple arenas as nedded
- search for empty space before making another arena
- aliging to 8 bytes

### Planned:
- make it so the arena size isn't hardcoded
- implement my_free()