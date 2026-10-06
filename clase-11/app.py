from flask import Flask, jsonify

app = Flask(__name__)


@app.get("/")
def index():
    return jsonify(message="Hola desde Docker en EC2 🐳")


@app.get("/health")
def health():
    return jsonify(status="ok")


if __name__ == "__main__":
    # Solo para desarrollo local; en el contenedor se usa gunicorn
    app.run(host="0.0.0.0", port=8000)