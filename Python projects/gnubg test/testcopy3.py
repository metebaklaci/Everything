import subprocess
import json
import re
import uuid
import time
import threading
import queue

GNUBG_CMD = "gnubg-cli.exe"


# -----------------------------
# GNUBG interface (debug version)
# -----------------------------
class GNUBG:
    def __init__(self):
        print("[INIT] starting GNUBG process")

        self.p = subprocess.Popen(
            [GNUBG_CMD, "--tty"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            text=True,
            bufsize=1
        )

        self.output_queue = queue.Queue()

        self.reader_thread = threading.Thread(
            target=self._stdout_reader,
            daemon=True
        )

        self.reader_thread.start()

        print("[INIT] GNUBG started")


    def _stdout_reader(self):
        """
        Runs forever and drains GNUBG output.
        Avoids readline blocking the main program.
        """
        for line in self.p.stdout:
            self.output_queue.put(line)


    def send(self, cmd):
        print(f"[SEND] {cmd}")

        self.p.stdin.write(cmd + "\n")
        self.p.stdin.flush()


    def read(self, timeout=0.5, tag=""):
        print(f"[READ] waiting {tag} ({timeout}s)")

        start = time.time()
        output = []

        while time.time() - start < timeout:

            try:
                line = self.output_queue.get(
                    timeout=0.05
                )
                output.append(line)

            except queue.Empty:
                pass


        result = "".join(output)

        print(
            f"[READ DONE] {tag} "
            f"({len(result)} chars)"
        )

        return result


# -----------------------------
# Setup
# -----------------------------
def set_cubeless(session):
    print("[STEP] set_cubeless")

    session.send("set evaluation cubeful off")
    session.send("set analysis cubeful off")
    session.send("set analysis plies 2")

    session.read(0.5, "set_cubeless")


# -----------------------------
# Helpers
# -----------------------------
def get_board(session):
    print("[STEP] get_board")

    session.send("show board")
    return session.read(0.5, "board")


def get_analysis(session):
    print("[STEP] get_analysis")

    session.send("eval")
    return session.read(0.8, "eval")


def parse_best_move(text):
    m = re.search(r"(Best move|Hint)\s*[:=]\s*(.+)", text, re.IGNORECASE)
    return m.group(2).strip() if m else None


def parse_equity(text):
    m = re.search(r"(cubeless equity|equity)\s*[:=]\s*([-\d\.]+)", text, re.IGNORECASE)
    return float(m.group(2)) if m else None


def extract_dice(board_text):
    m = re.search(r"Dice:\s*(\d)\s*(\d)", board_text)
    if not m:
        return None
    return [int(m.group(1)), int(m.group(2))]


# -----------------------------
# Main loop (debug version)
# -----------------------------
def run_single_game(out_file="game_dataset.jsonl"):
    print("[START] run_single_game")

    session = GNUBG()
    set_cubeless(session)

    session.send("new game")
    session.read(1.0, "new_game")

    game_id = uuid.uuid4().hex
    start_time = time.time()

    with open(out_file, "w") as f:
        move_index = 0

        while True:
            print("\n==============================")
            print(f"[LOOP] move_index={move_index}")
            print("==============================")

            # ---- BOARD ----
            print("[PHASE] board")
            board = get_board(session)

            # ---- ANALYSIS ----
            print("[PHASE] analysis")
            analysis = get_analysis(session)

            dice = extract_dice(board)
            best_move = parse_best_move(analysis)
            equity = parse_equity(analysis)

            print(f"[DATA] dice={dice} move={best_move} eq={equity}")

            # ---- WRITE ----
            record = {
                "move_index": move_index,
                "board": board.replace("\n", " | "),
                "dice": dice,
                "best_move": best_move,
                "cubeless_equity": equity
            }

            f.write(json.dumps(record) + "\n")
            f.flush()

            # ---- PLAY ----
            print("[PHASE] play")

            if best_move:
                session.send(f"play {best_move}")
            else:
                session.send("move")

            session.read(0.8, "play")

            # ---- GAME STATUS ----
            print("[PHASE] status")

            session.send("show game status")
            status = session.read(0.3, "status")

            # ---- PROGRESS ----
            if move_index % 5 == 0:
                elapsed = time.time() - start_time
                print(
                    f"[PROGRESS] game={game_id[:8]} "
                    f"ply={move_index} "
                    f"time={elapsed:.1f}s"
                )

            if "Game over" in status:
                print(f"[END] Game over at ply {move_index}")
                break

            move_index += 1

    print(f"[DONE] saved to {out_file}")


# -----------------------------
# entry
# -----------------------------
if __name__ == "__main__":
    run_single_game()