import subprocess
import json
import re
import uuid
import time
import queue
import threading


GNUBG_CMD = r"C:\Program Files (x86)\gnubg\gnubg-cli.exe"


# -------------------------------------------------
# GNUBG wrapper
# -------------------------------------------------
class GNUBG:

    def __init__(self):

        print("[INIT] starting GNUBG process")

        self.p = subprocess.Popen(
            [GNUBG_CMD, "--tty"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=1
        )

        self.queue = queue.Queue()
        self.running = True

        self.thread = threading.Thread(
            target=self.reader,
            daemon=True
        )

        self.thread.start()

        print("[INIT] GNUBG started")

        time.sleep(1)

        self.send("show version")
        print(self.read(2))


    def reader(self):

        while self.running:

            line = self.p.stdout.readline()

            if line:
                self.queue.put(line)



    def send(self, cmd):

        self.p.stdin.write(cmd + "\n")
        self.p.stdin.flush()



    def read(self, timeout=1.0):

        start = time.time()
        output = []

        while time.time() - start < timeout:

            try:
                output.append(
                    self.queue.get(timeout=0.05)
                )

            except queue.Empty:
                pass


        return "".join(output)



    def query(self, cmd, timeout=1.0):

        self.send(cmd)

        return self.read(timeout)



# -------------------------------------------------
# GNUBG settings
# -------------------------------------------------
def set_cubeless(g):

    g.send("set evaluation cubeful off")
    g.send("set analysis cubeful off")
    g.send("set hint cubeful off")
    g.send("set analysis plies 2")



# -------------------------------------------------
# Data extraction
# -------------------------------------------------

def extract_dice(board_text):

    patterns = [

        r"Dice:\s*(\d)\s*(\d)",

        r"Roll:\s*(\d)\s*(\d)",

        r"rolls?\s*(\d)\s*(\d)"

    ]


    for p in patterns:

        m = re.search(
            p,
            board_text,
            re.IGNORECASE
        )

        if m:

            return [
                int(m.group(1)),
                int(m.group(2))
            ]


    return None



def parse_board(board_text):

    """
    Returns:

    [
      point1,
      point2,
      ...
      point24,
      white_bar,
      white_off,
      black_bar,
      black_off
    ]

    Positive = white
    Negative = black
    """

    board = [0] * 28


    # points
    for m in re.finditer(
        r"(\d+):\s*(-?\d+)",
        board_text
    ):

        point = int(m.group(1))

        count = int(m.group(2))


        if 1 <= point <= 24:

            board[point-1] = count



    # bars/off if present

    patterns = {

        24: r"White bar:\s*(\d+)",

        25: r"White borne off:\s*(\d+)",

        26: r"Black bar:\s*(\d+)",

        27: r"Black borne off:\s*(\d+)"

    }


    for idx, pat in patterns.items():

        m = re.search(
            pat,
            board_text,
            re.IGNORECASE
        )

        if m:

            board[idx] = int(m.group(1))


    return board



# -------------------------------------------------
# Hint parsing
# -------------------------------------------------

def parse_best_move(text):

    patterns = [

        r"Best move:\s*(.+)",

        r"1\.\s+.*?\s{2,}(.+?)\s+Eq",

        r"1\.\s+(.+?)\s+Eq"

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

    patterns = [

        r"Eq\.\s*:\s*([+-]?\d+\.\d+)",

        r"equity\s+([+-]?\d+\.\d+)"

    ]


    for p in patterns:

        m = re.search(
            p,
            text,
            re.IGNORECASE
        )

        if m:

            return float(
                m.group(1)
            )


    return None



# -------------------------------------------------
# Game generation
# -------------------------------------------------

def run(
    out_file="dataset.jsonl"
):

    print("[START]")


    g = GNUBG()


    set_cubeless(g)


    g.send("new game")

    g.read(1)


    position_id = uuid.uuid4().hex


    with open(out_file, "w") as f:


        move_index = 0


        while True:


            print()
            print("====================")
            print(
                f"[PLY] {move_index}"
            )
            print("====================")


            board_text = g.query(
                "show board",
                1
            )


            dice = extract_dice(
                board_text
            )


            board_state = parse_board(
                board_text
            )


            hint = g.query(
                "hint",
                3
            )


            best_move = parse_best_move(
                hint
            )


            equity = parse_equity(
                hint
            )


            print(
                "[DATA]",
                dice,
                best_move,
                equity
            )


            record = {

                "move_index":
                    move_index,

                "position_id":
                    position_id,

                "dice":
                    dice,

                "board_state":
                    board_state,

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



            if best_move:

                g.send(
                    f"move {best_move}"
                )

            else:

                g.send(
                    "move"
                )


            status = g.query(
                "show game status",
                1
            )


            if "game over" in status.lower():

                print("[END]")
                break


            move_index += 1



if __name__ == "__main__":

    run()