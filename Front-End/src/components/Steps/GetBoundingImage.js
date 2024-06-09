import {useEffect, forwardRef} from "react";

const GetBoundingImage=(props)=>
{
    const imageElement= document.getElementById(props.idImg);
    let imgBounding=[];
    if (imageElement)
    {
        const rect = imageElement.getBoundingClientRect();
        imgBounding = [
            rect.top,
            rect.left,
            rect.right,
            rect.bottom,
            rect.width,
            rect.height
        ];
    }

    props.setArrayBounding(imgBounding);

    return(
        <p>{imgBounding[0]} , {imgBounding[1]} , {imgBounding[2]} , {imgBounding[3]} , {imgBounding[4]} , {imgBounding[5]} </p>
    );
}
export default GetBoundingImage;