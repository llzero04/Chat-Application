let submitBtn = document.getElementById("submitBtn");

let usernameInput = document.getElementById("usernameInput");
let passwordInput = document.getElementById("passwordInput");
let rePasswordInput = document.getElementById("rePasswordInput");

submitBtn.addEventListener("click" , validateSignUp);

function validateSignUp(event)
{
    if(usernameInput.value == "" || passwordInput.value == "" || rePasswordInput.value == "")
    {
        window.alert("Enter all the Fields");
        return;
    }

    let userName = `${usernameInput.value}`;
    let password = `${passwordInput.value}`;
    let rePassword = `${rePasswordInput.value}`;

    if(password.localeCompare(rePassword) != 0)
    {
        window.alert("Password Mismatch");
        return;
    }

    let xhr = new XMLHttpRequest();

    xhr.open("POST" , "server.c" , true);

    xhr.onload = function()
    {
        console.log(this.responseText);
        let response = `${this.responseText}`;
        if(response.localeCompare("True") == 0)
        {
            let url = `${window.location}`;
            let ipAddress = "";
            let count = 0;

            for(let itr = 0 ; itr < url.length ; itr++)
            {
                if(`${url[itr]}`.localeCompare("/") == 0)
                {
                    count++;
                }
                if(count == 3)
                {
                    break;
                }
                ipAddress += url[itr];
            }

            setTimeout(() => {window.location = `${ipAddress}/Login/index.html`} , 1000);
        }
        else
        {
            window.alert("Username already taken");
        }
    }

    xhr.send(`~:ValidateSignUp,${usernameInput.value},${passwordInput.value}`);

    usernameInput.value = "";
    passwordInput.value = "";
    rePasswordInput.value = "";
}