# Notes

## Instructions

Instructions are not simply functions. It's better to view them as data.

```c++
struct Instruction {
    const char *name;
    int cycles;
    void (*execute)();
};
```

Something useful to know is pointer-to-member templates. This grants the ability
to switch registers at compile time without the need for switches or branching.

Given by the syntax `template<T MyClass::*MyClassMember>`

Here's a practical example

```c++
template<uint16_t CPU::*Dest, uint16_t CPU::*Src>
void ld_r16_r16(CPU& cpu) {
    cpu.*Dest = cpu.*Src;
}
```

Assign it like so

```c++
std::array<CPU::*, 3> reg16 = { &BC, &DE, &HL };
std::array<Instruction, 256> instruction_set{};
opcode = 0x20; // Not the actual op
// Note: BC and DE can be determined more programmatically than this
instruction_set[opcode] = {"LD BC DE", 2, ld_r16_r16<reg16[0], reg16[1]>};
```