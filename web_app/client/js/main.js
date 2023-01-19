SetupBoard();
SetupSidePanel();
SetStartingPosition();

document.addEventListener('contextmenu', function(e)
{
    e.preventDefault();

    if (moving)
    {
        DeselectSquare(srcDiv);
        moving = false;
    }
})

function SetupBoard()
{
    let light = false;

    for (let y = 0; y < 8; ++y)
    {
        light = !light;

        for (let x = 0; x < 8; ++x)
        {
            let div = document.createElement("div");
            div.classList.add("Square");
            light ? div.classList.add("LightSquare") : div.classList.add("DarkSquare");
            div.setAttribute("id", (String.fromCharCode("A".charCodeAt(0) + x)) + (8 - y));
            div.addEventListener("click", OnClick, false);

            document.getElementById("Board").appendChild(div);

            light = !light;
        }
    }

    document.getElementById("DuckSquare").addEventListener("click", OnClick, false);

    let ranks = document.getElementById("RanksPanel");
    let files = document.getElementById("FilesPanel");

    for (let i = 0; i < 8; ++i)
    {
        const ranksParagraph = document.createElement("p");
        const filesParagraph = document.createElement("p");
        const ranksDiv = document.createElement("div");
        const filesDiv = document.createElement("div");

        ranksParagraph.classList.add('RanksFilesParagraph');
        filesParagraph.classList.add('RanksFilesParagraph');
        ranksDiv.classList.add('RanksDiv');
        filesDiv.classList.add('FilesDiv');

        ranksParagraph.innerHTML = i + 1;
        filesParagraph.innerHTML = String.fromCharCode("A".charCodeAt(0) + i);

        ranksDiv.appendChild(ranksParagraph);
        filesDiv.appendChild(filesParagraph);

        ranks.appendChild(ranksDiv);
        files.appendChild(filesDiv);
    }
}

function SetupSidePanel()
{
    document.getElementById("ResetBoardButton").onclick = function()
    {
        if (moving)
        {
            DeselectSquare(srcDiv);
            moving = false;
        }

        CleanBoard();
        SetStartingPosition();
    };

    document.getElementById("FindGameButton").onclick = FindGame;
}

function SetStartingPosition()
{
    let dict = {};

    dict["A1"] = "../res/whiteRook.png";
    dict["H1"] = "../res/whiteRook.png";
    dict["B1"] = "../res/whiteKnight.png";
    dict["G1"] = "../res/whiteKnight.png";
    dict["C1"] = "../res/whiteBishop.png";
    dict["F1"] = "../res/whiteBishop.png";
    dict["D1"] = "../res/whiteQueen.png";
    dict["E1"] = "../res/whiteKing.png";
    dict["A8"] = "../res/blackRook.png";
    dict["H8"] = "../res/blackRook.png";
    dict["B8"] = "../res/blackKnight.png";
    dict["G8"] = "../res/blackKnight.png";
    dict["C8"] = "../res/blackBishop.png";
    dict["F8"] = "../res/blackBishop.png";
    dict["D8"] = "../res/blackQueen.png";
    dict["E8"] = "../res/blackKing.png";
    dict["DuckSquare"] = "../res/duck.png"

    for (let i = 0; i < 8; ++i)
    {
        dict[String.fromCharCode("A".charCodeAt(0) + i) + 2] = "../res/whitePawn.png";
        dict[String.fromCharCode("A".charCodeAt(0) + i) + 7] = "../res/blackPawn.png";
    }

    for (const [id, path] of Object.entries(dict))
    {
        let img = document.createElement("img");
        img.src = path;

        switch (path.charAt(7)) // when paths to images changes, this won't work
        {
            case 'b': { img.classList.add('black'); break; }
            case 'w': { img.classList.add('white'); break; }
            case 'd': { img.classList.add('duck'); break; }
        }
        
        document.getElementById(id).appendChild(img);
    }
}

