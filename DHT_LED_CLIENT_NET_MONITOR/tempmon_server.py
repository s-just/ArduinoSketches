from flask import Flask, request, jsonify
import datetime

app = Flask(__name__)

latest_temperature = None
latest_humidity = None
last_updated_timestamp = None

@app.route('/data', methods=['POST'])
def receive_data():
    global latest_temperature, latest_humidity, last_updated_timestamp
    if request.method == 'POST':
        temperature = request.form.get('temperature')
        humidity = request.form.get('humidity')

        if temperature and humidity:
            latest_temperature = temperature
            latest_humidity = humidity
            last_updated_timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            print(f"[{last_updated_timestamp}] Received data: Temperature = {latest_temperature}C, Humidity = {latest_humidity}%")
            return "Data received successfully", 200
        else:
            return "Missing DHT data", 400
    return "Invalid request method", 405

@app.route('/latest_readings', methods=['GET'])
def get_latest_readings():
    data = {
        'temperature': latest_temperature,
        'humidity': latest_humidity,
        'timestamp': last_updated_timestamp
    }
    return jsonify(data)

@app.route('/')
def index():
    with open('index.html', 'r') as f:
        return f.read()

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8000, debug=True)
