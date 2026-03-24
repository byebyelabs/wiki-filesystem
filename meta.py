# A metaprogram to dynamically change the output file for Python-to-C communication

import os

with open("wikipedia.c", "r") as f:
    code = f.read()

updated_code = code.replace("$CWD", os.getcwd())

with open("wikipedia.c", "w") as f:
    f.write(updated_code)
