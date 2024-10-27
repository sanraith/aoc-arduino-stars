#include <avr/pgmspace.h>

const char WEBSITE_SOURCE_TEMPLATE[] PROGMEM = R"abc123~~~(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>aoc-arduino-stars config</title>
    <style>
        @keyframes fadeInOut {
            0% { opacity: 0; }
            10% { opacity: 1; }
            90% { opacity: 1; }
            100% { opacity: 0; }
        }

        #successMessage {
            display: none;
            color: green;
            font-size: 24px;
            animation: fadeInOut 1s ease-in-out;
        }

        .container {
            display: flex;
            justify-content: flex-start;
            margin: 0 auto;
            width: 50%; /* Adjust width as needed */
        }

        .form-group {
            display: flex;
            justify-content: flex-end;
            margin-bottom: 10px;
            width: 100%;
        }

        .form-group label {
            margin-right: 10px;
            width: 150px; /* Adjust width as needed */
            text-align: right;
        }

        .form-group input {
            margin-right: 10px;
        }
    </style>
    <script>
        function sendRequest(variableName, value = '') {
            const xhr = new XMLHttpRequest();
            xhr.open("GET", `/${variableName}?value=${encodeURIComponent(value)}`, true);
            xhr.onreadystatechange = function () {
                if (xhr.readyState === 4 && xhr.status === 200) {
                    const successMessage = document.getElementById('successMessage');
                    successMessage.style.display = 'block';
                    setTimeout(() => {
                        successMessage.style.display = 'none';
                    }, 1000);
                }
            };
            xhr.send();
        }
    </script>
</head>

<body>
    <h1>Advent of Code star display configuration</h1>
    <div class="container">
        <form>
            <div class="form-group">
                <label for="year">Year:</label>
                <input type="text" id="year" name="year" value="%d">
                <button type="button" onclick="sendRequest('year', document.getElementById('year').value)">Submit</button>
            </div>
            <div class="form-group">
                <label for="userId">User ID:</label>
                <input type="text" id="userId" name="userId" value="%s">
                <button type="button" onclick="sendRequest('userId', document.getElementById('userId').value)">Submit</button>
            </div>
            <div class="form-group">
                <label for="leaderboardId">Leaderboard ID:</label>
                <input type="text" id="leaderboardId" name="leaderboardId" value="%s">
                <button type="button" onclick="sendRequest('leaderboardId', document.getElementById('leaderboardId').value)">Submit</button>
            </div>
            <div class="form-group">
                <label for="sessionKey">Session Key:</label>
                <input type="text" id="sessionKey" name="sessionKey" value="%s">
                <button type="button" onclick="sendRequest('sessionKey', document.getElementById('sessionKey').value)">Submit</button>
            </div>
            <div class="form-group">
                <label for="leaderboardHost">Leaderboard Host:</label>
                <input type="text" id="leaderboardHost" name="leaderboardHost" value="%s">
                <button type="button" onclick="sendRequest('leaderboardHost', document.getElementById('leaderboardHost').value)">Submit</button>
            </div>
            <div class="form-group">
                <label for="leaderboardPort">Leaderboard Port:</label>
                <input type="text" id="leaderboardPort" name="leaderboardPort" value="%d">
                <button type="button" onclick="sendRequest('leaderboardPort', document.getElementById('leaderboardPort').value)">Submit</button>
            </div>
        </form>
    </div>

    <p style="font-size:24px;">Click <a href="#" onclick="sendRequest('update')">here</a> to force an update<br></p>

    <p id="successMessage">SUCCESS</p>
</body>
</html>
)abc123~~~";