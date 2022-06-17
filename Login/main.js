let submitBtn = document.getElementById("submitBtn");

let usernameInput = document.getElementById("usernameInput");
let passwordInput = document.getElementById("passwordInput");

submitBtn.addEventListener("click" , validateLogin);

function validateLogin(event)
{
    if(usernameInput === "" || passwordInput === "")
    {
        alert("Enter the required fields");
        return;
    }

    let xhr = new XMLHttpRequest();

    xhr.open("POST" , "server.c");
    xhr.setRequestHeader("Content-Type" , "texxt/html");

    xhr.onload = function()
    {
        console.log(this.responseText);
        let txt = `${this.responseText}`;
        if(txt.localeCompare("False") == 0)
        {
            alert("Enter Correct Credentials");
            usernameInput.value = "";
            passwordInput.value = "";
            return;
        }
        else
        {
            window.sessionStorage.setItem('Horizeon' , `${usernameInput.value}`);
            console.log(window.sessionStorage.getItem('Horizeon'));

            setTimeout(redirectPage , 1500);
        }
    }

    xhr.send(`~:ValidateLogin,${usernameInput.value},${passwordInput.value}`);
}

function redirectPage()
{
    let url = `${window.location}`;

    let ipAddress = "";
    let count = 0;

    for(let itr = 0 ; itr < url.length ; itr++)
    {
        if(url[itr] == '/')
        {
            count++;
        }
        if(count == 3)
        {
            break;
        }
        ipAddress += url[itr];
    }

    window.location = `${ipAddress}/Main/index.html`;
}



let signUpPage = document.getElementById("signUpPage");
signUpPage.addEventListener("click" , signUpPageRouter);

function signUpPageRouter()
{
    let url = `${window.location}`;

    let ipAddress = "";
    let count = 0;

    for(let itr = 0 ; itr < url.length ; itr++)
    {
        if(url[itr] == '/')
        {
            count++;
        }
        if(count == 3)
        {
            break;
        }
        ipAddress += url[itr];
    }

    window.location = `${ipAddress}/SignUp/index.html`;
}