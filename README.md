[github-license-mit]: https://github.com/FedericoBruzzone/yapl/blob/main/LICENSE-MIT
[github-license-apache]: https://github.com/FedericoBruzzone/yapl/blob/main/LICENSE-APACHE

# yamalloc - Yet Another Memory Allocator

`yamalloc` is a cross-platform thread-safe memory allocator written in C.

## Features

`yamalloc` can be compiled with thread-safe support using the `YAMALLOC_THREAD_SAFE` definition (see `Makefile`).

`yamalloc` can be compiled specifying the memory allocation strategy flag (see `Makefile`):

- **Linked List** (using `YAMALLOC_LINKED_LIST` definition): The linked list is a list of blocks of memory. It is a singly linked list where each node contains a pointer to the next block of memory. In Linux it uses `sbrk` system call to request memory from the kernel, while in Windows it uses `NtAllocateVirtualMemory`.
The *Time complexity* to find a free block of memory is $O(n)$, where $n$ is the number of **blocks** in the list.

- **Free List LL** (using `YAMALLOC_FREE_LIST_LL` definition): The free list is a list of free blocks of memory. It is a singly linked list where each node contains a pointer to the next free block of memory. In Linux it uses `sbrk` system call to request memory from the kernel, while in Windows it uses `NtAllocateVirtualMemory`. The *Time complexity* to find a free block of memory is $O(n)$, where $n$ is the number of **free blocks** in the list.

- **Red-Black Tree** (using `YAMALLOC_FREE_LIST_RBT` definition): Future Work

## Example

```c
#include "yamalloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    char *str = (char *)yamalloc(14 * sizeof(char));
    if (str == NULL) {
        return 1;
    }
    strcpy(str, "Hello, World!\0");
    printf("%s\n", str);
    yafree(str);
    return 0;
}
```

## Compilation

Run tests:
```bash
$ make clean && make test
```
Compile the library and link it to the executable:
```bash
$ make clean && make && ./target/debug/src/cli/main
```
Compile the library as dynamic and link it to the executable:
```bash
$ export LD_LIBRARY_PATH=:./target/debug/src/lib/dynamic_lib/:$LD_LIBRARY_PATH
$ make clean && make comp_cli_dynamic && ./target/debug/src/cli/dynamic/main
```
Compile the library as static and link it to the executable:
```bash
$ make clean && make comp_cli_static && ./target/debug/src/cli/static/main
```

## License

This repository are licensed under either of

* Apache License, Version 2.0 ([LICENSE-APACHE][github-license-apache] or http://www.apache.org/licenses/LICENSE-2.0)

* MIT license ([LICENSE-MIT][github-license-mit] or http://opensource.org/licenses/MIT)

at your option.

Please review the license file provided in the repository for more information regarding the terms and conditions of the license.

## Contact

If you have any questions, suggestions, or feedback, do not hesitate to [contact me](https://federicobruzzone.github.io/).

