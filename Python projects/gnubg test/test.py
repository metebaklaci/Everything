import re
import random
import subprocess

GNUBG = r"C:\Program Files (x86)\gnubg\gnubg-cli.exe"


# -----------------------------
# Parse Arkadium log
# -----------------------------
pattern = r"\d+\)\s+(\d+):\s+(.+?)\s+(\d+):\s+(.+)"


def extract_moves(log):
    moves = []
    for line in log.splitlines():
        m = re.search(pattern, line)
        if m:
            moves.append((m.group(1), m.group(2).strip()))
            moves.append((m.group(3), m.group(4).strip()))
    return moves


# -----------------------------
# VERY IMPORTANT:
# Instead of full board simulation,
# we rely on GNUBG replay up to a point,
# then evaluate snapshot.
# -----------------------------
def build_gnubg_script(moves, stop):
    cmds = [
        "new game",
        "set cube use off"
    ]

    for i in range(stop):
        dice, move = moves[i]

        cmds.append(f"set dice {dice}")
        cmds.append(f"move {move}")

    cmds += [
        "show board",
        "eval",
        "hint 3",
        "quit"
    ]

    return "\n".join(cmds) + "\n"


def run(cmds):
    result = subprocess.run(
        [GNUBG, "--tty"],
        input=cmds,
        text=True,
        capture_output=True,
        timeout=60
    )
    return result.stdout, result.stderr


# -----------------------------
# Example log (replace with dataset sample later)
# -----------------------------
log = """
1) 35: 8/3 6/3                 43: 24/20 24/21
2) 61: 13/7 7/6                26: 21/15 15/13
3) 33: 8/5* 8/5 24/21 24/21    54: 25/21 13/8
"""


moves = extract_moves(log)

if len(moves) < 3:
    raise RuntimeError("Not enough moves parsed")

# pick a mid-game position
stop = random.randint(2, len(moves) - 1)

cmds = build_gnubg_script(moves, stop)

out, err = run(cmds)

print("\n=== GNUBG OUTPUT ===\n")
print(out)

if err:
    print("\n=== ERRORS ===\n")
    print(err)