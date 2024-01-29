from argparse import ArgumentParser
from argparse import RawDescriptionHelpFormatter
from pathlib import Path
from difflib import unified_diff
from termcolor import colored
import os
import subprocess as sp

DEFAULT_TIMEOUT = 5

def run_shell(args):
    """ Run a process with given args. Return the captured output """

    args = ["timeout", "--signal=KILL", f"{DEFAULT_TIMEOUT}"] + args
    res = sp.run(args, capture_output=True, text=True)
    if (res.returncode == -9):
        raise TimeoutError(f"Timeout after {DEFAULT_TIMEOUT} seconds")
    return res

def diff(ref, student):
    """ Return nice difference between two inputs strings """

    ref = ref.splitlines(keepends=True)
    student = student.splitlines(keepends=True)

    return ''.join(unified_diff(ref, student, fromfile="ref", tofile="student"))

def launch_diff(binary, listener, pgn, out):
    """ Launch the test and print OK or KO (+diff) depending on the result

    return : 1 if test passed, 0 otherwise
    """

    with open(out, "r") as out_file:
        ref_out = out_file.read()
        try:
            student = run_shell([binary, "--pgn", pgn, "-l", listener])
        except exception as err:
            print(f"[{colored('KO', 'red')}]", pgn)
            if verbosity:
                print(err)
            return 0
        if (ref_out != student.stdout):
            print(f"[{colored('KO', 'red')}]", pgn)
            if verbosity:
                print(f"out differs:\n{diff(ref_out, student.stdout)}")
            return 0

    print(f"[{colored('OK', 'green')}]", pgn)
    return 1

def pretty_print_synthesis(passed, failed):
    """ Print on stdout the synthesis depending on nb failed/passed tests """

    print(f"[{colored('==', 'red' if failed else 'blue')}] Synthesis:", end='')
    color_tested = colored(f"{passed + failed:2d}", 'blue')
    print(f" Tested: {color_tested} | ", end='')
    color_passed = colored (f"{passed:2d}", 'green')
    print(f"Passing: {color_passed} | ", end='')
    color_failed = colored (f"{failed:2d}", 'red' if failed else 'blue')
    print(f"Failing: {color_failed}")


def launch_tests(binary, listener, pgn_folder, verbosity):
    """ Launch with binary and listener all pgn of pgn_folder then compare """

    print("\n" + " PGN-TESTSUITE ".center(80, "-"))
    passed, failed = 0, 0
    for pgn in pgn_folder.rglob('*.pgn'):
        test_name = os.path.splitext(pgn)[0]
        out = test_name + ".out"
        if launch_diff(binary, listener, pgn, out):
            passed += 1
        else:
            failed += 1

    print("\n" + " GLOBAL SYNTHESIS ".center(80, "-"))
    pretty_print_synthesis(passed, failed)
    print("".center(80, "-") + "\n")

if __name__ == "__main__":
    """ This script launch pgn-testsuite """

    # Set all arguments that can be used by the script
    parser = ArgumentParser(description= "PGN-testsuite : compare pgn.out with binary out",
            formatter_class = RawDescriptionHelpFormatter)
    parser.add_argument("bin", metavar="BINARY")
    parser.add_argument("listener", metavar="LISTENER")
    parser.add_argument("pgnFolder", metavar="PGN_FOLDER")
    parser.add_argument("-v", "--verbose", action="store_true",
            dest="verbose", default=False,
            help="Show all errors of tests")
    args = parser.parse_args()

    binary = Path(args.bin).absolute()
    listener = Path(args.listener).absolute()
    pgn_folder = Path(args.pgnFolder).absolute()
    verbosity = args.verbose

    launch_tests(binary, listener, pgn_folder, verbosity)
