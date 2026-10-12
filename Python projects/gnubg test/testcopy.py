import re
import random
import subprocess

GNUBG = r"C:\Program Files (x86)\gnubg\gnubg-cli.exe"

CUBE_ACTIONS = {"double", "take", "pass", "drop", "beaver", "redouble"}

pattern = r"\d+\)\s+(\d+):\s+(.+?)\s+(\d+):\s+(.+)"


def clean_move(m):
    return m.replace("(2)", "").strip()


def is_cube(m):
    return m.lower() in CUBE_ACTIONS


def is_valid_move_format(move):
    return bool(re.match(r"^[0-9/ *\-]+$", move))


def extract_moves(log):
    moves = []

    for line in log.splitlines():
        m = re.search(pattern, line)
        if not m:
            continue

        dice1, p1, dice2, p2 = m.group(1), m.group(2), m.group(3), m.group(4)

        p1 = clean_move(p1)
        p2 = clean_move(p2)

        if not is_cube(p1) and is_valid_move_format(p1):
            moves.append((dice1, p1))

        if not is_cube(p2) and is_valid_move_format(p2):
            moves.append((dice2, p2))

    return moves


def build_commands(moves, stop):
    cmds = [
        "new game",
        "set cube use off",
        "set automatic game off"
    ]

    for i in range(stop):
        dice, move = moves[i]

        print(f"Applying move {i}: {dice} {move}")

        cmds.append(f"set dice {dice}")
        cmds.append(f"move {move}")

    cmds += [
        "show board",
        "eval",
        "hint 3",
        "quit"
    ]

    return "\n".join(cmds) + "\n"


def run_gnubg(commands):
    result = subprocess.run(
        [GNUBG, "--tty"],
        input=commands,
        capture_output=True,
        text=True,
        timeout=60
    )
    return result.stdout, result.stderr


def extract_error_context(output):
    lines = output.splitlines()

    for i, line in enumerate(lines):
        if "Illegal or unparsable move" in line:
            start = max(0, i - 8)
            end = i + 8
            return "\n".join(lines[start:end])

    return None


# -----------------------------
# Test input
# -----------------------------
log = """
1) 35: 8/3 6/3                 43: 24/20 24/21
2) 61: 13/7 7/6                26: 21/15 15/13
3) 33: 8/5* 8/5 24/21 24/21    54: 25/21 13/8
"""

moves = extract_moves(log)

if len(moves) < 3:
    raise RuntimeError("Not enough moves parsed")

stop = random.randint(2, len(moves) - 1)

print("\nStopping at move index:", stop)

commands = build_commands(moves, stop)

output, errors = run_gnubg(commands)

print("\n=== GNUBG OUTPUT ===\n")
print(output)

if errors:
    print("\n=== GNUBG ERRORS ===\n")
    print(errors)

    context = extract_error_context(errors)
    if context:
        print("\n=== ERROR CONTEXT ===\n")
        print(context)