import './step.css';
import Header from "../Headers/Header";
import Underline from "../Underline";
import Upload from "./UploadTemplate/UploadTemplate";
import React from 'react';


const Step1 =(props)=>
{
    return (
            <div>
                <Header goToPage={props.goToPage}/>
                <p className="textHead">Step 1</p>
                <p className="textPara">Upload a map of a structure where you're willing to identify optimal access
                points' placements.</p>
                <Upload setUploadedImg={props.setUploadedImg} setStepNumber={props.setStepNumber}/>
                <Underline/>
            </div>
    );
}

export default Step1;