import os
import chess
from fen_eval import perft_eval
from pathlib import Path

def which(bin_name):
    path = os.getenv('PATH')
    for bin_path in path.split(os.path.pathsep):
        bin_path = os.path.join(bin_path, bin_name)
        if os.path.exists(bin_path) and os.access(bin_path, os.X_OK):
            return bin_path

stockfish_path = which('stockfish')
chessengine_path = '../chessengine-static'

perft_directory = 'eval-perft/'


def get_perfts():
    perfts = []

    for filename in os.listdir(perft_directory):
        if filename.endswith(".perft"):
            perft_path = perft_directory + filename
            perft_base = os.path.basename(perft_path)
            perft_name = os.path.splitext(perft_base)[0]
            with open (perft_path, "r") as perft_file:
                perft_string = perft_file.readline().rstrip("\n")
                perfts.append((perft_name, perft_string))

    return perfts

perfts = get_perfts()

# This is usefull to normalize Score objects to integers
# and should be a large value
mate_score = 1000000000

# There is a total order defined on centi-pawn and mate scores.
# >>> from chess.engine import Cp, Mate, MateGiven
# >>> Mate(-0) < Mate(-1) < Cp(-50) < Cp(200) < Mate(4) < Mate(1) < MateGiven
# True
def score_to_centipawns(pov_score):
    score = pov_score.relative

    if type(score) == chess.engine.Cp:
        assert score.score() < mate_score

    return score.score(mate_score=mate_score)

def ref_eval(perft):
    res = score_to_centipawns(perft_eval(perft, stockfish_path))
    print(f"stockfish : {res}")
    return res

def eval(perft):
    res = score_to_centipawns(perft_eval(perft, chessengine_path))
    print(f"blipblop : {res}")
    return res

def identity(x):
    return x

def first(couple):
    return couple[0]

def second(couple):
    return couple[1]

# Construct a list of all the possible unordered couples in list
def n_choose_2(list):
    if len(list) == 0:
        return []

    couples_with_first = []

    first, rest = list[0], list[1:]

    for elt in rest:
        couples_with_first.append((first, elt))

    return couples_with_first + n_choose_2(rest)

def index_by_key(list, elt, key=identity):
    list_with_key_applied = [key(list[i]) for i in range(len(list))]

    return list_with_key_applied.index(elt)

def get_choice_dissonance(perft_couple, perft_and_ref_value_couples, perft_and_value_couples):
    first_perft, second_perft = perft_couple

    first_perft_ref_index = index_by_key(perft_and_ref_value_couples, first_perft, key=first)
    second_perft_ref_index = index_by_key(perft_and_ref_value_couples, second_perft, key=first)

    first_perft_index = index_by_key(perft_and_value_couples, first_perft, key=first)
    second_perft_index = index_by_key(perft_and_value_couples, second_perft, key=first)

    first_perft_ref_value = second(perft_and_ref_value_couples[first_perft_ref_index])
    second_perft_ref_value = second(perft_and_ref_value_couples[second_perft_ref_index])

    first_perft_value = second(perft_and_value_couples[first_perft_index])
    second_perft_value = second(perft_and_value_couples[second_perft_index])

    if (first_perft_ref_index <= second_perft_ref_index and first_perft_index <= second_perft_index) \
       or (second_perft_ref_index <= first_perft_ref_index and second_perft_index <= first_perft_index):
        return None

    return {'perft_string_1': first_perft,
            'perft_string_1_ref_value': first_perft_ref_value,
            'perft_string_1_value': first_perft_value,
            'perft_string_2': second_perft,
            'perft_string_2_ref_value': second_perft_ref_value,
            'perft_string_2_value': second_perft_value}

def get_winner_dissonance(perft, perft_and_ref_value_couples, perft_and_value_couples):
    perft_ref_index = index_by_key(perft_and_ref_value_couples, perft, key=first)
    perft_index = index_by_key(perft_and_value_couples, perft, key=first)

    perft_ref_value = second(perft_and_ref_value_couples[perft_ref_index])
    perft_value = second(perft_and_value_couples[perft_index])

    if (perft_ref_value <= 0 and perft_value <= 0) or (perft_ref_value > 0 and perft_value > 0):
        return None

    return {'perft_string': perft,
            'perft_string_ref_value': perft_ref_value,
            'perft_string_value': perft_value}


# Assumes that the couples are sorted by value
def dissonances(perft_and_ref_value_couples, perft_and_value_couples):
    winner_dissonances = []
    choice_dissonances = []

    perft_strings = [perft[1] for perft in perfts]

    for perft in perft_strings:
        winner_dissonance = get_winner_dissonance(perft, perft_and_ref_value_couples, perft_and_value_couples)
        if winner_dissonance:
            winner_dissonances.append(winner_dissonance)

    for perft_couple in n_choose_2(perft_strings):
        choice_dissonance = get_choice_dissonance(perft_couple, perft_and_ref_value_couples, perft_and_value_couples)
        if choice_dissonance:
            choice_dissonances.append(choice_dissonance)

    return {'winner_dissonances': winner_dissonances, 'choice_dissonances': choice_dissonances}

def evaluate_eval_fun(eval_fun):
    perft_and_ref_value_couples = []
    perft_and_value_couples = []

    nb_perfts = len(perfts)
    for i in range(nb_perfts):
        perft_name = perfts[i][0]
        perft_string = perfts[i][1]
        print(i + 1, "/", nb_perfts, ": ", perft_name, ": ", perft_string, sep="")
        perft_and_ref_value_couples.append((perft_string, ref_eval(perft_string)))
        perft_and_value_couples.append((perft_string, eval_fun(perft_string)))
        print()

    perft_and_ref_value_couples.sort(key=second)
    perft_and_value_couples.sort(key=second)

    return dissonances(perft_and_ref_value_couples, perft_and_value_couples)

dissonances = evaluate_eval_fun(eval)
nb_perfts = len(perfts)
print("nb perft tested:", nb_perfts)
print("nb winner dissonances: ", len(dissonances["winner_dissonances"]), "/", nb_perfts, sep="")
print("nb choice dissonances: ", len(dissonances["choice_dissonances"]), "/", int((nb_perfts * (nb_perfts - 1)) / 2), sep="")
#print(dissonances)
