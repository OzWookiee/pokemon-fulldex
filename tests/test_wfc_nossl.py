from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


class WfcNoSslTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which('c++'), 'C++ compiler required')
    def test_wfc_urls_switch_to_http_only(self):
        with tempfile.TemporaryDirectory() as folder:
            exe = Path(folder) / 'wfc-nossl'
            subprocess.run(['c++', '-std=c++17',
                            '-I' + str(ROOT / 'frontend'),
                            str(ROOT / 'tests/test_wfc_nossl.cpp'), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)


if __name__ == '__main__':
    unittest.main()
