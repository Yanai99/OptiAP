import './step.css'
import React, {useState} from "react";
import RedCircles from "./RedDots";
/*
props.array[0] <- element.top
props.array[1] <- element.left
props.array[2] <- element.right
props.array[3] <- element.bottom
props.array[4] <- element.width
props.array[5] <- element.height
 */
function GetClientChoice(props) {
   // const [circles,setCircles] = useState(null);
    let circles;
    const updateDotsArray=(x,y)=>{
        let tmp=[...props.arrayClientChoice];
        tmp.push({x: x, y: y});
        props.setArrayClientChoice(tmp);
    }

    document.addEventListener("click", (event)=> {
        const x = event.clientX;
        const y = event.clientY;
        circles += <circle cx={x} cy={y} r="5" fill="red" />;
        updateDotsArray(x,y);
        console.log(`Clicked at (${x}, ${y})`);  // Log the coordinates
        });


    return (
        <div>
            <p>{circles}</p>
        </div>
    );
}
/*

    //props.increaseCountStep();
    return (
        <div>
                       <RedCircles pointsArray={props.arrayClientChoice}/>
        </div>
    );



    /*
    let x,y;
    if (props.array)
    {
        document.addEventListener("click", (event)=> {
            x = event.clientX;
            y = event.clientY;
            // Log the coordinates
            console.log(`Clicked at (${x}, ${y})`);
        });

        if (x >= props.array[1] && x <= props.array[2])
        {
            if (y >= props.array[0] && y <= props.array[3])
            {
                props.setInImg(true);
                props.increaseCountStep();
            }
        }
        return(
            <div>
            <ul>
                {dots.map(item => {
                    return <li>{item[0]} ,</li>;
                })}
            </ul>

                <RedCircle x={x} y={y}/>
            </div>
        );
               /* else {
                    return(
                        <div>
                            <p>notInImg</p>
                            <p>{props.array[0]} , {props.array[1]} , {props.array[2]} , {props.array[3]} , {props.array[4]} , {props.array[5]} </p>
                        </div>
                    );
                }*/




export default GetClientChoice;