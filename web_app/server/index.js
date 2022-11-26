const express = require('express');
const app = express();
const port = 3000;
let playersList = new Array();
let move;

app.use(express.static('../client/html'));
app.use('/css', express.static("../client/css"));
app.use('/js', express.static("../client/js"));
app.use('/res', express.static("../client/res"));

app.use(express.json({limit: '1mb'}));

app.listen(port, () => console.log('listening on port ' + port));

app.post('/findGame', (request, response) =>
{
    let clientAddress = request.socket.remoteAddress;
    console.log(clientAddress + " wants to play!");

    if (!playersList.includes(clientAddress))
    {
        playersList.push(clientAddress)

        if (playersList.length == 2)
        { 
            response.json({ movesFirst: false });
            SetupGame();
        }
        else
        {
            response.json({ movesFirst: true });
        }
    }
})

let storedMove = null;

app.post('/move', (request, response) => 
{
    console.log(request.socket.remoteAddress + " moved [src: " + request.body.src + ", dest: " + request.body.dest + "]");

    if (storedMove != null)
    {
        response.json({ src: storedMove.src, dest: storedMove.dest });
    }

    storedMove = request.body;
});

function SetupGame()
{
    console.log("Setting up a game. White: " + playersList[0] + ", Black: " + playersList[1]);
}