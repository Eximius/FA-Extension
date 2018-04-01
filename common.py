
import os

def write_asm(filename, src):
    if not os.path.exists('build/'+os.path.dirname(filename)):
        os.makedirs('build/'+os.path.dirname(filename))

    with open(filename, 'w') as f:
        f.write(src)

def write_hook(filename, src):
    filename = 'build/hooks/'+filename
    if not os.path.exists(os.path.dirname(filename)):
        os.makedirs(os.path.dirname(filename))

    with open(filename, 'w') as hf:
        hf.write(src)