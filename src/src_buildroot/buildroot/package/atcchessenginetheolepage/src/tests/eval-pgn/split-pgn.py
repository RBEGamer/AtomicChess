import io
import os
import functools
import chess
import chess.pgn

def line_is_a_comment(line):
    return line.startswith("[")

def split_in_sections(pgn_path):
    pgn_sections = []

    with open(pgn_path, "r") as pgn_file:
        line = pgn_file.readline()

        pgn_section = ""
        in_comment_section = True

        while line:
            if line_is_a_comment(line):
                if not in_comment_section:
                    pgn_sections.append(pgn_section)
                    pgn_section = ""
                    in_comment_section = True
            else:
                in_comment_section = False

            pgn_section += line
            line = pgn_file.readline()

    return pgn_sections

# The moves up to half of the game
def pgn_section_begin(pgn_section):
    lines = pgn_section.splitlines()

    comments = ""

    i = 0
    while not lines[i] or lines[i].isspace() or line_is_a_comment(lines[i]):
        comments += lines[i] + "\n"
        i += 1

    nb_move_lines = len(lines) - i
    first_move_lines = lines[i:i+nb_move_lines//2]

    return comments + functools.reduce(lambda str1, str2 : str1 + "\n" + str2, first_move_lines) + "\n"

def pgn_string_to_perft_string(pgn_string):
    game = chess.pgn.read_game(io.StringIO(pgn_string))
    game = game.end()
    board = game.board()

    return board.fen()

def cut_last_word(string):
    return string.rsplit(' ', 1)[0]

def generate_perfts(pgn_path):
    pgn_sections = split_in_sections(pgn_path)
    for i in range(len(pgn_sections)):
        pgn_section = pgn_sections[i]

        perft_string = pgn_string_to_perft_string(pgn_section)
        perft_path = "../eval-perft/" + os.path.splitext(pgn_path)[0] + "_" + str(i + 1) + ".perft"
        with open(perft_path, "w") as perft_file:
            # eval-evaluation.py takes a perft string without its last number
            perft_file.write(cut_last_word(perft_string))

def generate_all_perfts():
    for filename in os.listdir("."):
        if filename.endswith(".pgn"):
            generate_perfts(filename)

generate_all_perfts()
