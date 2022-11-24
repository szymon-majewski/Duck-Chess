SetupBoard();
SetupSidePanel();
SetStartingPosition();

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
}

function SetupSidePanel()
{
    document.getElementById("DuckSquare").addEventListener("click", OnClick, false);
    //document.getElementById("NewGameButton").onclick = SetStartingPosition();
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
        document.getElementById(id).appendChild(img);
    }
}

let moving = false;
let srcDiv;

function OnClick(e)
{
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

            if (parentDiv != srcDiv)
            {
                parentDiv.innerHTML = "";
                parentDiv.appendChild(srcDiv.childNodes[0]);
                srcDiv.innerHTML = "";
                DeselectSquare(srcDiv);
                SendMoveToServer(srcDiv.id, parentDiv.id);

                moving = false;
            }
        }
        else
        {
            srcDiv = div.parentNode;
            moving = true;

            if (srcDiv.classList.contains("LightSquare"))
            {
                srcDiv.classList.add("SelectedLightSquare");
                srcDiv.classList.remove("LightSquare");
            }
            else if (srcDiv.classList.contains("DarkSquare"))
            {
                srcDiv.classList.add("SelectedDarkSquare");
                srcDiv.classList.remove("DarkSquare");
            }
        }     
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
}

function SendMoveToServer(srcSquare, destSquare)
{
    const data = { srcSquare, destSquare };
    const options = 
    { 
        method: 'POST',
        headers: {'Content-Type': 'application/json'},
        body: JSON.stringify(data) 
    };

    fetch('/api', options);
}