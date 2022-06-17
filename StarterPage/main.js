let text = document.querySelector('.animate-text');

const strText = text.textContent;


const splitText = strText.split("");

// console.log(strText);
// console.log(splitText);

text.textContent = "";

for(let i = 0 ; i < splitText.length ; i++)
{

    text.innerHTML += `<span>${splitText[i]}</span>`
}


let char = 0;

let timer = setInterval(onTick , 50);

function onTick()
{
    const span = text.querySelectorAll('span')[char];
    span.classList.add('fade');
    char = char + 1;
    if(char ===  splitText.length)
    {
        complete();
        return;
    }
}



function complete()
{
    text.classList.add('something');
    console.log(text.classList);
    clearInterval(timer);
    timer = null;

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

    window.setTimeout(() => {window.location = `${ipAddress}/HomePage/index.html`} , 1500);
}