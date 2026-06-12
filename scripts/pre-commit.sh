#!/usr/bin/env bash

set -o errexit -o nounset -o pipefail
shopt -s inherit_errexit lastpipe

if ! command -v git-clang-format &> /dev/null; then
	echo 'Please make sure git-clang-format is available.' >&2
	exit 1
fi

output="$( git clang-format --style file --diff )" || true

ok1='no modified files to format'
ok2='clang-format did not modify any files'

if [ "$output" != "$ok1" ] && [ "$output" != "$ok2" ]; then
	echo "$output"
	echo
	echo 'clang-format required some formatting fixes.'
	echo 'Please stage your changes, run `git clang-format`, review the formatting fixes, and commit.'
	exit 1
fi
