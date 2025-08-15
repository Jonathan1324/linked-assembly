import subprocess

def build(debug):
    cmd = ["make"]
    if (debug): cmd.append("DEBUG=1")
    subprocess.run(cmd)

def clean(debug):
    cmd = ["make", "clean"]
    if (debug): cmd.append("DEBUG=1")
    subprocess.run(cmd)