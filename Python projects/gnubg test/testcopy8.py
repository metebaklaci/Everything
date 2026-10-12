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

        print("[INIT] starting GNUBG")

        self.p = subprocess.Popen(
            [GNUBG_CMD, "--tty"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=1
        )

        self.q = queue.Queue()


        threading.Thread(
            target=self.reader,
            daemon=True
        ).start()


        time.sleep(1)


        self.send("show version")

        print(
            self.read(2)
        )


    def reader(self):

        for line in self.p.stdout:

            self.q.put(line)



    def send(self, cmd):

        print("[SEND]", cmd)

        self.p.stdin.write(
            cmd + "\n"
        )

        self.p.stdin.flush()



    def read(self, timeout=1):

        start = time.time()

        output=[]


        while time.time()-start < timeout:

            try:

                output.append(
                    self.q.get(timeout=0.05)
                )

            except queue.Empty:

                pass


        return "".join(output)



    def query(self, cmd, timeout=1):

        self.send(cmd)

        result = self.read(timeout)

        print(
            "[QUERY RESULT]",
            cmd,
            "=>",
            len(result),
            "chars"
        )

        return result



# -------------------------------------------------
# Parsing
# -------------------------------------------------

def parse_position(text):

    print("\n========== RAW POSITION ==========")
    print(text)
    print("==================================\n")


    pos_id=None
    dice=None


    m=re.search(
        r"Position ID:\s*(\S+)",
        text
    )

    if m:

        pos_id=m.group(1)



    d=re.search(
        r"Rolled\s+(\d)(\d)",
        text
    )


    if d:

        dice=[
            int(d.group(1)),
            int(d.group(2))
        ]


    print(
        "[PARSED POSITION]",
        "id=",
        pos_id,
        "dice=",
        dice
    )


    return pos_id,dice



def parse_hint(text):

    print("\n========== RAW HINT ==========")
    print(text)
    print("==============================\n")


    move=None
    equity=None



    # try normal move list

    patterns=[

        r"\d+\.\s+.*?ply\s+(.*?)\s+Eq\.",

        r"\d+\.\s+.*?\s{2,}(.+?)\s+Eq\.",

        r"Best move:\s*(.+)"

    ]


    for p in patterns:

        m=re.search(
            p,
            text,
            re.DOTALL
        )

        if m:

            move=m.group(1).strip()

            break



    e=re.search(
        r"Eq\.\s*:\s*([+-]?\d+\.\d+)",
        text
    )


    if e:

        equity=float(
            e.group(1)
        )


    print(
        "[PARSED HINT]",
        "move=",
        move,
        "equity=",
        equity
    )


    return move,equity



# -------------------------------------------------
# Game
# -------------------------------------------------

def run():


    print("[START]")


    g=GNUBG()



    print("[SETUP] cubeless")


    g.send(
        "set evaluation cubeful off"
    )

    g.send(
        "set analysis cubeful off"
    )

    g.send(
        "set hint cubeful off"
    )

    g.send(
        "set analysis plies 2"
    )


    time.sleep(1)



    g.send(
        "new game"
    )

    g.read(1)



    position_id = uuid.uuid4().hex



    with open(
        "dataset.jsonl",
        "w"
    ) as f:


        ply=0


        while True:


            print("\n======================")
            print(
                "PLY",
                ply
            )
            print("======================")



            # -------------------------
            # Position
            # -------------------------

            position = g.query(
                "show position",
                1
            )


            pos_id,dice = parse_position(
                position
            )



            # -------------------------
            # Hint
            # -------------------------

            hint = g.query(
                "hint",
                4
            )



            move,equity = parse_hint(
                hint
            )



            print(
                "[FINAL DATA]",
                {
                    "dice":dice,
                    "move":move,
                    "equity":equity
                }
            )



            record={

                "move_index":ply,

                "position_id":
                    pos_id,

                "dice":
                    dice,

                "position":
                    position,

                "best_move":
                    move,

                "cubeless_equity":
                    equity

            }


            f.write(
                json.dumps(record)
                + "\n"
            )

            f.flush()



            if move:


                g.send(
                    "move " + move
                )


            else:

                print(
                    "[STOP] no move parsed"
                )

                break



            status=g.query(
                "show game status",
                1
            )


            print(
                "[STATUS]"
            )

            print(status)



            if "game over" in status.lower():

                break



            ply += 1



if __name__=="__main__":

    run()