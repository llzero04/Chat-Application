let loginBtn = document.getElementById("loginBtn");
let signUpBtn = document.getElementById("signUpBtn");

loginBtn.addEventListener("click" , redirectLogin);
signUpBtn.addEventListener("click" , redirectSignUp);

function redirectLogin(event)
{
    let url = `${window.location}`;
    let ipAddress = "";
    let count = 0;

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

function redirectSignUp(event)
{
    let url = `${window.location}`;
    let ipAddress = "";
    let count = 0;

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

    window.location = `${ipAddress}/SignUp/index.html`;
}