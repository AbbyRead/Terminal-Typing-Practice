# TypeBelow

This program copies in some text, prints a line, lets you retype it, and does that until EOF.

## Features
- Accepts an optional starting line number to begin practicing from any line.
- Accepts negative starting line number to offset from the end of the source text.
- ~~Reports ending line number to let you easily start where you left off next time.~~  (to do)
- Padded bottom for comfort.  Typing at the very edge of the terminal is no fun.

## Anti-Features
- Does not halt your progress if you type something wrong.
- Does not track your typing speed or mistakes.
- Does not store any data (no profiles, saves, etc).

## Usage
```bash
typebelow [options] filename    # Source text from an existing file
typebelow [options] -           # Source from piping/redirection
typebelow [options]             # Source from OS copy/paste clipboard

Options:
  -s <positive_number>          # Start from the specified line number
  -s <negative_number>          # Start a number of lines from the end
  -h, --help                    # Show this help message and exit
  -v, --version                 # Show program version and exit

Examples:
  typebelow -s 10 myfile.txt   # Start at line 10 from file
  typebelow -s -3 -            # Start 3 lines from end, read from stdin
  typebelow                    # Read from clipboard, start at line 1
```

<br><br>

# Special Bonus: Syntax-Colorizing Shell Script Version (requires bat)
## Usage
```bash
cd colorized
./color_tb [options] filename    # Source text from an existing file
./color_tb [options]             # Source from clipboard or pipe/redirection
```
## Option
```bash
  -l <language>  # specify a programming language syntax
  or
  --language=<language>	 # to tell bat what language syntax to color it as
  # Tip: use "bat -L" to get a list of available languages

  Example usage:
  ./color_tb -l sh example_input.c  # to use bash syntax instead of C syntax
 ```
## Additional Feature
- Automatic dominant language syntax detection (requires jq and enry)
	- You can still override by specifying a language