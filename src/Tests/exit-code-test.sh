#!/bin/sh
# End-to-end: todoos exits with 0 on success and 1 when the command prints an error,
# so that `todoos … && …` chains stop on failures. Runs the real binary in a throwaway HOME.
set -u
TODOOS="$1"
HOME=$(mktemp -d)
export HOME
trap 'rm -rf "$HOME"' EXIT
failed=0

expect() {
    expected="$1"
    shift
    "$TODOOS" "$@" >/dev/null 2>&1
    actual=$?
    if [ "$actual" != "$expected" ]; then
        echo "FAILED: todoos $* exited with $actual, expected $expected"
        failed=1
    fi
}

"$TODOOS" >/dev/null 2>&1 # first launch: installation

expect 0 add "a new item"
expect 0 show
expect 0 commands ad
expect 1 add
expect 1 add -p urgent
expect 1 edit zzzz "no such item"
expect 1 notacommand
expect 1 -l nosuchlist show

exit $failed
