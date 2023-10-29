![RISC-V Logo](https://riscv.org/wp-content/uploads/2020/06/riscv-color.svg)

# tinyriscv

Compact, simple and easy-to-use RISCV emulator written in C. The core exists in a single-file-header tinyriscv.h following the STB library style. It is fast, portable, platform agnostic and self-contained (no external dependencies).

tinyriscv is designed to enable fast iterations and to empower programmers to create simulation environments and visualization / debug tools (as opossed to program compatibility for the average end-user). It favors simplicity and productivity toward this goal and lacks certain features commonly found in more high-level emulators.
tinyriscv is particulary suited to integration in simulation engines, embedded applications or any applications on platforms where operating system features are non-standard.
