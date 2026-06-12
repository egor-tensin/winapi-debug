#!/usr/bin/env bash

set -o errexit -o nounset -o pipefail
shopt -s inherit_errexit lastpipe

script_dir="$( dirname -- "${BASH_SOURCE[0]}" )"
script_dir="$( cd -- "$script_dir" && pwd )"

setup_clang_format_hook() {
	local gitdir
	gitdir="$( git rev-parse --git-dir )"
	local hooks_dir
	hooks_dir="$gitdir/hooks"

	local symlink_dest
	symlink_dest="$( realpath "--relative-to=$hooks_dir" -- "$script_dir/pre-commit.sh" )"

	ln -fs -- "$symlink_dest" "$hooks_dir/pre-commit"
}

setup_git_blame() {
	git config blame.ignoreRevsFile .git-blame-ignore-revs
}

main() {
	cd -- "$script_dir"
	setup_clang_format_hook
	setup_git_blame
}

main
