from flask import Flask, request, jsonify, render_template
import subprocess

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/schedule', methods=['POST'])
def schedule():
    data = request.json
    jobs = data['jobs']

    input_file = 'input.txt'
    output_file = 'output.txt'

    # Write the job data to input.txt
    with open(input_file, 'w') as f:
        for job in jobs:
            f.write(f"{job['id']} {job['burst']} {job['arrival']}\n")

    # Run the C++ scheduling program
    subprocess.run(['./main'], check=True)

    # Read the output file
    with open(output_file, 'r') as f:
        output_data = f.read()

    return jsonify({'output': output_data})

if __name__ == '__main__':
    app.run(debug=True)
