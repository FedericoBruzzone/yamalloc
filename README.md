[github-license-mit]: https://github.com/FedericoBruzzone/yapl/blob/main/LICENSE-MIT
[github-license-apache]: https://github.com/FedericoBruzzone/yapl/blob/main/LICENSE-APACHE

# yamalloc - Yet Another Memory Allocator

`yamalloc` is a cross-platform thread-safe memory allocator written in C.

In Linux it uses `brk` system call to request memory from the kernel, while in Windows it uses `NtAllocateVirtualMemory`.

## Usage

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

- Email:
  - [federico.bruzzone.i@gmail.com]
  - [federico.bruzzone@studenti.unimi.it]
- GitHub:
  - [FedericoBruzzone](https://github.com/FedericoBruzzone)