function CleanBoard()
{
    for (let y = 0; y < 8; ++y)
    {
        for (let x = 0; x < 8; ++x)
        {
            document.getElementById(String.fromCharCode("A".charCodeAt(0) + x) + (y + 1)).innerHTML = "";
        }
    }

    document.getElementById("DuckSquare").innerHTML = "";
}

let playersTurn = true;
let playingOnline = false;

let moving = false;
let srcDiv;

function OnClick(e)
{
    if (playingOnline && !playersTurn)
    {
        return;
    }

    let div = e.target;

    if (div.tagName == "DIV")
    {
        if (moving)
        {
            if (div.children.length == 0)
            {
                div.appendChild(srcDiv.childNodes[0]);
                srcDiv.innerHTML = "";
                DeselectSquare(srcDiv)
                SendMoveToServer(srcDiv.id, div.id);
            }
            moving = false;
        }
    }
    else if (div.tagName == "IMG")
    {
        if (moving)
        {
            let parentDiv = div.parentNode;

            // switching players pieces
            if ((parentDiv.childNodes[0].classList.contains('white') && srcDiv.childNodes[0].classList.contains('white')) ||
                (parentDiv.childNodes[0].classList.contains('black') && srcDiv.childNodes[0].classList.contains('black')))
            {
                DeselectSquare(srcDiv);
                SelectSquare(parentDiv);
                srcDiv = parentDiv;
            }

            // check if player wants to capture duck or wants to capture piece with duck
            else if (!srcDiv.childNodes[0].classList.contains('duck') &&
                !parentDiv.childNodes[0].classList.contains('duck'))
            {
                parentDiv.innerHTML = "";
                parentDiv.appendChild(srcDiv.childNodes[0]);
                srcDiv.innerHTML = "";
                DeselectSquare(srcDiv);
                SendMoveToServer(srcDiv.id, parentDiv.id);

                moving = false;

                //after first move disable ducksquare
            }
        }
        else
        {
            srcDiv = div.parentNode;
            moving = true;

            SelectSquare(srcDiv);
        }     
    }
}

function SelectSquare(square)
{
    if (square.classList.contains("LightSquare"))
    {
        square.classList.add("SelectedLightSquare");
        square.classList.remove("LightSquare");
    }
    else if (square.classList.contains("DarkSquare"))
    {
        square.classList.add("SelectedDarkSquare");
        square.classList.remove("DarkSquare");
    }
    else
    {
        square.classList.add("SelectedDuckSquare");
        square.classList.remove("DuckSquare");
    }
}

function DeselectSquare(square)
{
    if (square.classList.contains("SelectedLightSquare"))
    {
        square.classList.remove("SelectedLightSquare");
        square.classList.add("LightSquare");
    }
    else if (square.classList.contains("SelectedDarkSquare"))
    {
        square.classList.remove("SelectedDarkSquare");
        square.classList.add("DarkSquare");
    }
    else
    {
        square.classList.remove("SelectedDuckSquare");
        square.classList.add("DuckSquare");
    }
}

function SendMoveToServer(srcSquare, destSquare)
{
    const data = { src: srcSquare, dest: destSquare };
    const options = 
    { 
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify(data) 
    };

    fetch('/move', options);
    // .then((response) =>
    // {
    //     return response.json();
    // })
    // .then((move) =>
    // {
    //     const sourceDiv = document.getElementById(move.src);
    //     const destinationDiv = document.getElementById(move.dest);

    //     destinationDiv.innerHTML = "";
    //     destinationDiv.appendChild(sourceDiv.childNodes[0]);
    //     sourceDiv.innerHTML = "";
    // });
}

function FindGame()
{
    fetch('/findGame', { method: 'POST' })
    .then((response) =>
    {
        return response.json();
    })
    .then((response) =>
    {
        console.log(response.movesFirst);
    });

    document.getElementById("FindGameButton").disabled = true;
}