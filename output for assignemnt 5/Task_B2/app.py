from flask import Flask, jsonify
import time
app = Flask(__name__)
RESULTS = {"CSA0405001": "PASS (A)", "CSA0405002": "PASS (B)", "CSA0405003": "FAIL"}

@app.route("/")
def home():
    return "<h1>SIMATS Result Portal</h1><p>Enter your register number at /result/&lt;regno&gt;</p>"

@app.route("/result/<regno>")
def result(regno):
    return jsonify(regno=regno, result=RESULTS.get(regno, "NOT FOUND"))

@app.route("/health")
def health():
    return jsonify(status="ok", ts=time.time())

@app.route("/ping")
def ping():
    # fixed-length body so ab does not count length differences as failures
    return "pong"
