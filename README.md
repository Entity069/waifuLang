# waifuLang

An esoteric language made especially for communicating with waifus.

## Introduction and Motivation

Waifus are intricate, complex to understand, and embody a blend of emotion, personality, and depth that makes them truly captivating. Similarly, waifuLang mirrors this complexity, combining playful syntax with logical precision to create a programming language that feels as much like an interaction as it does a technical tool. Here's a [small guide](https://www.youtube.com/watch?v=z1l4DDUCDOg) to understanding different types of waifus

By designing waifuLang, we celebrate the multi-dimensional aspects of waifus—adapting their traits into programming constructs that are fun, meaningful, and (completely) intuitive. Just as understanding a waifu requires patience and attention to detail, so does waifuLang.

## Instructions

The instructions in waifuLang are very similar to [brainfuck](https://esolangs.org/wiki/Brainfuck), a similar programming language.

The following instructions are similar to brainfuck.

- `yandere` - Move to the right of the current memory block (brainfuck equivalent of `>`)
- `tsundere` - Move to the left of the current memory block (brainfuck equivalent of `<`)
- `senpai` - Increment the value at the current memory block by one (brainfuck equivalent of `+`)
- `kohai` - Decrement the value at the current memory block by one (brainfuck equivalent of `-`)
- `hajimeru` - Start while loop (brainfuck equivalent of `[`)
- `yamete` - Stop loop (brainfuck equivalent of `]`)
- `sugoi` - Take input from `stdin` (brainfuck equivalent of `,`)
- `ara` - Print to `stdout` (brainfuck equivalent of `.`)

Besides the above the following has also been added natively.

- `arigato x` - Jump to the `x+1`-th memory block
- `hayai` - Add the value of the right memory block to the current block
- `hikari` - Subtract the value of the riht memory block from the current block
- `saiko` - Start `if-else_if-else` conditional block
- `saitei` - Similar to `else_if`
- `shimatta` - Similar to `else`
- `kudasai` - End `if-else_if-else` conditional block

Below are some logical operators translated to maintain homogeneity of waifuLang

- `hitoshi` - Equals to (`=`)
- `hitoshikunai` - Not equals to (`!=`)
- `ika` - Less than equals to (`<=`)
- `ijo` - Greater than equals to (`>=`)
- `oki` - Greater than (`>`)
- `miman` - Less than (`<`)

## Installation

To install and run waifuLang locally first clone the repository:

```bash
git clone https://github.com/Entity069/waifuLang.git
```
Compile the source:

```bash
gcc -Wall waifuLang.c -o waifuLang
```

Now you can use the `waifuLang` binary to run any compatible waifuLang program.

### Example Usage

```bash
./waifuLang hello.wfl
Hello World!
```

### License

The code is released under MIT License. See [LICENSE](./LICENSE).