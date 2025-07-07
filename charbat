#!/bin/bash

# Check if bat is installed
if ! command -v bat &>/dev/null; then
	echo "Error: 'bat' command not found."
	echo "Please install bat: https://github.com/sharkdp/bat"
	exit 1
fi

LANGUAGE="C"

# Put terminal in raw mode to capture character-by-character input
stty -echo -icanon time 0 min 1

# Reset terminal on exit
trap "stty sane; tput cnorm; echo; exit" INT TERM EXIT

input=""
previous=""

while true; do
	# Read one character
	char=$(dd bs=1 count=1 2>/dev/null)

	# Exit on Ctrl+D (EOF)
	if [[ -z "$char" ]]; then
		break
	fi

	if [[ $char == $'\x7f' ]]; then
		input="${input%?}" # Backspace
	else
		input+="$char"
	fi

	# Skip redraw if nothing changed
	if [[ "$input" == "$previous" ]]; then
		continue
	fi

	tput civis

	# Find common prefix length
	prefix_len=0
	while [[ "${input:$prefix_len:1}" == "${previous:$prefix_len:1}" && $prefix_len -lt ${#input} ]]; do
		((prefix_len++))
	done

	# Move cursor to point of change
	printf "\r"
	printf "%s" "${input:0:$prefix_len}"

	# Clear and redraw changed portion (highlighted)
	changed="${input:$prefix_len}"

	if [[ -n "$changed" ]]; then
		echo -n "$changed" | bat --language="$LANGUAGE" --paging=never --force-colorization
	fi

	# If previous was longer, erase trailing characters
	if ((${#previous} > ${#input})); then
		printf '%*s' $((${#previous} - ${#input})) '' # print spaces
		printf "\r"
		printf "%s" "$input"
	fi

	previous="$input"
	tput cnorm
done

stty sane
echo
