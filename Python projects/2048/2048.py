class Game2048:

    def __init__(self):
        self.board = [[0]*4 for _ in range(4)]

    def print(self):

        print()

        for row in self.board:
            print(" ".join(f"{x:5}" if x else "    ." for x in row))

        print()

    def get_board_flat(self):

        return [x for row in self.board for x in row]

    def add_tile(self, row, col, value):

        if self.board[row][col] != 0:
            raise ValueError("Square already occupied.")

        if value not in (2, 4):
            raise ValueError("Tile must be 2 or 4.")

        self.board[row][col] = value

    def compress(self, row):

        row = [x for x in row if x != 0]

        i = 0

        merged = []

        while i < len(row):

            if i + 1 < len(row) and row[i] == row[i+1]:
                merged.append(row[i] * 2)
                i += 2
            else:
                merged.append(row[i])
                i += 1

        merged += [0] * (4 - len(merged))

        return merged

    def move_left(self):

        moved = False

        new_board = []

        for row in self.board:

            compressed = self.compress(row)

            if compressed != row:
                moved = True

            new_board.append(compressed)

        self.board = new_board

        return moved

    def move_right(self):

        moved = False

        new_board = []

        for row in self.board:

            reversed_row = row[::-1]

            compressed = self.compress(reversed_row)

            compressed = compressed[::-1]

            if compressed != row:
                moved = True

            new_board.append(compressed)

        self.board = new_board

        return moved

    def transpose(self):

        self.board = [list(row) for row in zip(*self.board)]

    def move_up(self):

        self.transpose()

        moved = self.move_left()

        self.transpose()

        return moved

    def move_down(self):

        self.transpose()

        moved = self.move_right()

        self.transpose()

        return moved

    def move(self, action):

        if action == 0:          # Up
            return self.move_up()

        elif action == 1:        # Right
            return self.move_right()

        elif action == 2:        # Down
            return self.move_down()

        elif action == 3:        # Left
            return self.move_left()

        else:
            raise ValueError("Invalid action.")

    def game_over(self):

        # Empty square

        for row in self.board:
            if 0 in row:
                return False

        # Horizontal merge

        for r in range(4):
            for c in range(3):
                if self.board[r][c] == self.board[r][c+1]:
                    return False

        # Vertical merge

        for r in range(3):
            for c in range(4):
                if self.board[r][c] == self.board[r+1][c]:
                    return False

        return True
    

import torch

import torch
import torch.nn as nn
import torch.optim as optim

import random
import time

# -------------------------------------------------
# Load trained model
# -------------------------------------------------

class CNN2048(nn.Module):

    def __init__(self):
        super().__init__()

        self.features = nn.Sequential(

            nn.Conv2d(16, 256, kernel_size=3, padding=1),
            nn.BatchNorm2d(256),
            nn.ReLU(inplace=True),

            nn.Conv2d(256, 256, kernel_size=3, padding=1),
            nn.BatchNorm2d(256),
            nn.ReLU(inplace=True),

            nn.Conv2d(256, 256, kernel_size=3, padding=1),
            nn.BatchNorm2d(256),
            nn.ReLU(inplace=True),

            nn.Conv2d(256, 256, kernel_size=3, padding=1),
            nn.BatchNorm2d(256),
            nn.ReLU(inplace=True),
        )

        self.classifier = nn.Sequential(

            nn.Flatten(),

            nn.Linear(256 * 4 * 4, 256),
            nn.ReLU(inplace=True),

            nn.Linear(256, 4)
        )

    def forward(self, x):

        x = self.features(x)
        x = self.classifier(x)

        return x
    

TILES = [
    0,
    2,
    4,
    8,
    16,
    32,
    64,
    128,
    256,
    512,
    1024,
    2048,
    4096,
    8192,
    16384,
    32768,
]

tile_to_channel = {tile: i for i, tile in enumerate(TILES)}


def encode_board(board):

    board = torch.tensor(board)

    channels = torch.tensor(
        [tile_to_channel[t.item()] for t in board]
    )

    x = torch.zeros(16, 16)

    x[channels, torch.arange(16)] = 1

    return x.view(16,4,4)



device = torch.device(
    "cuda" if torch.cuda.is_available() else "cpu"
)

print(device)

if device.type == "cuda":
    print(torch.cuda.get_device_name(0))


model = CNN2048().to(device)

model.load_state_dict(
    torch.load(
        "cnn2048_best_first500_500000smpls_largernet.pth",
        map_location=device
    )
)

model.eval()

ACTION_NAMES = [
    "UP",
    "RIGHT",
    "DOWN",
    "LEFT"
]

def spawn_random_tile(game):

    empty_cells = []

    for r in range(4):
        for c in range(4):
            if game.board[r][c] == 0:
                empty_cells.append((r, c))

    if len(empty_cells) == 0:
        return False

    # Equal probability among empty cells
    r, c = random.choice(empty_cells)

    # 90% 2, 10% 4
    value = 2 if random.random() < 0.9 else 4

    game.board[r][c] = value

    return True


# -------------------------------------------------
# Initialize game
# -------------------------------------------------

game = Game2048()

spawn_random_tile(game)
spawn_random_tile(game)

move_count = 0

print("Initial board:")
game.print()


# -------------------------------------------------
# Autonomous play loop
# -------------------------------------------------

while True:

    input("Press ENTER for next move...")

    board = game.get_board_flat()

    x = encode_board(board).unsqueeze(0).to(device)

    with torch.no_grad():

        logits = model(x)

        probabilities = torch.softmax(logits, dim=1)[0]

    action = probabilities.argmax().item()

    print("--------------------------------")
    print(f"Move {move_count + 1}")
    print(f"Action: {ACTION_NAMES[action]}")

    print("\nMove probabilities:")

    for i, name in enumerate(ACTION_NAMES):

        print(
            f"{name:5}: {probabilities[i].item():.3f}"
        )

    moved = game.move(action)

    if not moved:

        print("\nWARNING: Network selected an illegal move.")

        # Optional: try the next-best legal move
        ranked_actions = torch.argsort(
            probabilities,
            descending=True
        )

        for alternative in ranked_actions:

            alternative = alternative.item()

            if game.move(alternative):
                print(
                    f"Using alternative move: "
                    f"{ACTION_NAMES[alternative]}"
                )
                break

        else:

            print("No legal moves available.")
            break


    spawn_random_tile(game)

    move_count += 1

    game.print()

    if game.game_over():

        print("GAME OVER")
        print(f"Moves played: {move_count}")

        break