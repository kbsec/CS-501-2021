from flask import Flask 

app = Flask(__name__)

@app.route("/hello_there",  methods=["GET"])
def hello_there():
    return "General Kenobi!"

if __name__ == "__main__":
    app.run()