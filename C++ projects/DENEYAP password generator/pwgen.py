import secrets
import string

# -----------------------------
# Character sets
# -----------------------------

LOWERCASE = "abcçdefgğhıijklmnoöprsştuüvyz"
UPPERCASE = "ABCÇDEFGĞHIİJKLMNOÖPRSŞTUÜVYZ"
DIGITS = string.digits
SPECIAL = r"""!@#$%^&*()-_=+[]{}|;:,.<>?/"""

CHARSETS = {
    "L": LOWERCASE,
    "U": UPPERCASE,
    "D": DIGITS,
    "S": SPECIAL,
}

MIN_LENGTH = 15


# -----------------------------
# Sequence detection
# -----------------------------

def creates_sequence(password, new_char):
    """
    Checks whether appending new_char creates
    an increasing/decreasing sequence of length 3.
    """

    if len(password) < 2:
        return False

    chars = password[-2:] + new_char

    # Turkish alphabet
    alphabet = LOWERCASE

    a, b, c = chars

    # Letter sequences
    if (
        a.lower() in alphabet
        and b.lower() in alphabet
        and c.lower() in alphabet
    ):
        ia = alphabet.index(a.lower())
        ib = alphabet.index(b.lower())
        ic = alphabet.index(c.lower())

        if ib == ia + 1 and ic == ib + 1:
            return True

        if ib == ia - 1 and ic == ib - 1:
            return True

    # Numeric sequences
    if a.isdigit() and b.isdigit() and c.isdigit():

        x = int(a)
        y = int(b)
        z = int(c)

        if y == x + 1 and z == y + 1:
            return True

        if y == x - 1 and z == y - 1:
            return True

    return False


# -----------------------------
# Password generation
# -----------------------------

def generate_password(length):

    if length < MIN_LENGTH:
        raise ValueError(
            f"Password must be at least {MIN_LENGTH} characters."
        )

    # Target count of each category
    base = length // 4
    remainder = length % 4

    counts = {
        "L": base,
        "U": base,
        "D": base,
        "S": base,
    }

    # Randomly distribute the remaining characters
    categories = list(counts.keys())

    for c in secrets.SystemRandom().sample(categories, remainder):
        counts[c] += 1

    used = {
        "L": 0,
        "U": 0,
        "D": 0,
        "S": 0,
    }

    password = ""

    while len(password) < length:

        # Remaining need for each category
        remaining = {
            c: counts[c] - used[c]
            for c in categories
            if counts[c] > used[c]
        }

        # Relative deficit
        max_ratio = max(
            remaining[c] / counts[c]
            for c in remaining
        )

        # Categories most behind their target
        candidates = [
            c
            for c in remaining
            if remaining[c] / counts[c] == max_ratio
        ]

        secrets.SystemRandom().shuffle(candidates)

        placed = False

        for category in candidates:

            charset = list(CHARSETS[category])
            secrets.SystemRandom().shuffle(charset)

            for ch in charset:

                if not creates_sequence(password, ch):
                    password += ch
                    used[category] += 1
                    placed = True
                    break

            if placed:
                break

        # Extremely unlikely fallback
        if not placed:
            return generate_password(length)

    return password


# -----------------------------
# Main
# -----------------------------

def main():

    while True:
        try:
            length = int(
                input(f"Password length (minimum {MIN_LENGTH}): ")
            )

            if length < MIN_LENGTH:
                print(f"Length must be at least {MIN_LENGTH}.")
                continue

            break

        except ValueError:
            print("Please enter a valid integer.")

    print("\nGenerated password:\n")
    print(generate_password(length))


if __name__ == "__main__":
    main()