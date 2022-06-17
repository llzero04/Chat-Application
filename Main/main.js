UserName = `${window.sessionStorage.getItem("Horizeon")}`;

let profileName = document.getElementById("userName");
profileName.innerHTML = `${UserName}`;


let Friends = [];
let Messages = [];

let currentFriend = "";

let friendList = document.getElementsByClassName("friendList")[0];
let chatArea = document.getElementsByClassName("chatArea")[0];
let friendCounter = document.getElementById("friendCounter");

friendList.addEventListener("click" , switchCurrentFriend);


//-------------------------------------------------------------------

function searchFriend(friendName)
{
    for(let itr = 0 ; itr < Friends.length ; itr++)
    {
        if(Friends[itr].localeCompare(friendName) == 0)
        {
            return itr;
        }
    }
    return -1;
}

function updateMessageArea(friendName)
{
    let idx = searchFriend(friendName);
    
    friendCounter.innerText = `USERS AVAILABLE : ${Friends.length}`;
    if(idx == -1)
    {
        console.log(friendName);
        console.log("No such Friend Exists");
        return;
    }
    console.log(idx);
    for(let itr = chatArea.children.length ; itr != 0 ; itr--)
    {
        console.log("Here")
        chatArea.removeChild(chatArea.children[0]);
    }
    
    for(let itr = 0 ; itr < Messages[idx].length ; itr++)
    {
        let newDiv = document.createElement("div");
        newDiv.innerText = `${Messages[idx][itr].msg}`;
        
        if(Messages[idx][itr].from.localeCompare(UserName) == 0)
        {
            newDiv.className = "userMessages";
        }
        else
        {
            newDiv.className = "othersMessages";
        }
        chatArea.appendChild(newDiv);
    }
}

//-------------------------------------------------------------

function switchCurrentFriend(event)
{
    if(event.target.className != "userFriend")
    {
        console.log("Wrong Click");
        return;
    }

    currentFriend = `${event.target.innerText}`;
    console.log(currentFriend);

    for(let itr = 0 ; itr < friendList.children.length ; itr++)
    {
        friendList.children[itr].style.transform = "scale(1.0)";
    }
    event.target.style.transform = "scale(0.97)";

    updateMessageArea(currentFriend);
}

//--------------------------------------------------------------

// Sending Input to Server

let userMessageInput = document.getElementById("userMessageInput");
let sendBtn = document.getElementById("sendBtn");

sendBtn.addEventListener("click" , sendMessage)

function sendMessage()
{
    if(userMessageInput.value == "")
    {
        window.alert("Enter Something for Sending");
        return;
    }

    let idx = searchFriend(currentFriend);
    Messages[idx].push({from : `${UserName}` , to : `${currentFriend}` , msg : `${userMessageInput.value}`});
    
    let xhr = new XMLHttpRequest();
    xhr.open("POST" , "server.c" , true);
    xhr.setRequestHeader("Content-Type" , "text/html");
    xhr.onload = function()
    {
        console.log("Message Sent");
    }

    xhr.send(`~:SendMessage,${UserName},${currentFriend},${userMessageInput.value}`);

    userMessageInput.value = "";

    updateMessageArea(currentFriend);
}

console.log(`${window.sessionStorage.getItem("Horizeon")}`);


//--------------------------------------------------------------


//Receiving Messages

function recieveMessages()
{
    let xhr = new XMLHttpRequest();

    xhr.open("POST" , "server.c" , true);
    xhr.setRequestHeader("Content-Type" , "text/html");

    xhr.onload = function()
    {
        // console.log(this.responseText);
        let text = this.responseText;
        let from = "";
        let to = "";
        let msg = "";
        let flag = 0;
        console.log(text);
        for(let itr = 0 ; itr < text.length ; itr++)
        {
            if(`${text[itr]}`.localeCompare("|") == 0)
            {
                if(msg.length == 0)
                {
                    if(searchFriend(from) != -1)
                    {
                        continue;
                    }
                    Friends.push(from);
                    Messages.push([]);

                    let newDiv = document.createElement("div");
                    newDiv.className = "userFriend";
                    newDiv.innerText = `${from}`;
                    friendList.appendChild(newDiv);
                    
                    flag = 0;
                    from = "";
                    to = "";
                    msg = "";
                    continue;
                }
                let obj = {from : `${from}` , to : `${to}` , msg : `${msg}`};
                let idx = searchFriend(from);
                if(idx == -1)
                {
                    console.log("error");
                }
                else
                {
                    Messages[idx].push(obj);
                }
                flag = 0;
                from = "";
                to = "";
                msg = "";
            }
            else if(`${text[itr]}`.localeCompare(",") == 0)
            {
                flag += 1;
            }
            else if(flag == 0)
            {
                from += text[itr];
            }
            else if(flag == 1)
            {
                to += text[itr];
            }
            else if(flag == 2)
            {
                msg += text[itr];
            }
        }
        updateMessageArea(currentFriend);
    }

    xhr.send(`~:ReceiveMessages,${UserName}`);

    console.log(Messages);
    console.log(Friends);
}

// recieveMessages();

let interval = setInterval(recieveMessages , 4000);

let logoutBtn = document.getElementById("logoutBtn");
logoutBtn.addEventListener("click" , logoutUser);

function logoutUser(event)
{
    clearInterval(interval);

    let xhr = new XMLHttpRequest();
    xhr.open("POST" , "server.c" , true);

    xhr.send(`~:logoutUser,${UserName}`);

    let url = `${window.location}`;
    let count = 0;
    let ipAddress = "";

    for(let itr = 0 ; itr < url.length ; itr++)
    {
        if(`${url[itr]}`.localeCompare("/") == 0)
        {
            count += 1;
        }
        if(count == 3)
        {
            break;
        }
        ipAddress += url[itr];
    }

    window.location = `${ipAddress}/Login/index.html`;
}