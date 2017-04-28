import contextlib
import os
import subprocess
import tempfile


@contextlib.contextmanager
def mktemp(contents):
    _, path = tempfile.mkstemp()
    with open(path, "w+") as f:
        f.write(contents)
    yield path
    os.remove(path)


def run_fixture(fixture):
    fixture_contents = open("tests/fixtures/{}.yaml".format(fixture)).read()
    with mktemp("struct Foo {}\n") as swift_fixture:
        with mktemp(fixture_contents.replace("{file}", swift_fixture)) as path:
            return run_command(path)


def run_command(filepath):
    args = [x for x in ["./skit", filepath] if x is not None]
    process = subprocess.Popen(args, stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
    process.wait()
    stdout, stderr = process.communicate()
    return process.returncode, stdout, stderr
