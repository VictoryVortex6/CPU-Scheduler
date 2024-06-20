// project_directory/static/script.js
function addJob() {
    const container = document.getElementById('jobs-container');
    const jobDiv = document.createElement('div');
    jobDiv.classList.add('job');
    jobDiv.innerHTML = `
        <label>Job ID: <input type="number" name="id"></label>
        <label>Burst Time: <input type="number" name="burst"></label>
        <label>Arrival Time: <input type="number" name="arrival"></label>
    `;
    container.appendChild(jobDiv);
}

document.getElementById('job-form').addEventListener('submit', function(e) {
    e.preventDefault();

    const jobs = [];
    const jobElements = document.getElementsByClassName('job');
    for (let jobElement of jobElements) {
        const id = jobElement.querySelector('input[name="id"]').value;
        const burst = jobElement.querySelector('input[name="burst"]').value;
        const arrival = jobElement.querySelector('input[name="arrival"]').value;
        jobs.push({ id: parseInt(id), burst: parseInt(burst), arrival: parseInt(arrival) });
    }

    fetch('/schedule', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ jobs: jobs }),
    })
    .then(response => response.json())
    .then(data => {
        document.getElementById('output').innerText = data.output;
    });
});