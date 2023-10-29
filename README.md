![RISC-V Logo](https://riscv.org/wp-content/uploads/2020/06/riscv-color.svg)

# tinyriscv

Compact, simple and easy-to-use RISCV emulator written in C. The core exists in a single-file-header [tinyriscv.h](https://github.com/inixyz/tinyriscv/blob/main/src/tinyriscv.h) following the [STB library](https://github.com/nothings/stb) style. It is fast, portable, platform agnostic and self-contained (no external dependencies).

tinyriscv is designed to enable fast iterations and to empower programmers to create simulation environments and visualization / debug tools (as opossed to program compatibility for the average end-user). It favors simplicity and productivity toward this goal and lacks certain features commonly found in more high-level emulators.

tinyriscv is particulary suited to integration in simulation engines, embedded applications or any applications on platforms where operating system features are non-standard.

## Building and running

```
git clone https://github.com/inixyz/tinyriscv/
cd tinyriscv
make
```

## Usage 

```
 ABI   Reg Hex      │ ABI   Reg Hex     
 ───────────────────┼───────────────────
 zero  x0  0        │ ra    x1  0        
 sp    x2  80001000 │ gp    x3  0        
 tp    x4  0        │ t0    x5  0        
 t1    x6  0        │ t2    x7  0        
 s0/fp x8  0        │ s1    x9  0        
 a0    x10 0        │ a1    x11 0        
 a2    x12 0        │ a3    x13 0        
 a4    x14 7        │ a5    x15 0        
 a6    x16 0        │ a7    x17 0        
 s2    x18 0        │ s3    x19 0        
 s4    x20 0        │ s5    x21 0        
 s6    x22 0        │ s7    x23 0        
 s8    x24 0        │ s9    x25 0        
 s10   x26 0        │ s11   x27 0        
 t3    x28 0        │ t4    x29 0        
 t5    x30 0        │ t6    x31 0        
 pc    x32 80000084 │
```
```
 Address  Memory                                          ASCII
─────────────────────────────────────────────────────────────────────────
 80000000 13 01 01 fe 23 2e 81 00 13 04 01 02 b7 07 00 80 ....#...........
 80000010 93 87 07 0f 23 26 f4 fe b7 67 6c 6c 93 87 87 54 ....#&...gll...T
 80000020 23 20 f4 fe b7 27 00 00 93 87 f7 16 23 12 f4 fe # ...'......#...
 80000030 23 03 04 fe 23 24 04 fe 6f 00 80 01 83 27 c4 fe #...#$..o....'..
 80000040 13 87 17 00 23 26 e4 fe 03 47 74 fe 23 80 e7 00 ....#&...Gt.#...
 80000050 83 27 84 fe 13 87 17 00 23 24 e4 fe 93 87 07 ff .'......#$......
 80000060 b3 87 87 00 83 c7 07 ff a3 03 f4 fe 83 47 74 fe .............Gt.
 80000070 e3 96 07 fc 93 07 00 00 13 85 07 00 03 24 c1 01 .............$..
 80000080 13 01 01 02 00 00 00 00 00 00 00 00 00 00 00 00 ................
 80000090 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
 800000a0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
 800000b0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
 800000c0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
 800000d0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
 800000e0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 ................
 800000f0 48 65 6c 6c 6f 21 00 00 00 00 00 00 00 00 00 00 Hello!..........
```
