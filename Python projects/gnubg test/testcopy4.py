import subprocess
import json
import re
import uuid
import time
import threading
import queue


GNUBG_CMD = "gnubg-cli.exe"


# -----------------------------
# GNUBG interface
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

        for line in self.p.stdout:
            self.output_queue.put(line)



    def send(self, cmd):

        print(f"[SEND] {cmd}")

        self.p.stdin.write(cmd + "\n")
        self.p.stdin.flush()



    def read(self, timeout=1.0, tag=""):

        print(
            f"[READ] {tag} ({timeout}s)"
        )

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
            f"[READ DONE] {tag}: "
            f"{len(result)} chars"
        )


        return result



    def query(self, cmd, timeout=1.0):

        self.send(cmd)

        return self.read(
            timeout,
            cmd
        )



# -----------------------------
# GNUBG setup
# -----------------------------
def set_cubeless(session):

    print("[STEP] set_cubeless")


    session.send(
        "set evaluation cubeful off"
    )

    session.send(
        "set analysis cubeful off"
    )

    session.send(
        "set analysis plies 2"
    )


    # no read here
    # configuration commands often produce no output



# -----------------------------
# Helpers
# -----------------------------
def get_board(session):

    print("[STEP] get_board")

    return session.query(
        "show board",
        0.8
    )



def get_analysis(session):

    print("[STEP] get_analysis")


    return session.query(
        "hint 0",
        2.0
    )



def parse_best_move(text):

    patterns = [

        r"Move\s*[:=]\s*(.+)",

        r"Best move\s*[:=]\s*(.+)",

        r"Hint\s*[:=]\s*(.+)"

    ]


    for p in patterns:

        m = re.search(
            p,
            text,
            re.IGNORECASE
        )

        if m:

            return m.group(1).strip()


    return None



def parse_equity(text):

    m = re.search(
        r"cubeless equity\s+(-?\d+\.\d+)",
        text,
        re.IGNORECASE
    )


    if m:

        return float(
            m.group(1)
        )


    return None



def extract_dice(board):

    patterns = [

        r"Dice:\s*(\d)\s*(\d)",

        r"Roll:\s*(\d)(\d)",

        r"Rolls:\s*(\d)\s*(\d)"

    ]


    for p in patterns:

        m = re.search(
            p,
            board,
            re.IGNORECASE
        )


        if m:

            return [
                int(m.group(1)),
                int(m.group(2))
            ]


    return None



# -----------------------------
# Generate one self-play game
# -----------------------------
def run_single_game(
    out_file="game_dataset.jsonl"
):

    print("[START] run_single_game")


    session = GNUBG()


    set_cubeless(session)


    session.send(
        "new game"
    )

    session.read(
        1.0,
        "new game"
    )


    game_id = uuid.uuid4().hex

    start_time = time.time()



    with open(out_file, "w") as f:


        move_index = 0



        while True:


            print("\n====================")

            print(
                f"[LOOP] ply {move_index}"
            )

            print("====================")



            # -----------------
            # Roll dice
            # -----------------

            session.send(
                "roll"
            )

            session.read(
                0.5,
                "roll"
            )



            # -----------------
            # Position
            # -----------------

            board = get_board(session)



            # -----------------
            # Analysis
            # -----------------

            analysis = get_analysis(session)



            dice = extract_dice(
                board
            )

            best_move = parse_best_move(
                analysis
            )

            equity = parse_equity(
                analysis
            )



            print(
                "[DATA]",
                "dice=", dice,
                "move=", best_move,
                "equity=", equity
            )



            if best_move is None:


                print(
                    "----- RAW HINT OUTPUT -----"
                )

                print(
                    analysis
                )

                print(
                    "---------------------------"
                )



            record = {

                "move_index":
                    move_index,

                "board":
                    board.replace(
                        "\n",
                        " | "
                    ),

                "dice":
                    dice,

                "best_move":
                    best_move,

                "cubeless_equity":
                    equity
            }



            f.write(
                json.dumps(record)
                + "\n"
            )

            f.flush()



            # -----------------
            # Play move
            # -----------------

            print("[PHASE] play")


            if best_move:


                session.send(
                    f"play {best_move}"
                )


            else:


                print(
                    "[WARN] no best move"
                )


                session.send(
                    "move"
                )



            # -----------------
            # Check game end
            # -----------------

            status = session.query(
                "show game status",
                0.5
            )


            if "Game over" in status:


                print(
                    f"[END] game finished at ply {move_index}"
                )

                break



            if move_index % 5 == 0:


                elapsed = time.time() - start_time


                print(
                    f"[PROGRESS] "
                    f"{game_id[:8]} "
                    f"ply={move_index} "
                    f"time={elapsed:.1f}s"
                )



            move_index += 1



    print(
        f"[DONE] saved {out_file}"
    )



# -----------------------------
# Entry
# -----------------------------
if __name__ == "__main__":

    run_single_game()