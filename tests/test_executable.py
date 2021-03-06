from helpers import *
import unittest


class TestSourceKit(unittest.TestCase):
    def test_empty_yaml(self):
        with mktemp("\n") as path:
            code, out, err = run_command(path)
            self.assertEqual(code, 1)
            self.assertEqual(out, "")
            self.assertEqual(
                err,
                "Got an error from sourcekitd when creating yaml request: " +
                "Expected value as array, dictionary, or scalar at: \n")

    def test_random_strings(self):
        with mktemp("foo\n") as path:
            code, out, err = run_command(path)
            self.assertEqual(code, 1)
            self.assertEqual(out, "")
            self.assertIn("Got an error from sourcekitd:", err)

    def test_invalid_request(self):
        code, out, err = run_fixture("invalid_request")
        self.assertEqual(code, 1)
        self.assertEqual(out, "")
        self.assertEqual(err, "Got an error from sourcekitd: request invalid\n")

    def test_valid_request(self):
        code, out, err = run_fixture("valid_request")
        self.assertEqual(code, 0)
        self.assertEqual(out, open("tests/fixtures/valid.json").read())
        self.assertEqual(err, "")

    def test_text_request(self):
        code, out, err = run_command("tests/fixtures/valid_text_request.yaml")
        self.assertEqual(code, 0)
        self.assertEqual(out, open("tests/fixtures/sourcetext.json").read())
        self.assertEqual(err, "")

    def test_empty_argument(self):
        code, out, err = run_command("")
        self.assertEqual(code, 1)
        self.assertEqual(out, "")
        self.assertEqual(err, "No readable file exists at ''\n")

    def test_no_argument(self):
        code, out, err = run_command(None)
        self.assertEqual(code, 1)
        self.assertEqual(out, "")
        self.assertEqual(err, "Usage: skit [YAMLFILE|-v|--version]\n")

    def test_invalid_flag(self):
        code, out, err = run_command("--versionfoobar")
        self.assertEqual(code, 1)
        self.assertEqual(out, "")
        self.assertEqual(err, "No readable file exists at '--versionfoobar'\n")

    def test_version_command(self):
        code, out, err = run_command("--version")
        self.assertEqual(code, 0)
        self.assertRegexpMatches(out, r"\d+\.\d+\.\d+")
        self.assertEqual(err, "")

    def test_short_version_command(self):
        code, out, err = run_command("-v")
        self.assertEqual(code, 0)
        self.assertRegexpMatches(out, r"\d+\.\d+\.\d+")
        self.assertEqual(err, "")
