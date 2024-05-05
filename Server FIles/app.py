from flask import Flask, request, jsonify
from flask_socketio import SocketIO, send, emit

app = Flask(__name__)
app.config['SECRET_KEY'] = 'your_secret_key'
socketio = SocketIO(app)

beacon_locations = {}
beacon_colors = {}
map_size = (0,0)

# HTTP route to handle beacons locations and colors
@app.route('/beacons_locations_and_colors', methods=['POST'])
def get_beacons_locations_and_colors():
    # Get JSON data from the request body
    data = request.get_json()
    # Extract beacons data
    beacons_data = data.get('beacons data', [])

    for index, beacon in enumerate(beacons_data, start=1):
        # Ensure the beacon data is correctly formatted
        if len(beacon) == 2 and isinstance(beacon[0], (list, tuple)) and isinstance(beacon[1], (list, tuple)):
            beacon_id = f'beacon{index}'
            beacon_colors[beacon_id] = tuple(beacon[0])
            beacon_locations[beacon_id] = tuple(beacon[1])
    print(beacon_locations)
    # Return the dictionaries as JSON
    return jsonify({
        'locations': beacon_locations,
        'colors': beacon_colors
    })

# HTTP route to handle map size
@app.route('/map_size', methods=['GET'])
def get_map_size():
    # Get JSON data from the request body
    data = request.get_json()

    # Extract width and length from the JSON data
    width = data.get('width')
    length = data.get('length')

    # Validate that width and length are provided and are integers
    if width is None or length is None:
        return jsonify({'error': 'Width and length are required'}), 400
    if not isinstance(width, int) or not isinstance(length, int):
        return jsonify({'error': 'Width and length must be integers'}), 400

    # Store the map size in a global variable
    map_size = (width, length)

    # Respond with the saved map size
    return jsonify({'map_size': map_size})

# Send to Front-end the router coordinates

# WebSocket event to handle messages from ESP32
@socketio.on('message')
def handle_ws_message(msg):
    print(f"Received message from ESP32: {msg}")
    send(f"Response to {msg}")

# WebSocket event for a custom event from ESP32
@socketio.on('esp32_event')
def handle_esp32_event(data):
    print(f"Received event from ESP32: {data}")
    emit('response', {'data': 'Acknowledged by server!'})

if __name__ == '__main__':
    socketio.run(app, debug=True, port=8000)  # Change port as needed
