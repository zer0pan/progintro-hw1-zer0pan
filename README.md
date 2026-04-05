# Soundwave: WAV File Manipulation Utility

A command-line utility written in C for analyzing and modifying uncompressed PCM WAV audio files. The program processes files through standard input (`stdin`) and standard output (`stdout`).

## Features

The program currently supports three primary operations:
* Extracting and validating WAV header metadata.
* Modifying the playback sample rate.
* Extracting a single channel (left or right) from a stereo audio file.

*Note: The `--help` menu mentions a `volume` option, but it is not implemented in the current source code.*

## Compilation

To compile the program, use a standard C compiler such as `gcc`:

```bash
gcc -o soundwave main.c -lm
```

# Usage

The program expects the input file to be passed via standard input (<) and writes the modified audio file to standard output (>).

General Syntax:
```bash
./soundwave [option] [arguments] < input.wav > output.wav
```

# Options
## 1. Display Help

Displays the available commands and basic usage examples.
```bash
./soundwave --help
```

## 2. Info

Reads and validates the WAV file header, printing its structural metadata to the console. It does not output a new audio file.
```bash 
./soundwave info < input.wav
```
Output includes: File size, format chunk size, WAVE type format, mono/stereo indicator, sample rate, bytes/sec, block alignment, bits/sample, and data chunk size.

## 3. Rate

Modifies the sample rate of the audio file by a specified floating-point multiplier. Values greater than 1.0 speed up the audio, while values less than 1.0 slow it down.
```bash 
./soundwave rate <factor> < input.wav > output.wav
```
Example (Increase sample rate by 10%):
```bash
./soundwave rate 1.10 < original.wav > modified.wav
```

# 4. Channel

Extracts either the left or right channel from a stereo WAV file and outputs a new mono WAV file.
```bash
./soundwave channel <left|right> < input_stereo.wav > output_mono.wav
```
Example (Extract left channel):
```bash
./soundwave channel left < stereo.wav > mono_left.wav
```
