import re
import copy

# -----------------------------
# Board
# -----------------------------
def init_state():
    return {
        "board": [
            -2, 0, 0, 0, 0, 5,
             0, 3, 0, 0, 0, 5,
             5, 0, 0, 0, -3, 0,
            -5, 0, 0, 0, 0, 2
        ],
        "bar_X": 0,
        "bar_O": 0,
        "off_X": 0,
        "off_O": 0
    }


def opponent(p):
    return "O" if p == "X" else "X"


# -----------------------------
# Parsing
# -----------------------------
def is_cube_action(t):
    return t.lower() in {"double", "take", "pass", "drop", "beaver", "redouble"}


def clean(t):
    return t.replace("(2)", "").strip()


def parse_move(token):
    token = token.replace("*", "")
    if "/" not in token:
        return None

    a, b = token.split("/")
    if not a.isdigit() or not b.isdigit():
        return None

    return int(a), int(b)


def to_index(player, point):
    if player == "X":
        return 24 - point
    return point - 1


# -----------------------------
# Apply move
# -----------------------------
def apply_move(state, player, move_str):
    st = copy.deepcopy(state)
    board = st["board"]
    opp = opponent(player)

    for token in move_str.split():
        parsed = parse_move(token)
        if not parsed:
            continue

        src_p, dst_p = parsed
        src = to_index(player, src_p)
        dst = to_index(player, dst_p)

        # BAR handling
        bar_key = f"bar_{player}"
        if st[bar_key] > 0:
            # must enter from bar first (ignored here if not relevant)
            continue

        # remove from source
        if player == "X":
            if board[src] <= 0:
                continue
        else:
            if board[src] >= 0:
                continue

        # move checker out
        if player == "X":
            board[src] -= 1
        else:
            board[src] += 1

        # hit logic
        if player == "X":
            if board[dst] == -1:
                st["bar_O"] += 1
                board[dst] = 0
            board[dst] += 1
        else:
            if board[dst] == 1:
                st["bar_X"] += 1
                board[dst] = 0
            board[dst] -= 1

    return st


# -----------------------------
# Game reconstruction
# -----------------------------
def reconstruct_game(moves):
    state = init_state()
    history = []

    player = "X"

    for i, (dice, move) in enumerate(moves):
        if is_cube_action(move):
            continue

        state = apply_move(state, player, move)

        history.append({
            "ply": i,
            "player": player,
            "dice": dice,
            "move": move,
            "state": copy.deepcopy(state)
        })

        player = opponent(player)

    return history


# -----------------------------
# Parser
# -----------------------------
def extract_moves(log):
    pattern = r"\d+\)\s+(\d+):\s+(.+?)\s+(\d+):\s+(.+)"
    moves = []

    for line in log.splitlines():
        m = re.search(pattern, line)
        if not m:
            continue

        d1, m1, d2, m2 = m.groups()
        m1, m2 = clean(m1), clean(m2)

        if not is_cube_action(m1):
            moves.append((d1, m1))
        if not is_cube_action(m2):
            moves.append((d2, m2))

    return moves


# -----------------------------
# Test
# -----------------------------
if __name__ == "__main__":
    log = """
    1) 35: 8/3 6/3                 43: 24/20 24/21
    2) 61: 13/7 7/6                26: 21/15 15/13
    3) 33: 8/5* 8/5 24/21 24/21    54: 25/21 13/8
    """

    moves = extract_moves(log)
    hist = reconstruct_game(moves)

    for h in hist:
        print("\nPLY:", h["ply"])
        print("PLAYER:", h["player"])
        print("MOVE:", h["move"])
        print("BOARD:", h["state"]["board"])
        print("BAR X/O:", h["state"]["bar_X"], h["state"]["bar_O"])