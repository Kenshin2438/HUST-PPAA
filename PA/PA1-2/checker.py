#!/usr/bin/python3

import subprocess
import sys
import re
import time
from typing import List

def check(command: List[str]) -> None:
    process = subprocess.Popen(
        command,
        stdout=subprocess.PIPE,
        text=True,
        shell=True
    )
    N: int = 5 if len(command) == 1 else int(command[1])

    pre: List[str] = ["none"] * N
    cnt: List[int] = [0] * N

    def valid_ouput(id: int, cur: str) -> bool:
        L: int = (id - 1 + N) % N
        R: int = (id + 1) % N

        match cur:
            case "eating":
                return pre[id] == "hungry" and pre[L] != "eating" and pre[R] != "eating"
            case "thinking":
                return pre[id] == "eating"
            case "hungry":
                return pre[id] == "thinking" or pre[id] == "none"
        return False # "invaild output"

    try:
        print("\033[s\033[?25l", end="")
        output_pattern = re.compile(r'^\s*(\w+)\s+is\s+now\s+(\w+)\.\s*$')
        assert process.stdout is not None
        for line in process.stdout:
            match = output_pattern.match(line)
            assert match is not None
            id, cur = match.groups()
            # print(f"Extracted: ID={id}, Status={cur}")

            print("\033[u", end="")
            for i, c in enumerate(cnt): print(f"Philosopher {i} dined {c:8} times.")

            if valid_ouput(int(id), cur):
                pre[int(id)] = cur
                cnt[int(id)] += int(cur == "eating")
            else:
                print("\033[93m")
                for i, s in enumerate(pre): print(f"Philosopher {i}: {s}")
                print(f"Next: Philosopher {id} change to {cur}")
                print("\033[0m")
                raise Exception("Program must be wrong.")
    except Exception as e:
        print(f"Error while reading output: {e}", file=sys.stderr)
    finally:
        print("\033[?25h", end="")
        if process.stdout is not None: process.stdout.close()
        process.kill()


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python checker.py <command>", file=sys.stderr)
        sys.exit(1)

    command = sys.argv[1:]
    check(command)

