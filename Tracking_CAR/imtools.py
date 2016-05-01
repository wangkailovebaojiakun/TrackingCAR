import os

def get_imlist(path,str):
    return [os.path.join(path,f) for f in os.listdir(path) if f.endswith(str)]
