import './step.css';
import Header from "../Headers/Header";
import Underline from "../Underline";
import React from 'react';
import '../../ButtonsDesigns.css';
import videojs from 'video.js';
import Step2Exp from '../../assets/Step2Exp.mp4';

const Step2 =(props)=>{
    let tmp;

    return (
            <div>
                <Header goToPage={props.goToPage}/>
                <p className="textHead">Step 2</p>
                <pre className="textPara">{"Upload complete. \n Now ensure to delineate the submitted map by marking the vertices encompassing the structure. " }</pre>
                <p className="textParaSmaller">You can view a demonstration in the video provided below. </p>
                <video controls width="350" height="250">
                    <source src={Step2Exp} type="video/mp4" />
                </video>
                <div className="image-preview">
                    <img id="image" src={props.uploadedImg} alt="Uploaded Map" style={{ maxWidth: '100%' }}
                         onClick={(event)=>
                    {
                        console.log(event.clientX,event.clientY);
                        tmp=[...props.arrayBoundingImg];
                        tmp.push({x: event.pageX, y: event.pageY});
                        props.setArrayBoundingImg(tmp);
                    }
                    }/>
                    {props.arrayBoundingImg.map((coordinate, index) => (
                        <div
                            key={index}
                            style={{
                                position: 'absolute',
                                left: coordinate.x,
                                top: coordinate.y,
                                width: '10px',
                                height: '10px',
                                borderRadius: '50%',
                                backgroundColor: 'red',
                                transform: 'translate(-50%, -50%)' // Center the dot on the clicked point
                            }}
                        />
                    ))}
                </div>
                <table className={'tableBody'}>
                    <td><button className={'buttonGrey'} onClick={() => {
                        props.setUploadedImg(null);
                        props.setStepNumber('1');}}>Back</button></td>
                    <td><button className={'buttonGrey'} onClick={() =>props.setStepNumber('3')}>Next</button></td>
                </table>
                <Underline/>
            </div>
        );
}
/*
                <GetBoundingImage idImg={'image'} arrayBounding={arrayBounding} setArrayBounding={setArrayBounding}/>
                <GetClientChoice arrayBounding={arrayBounding} arrayClientChoice={arrayClientChoice} setArrayClientChoice={setArrayClientChoice} setInImg={props.setInImg} increaseCountStep={props.increaseCountStep}/>
                <Underline/>
 */

export default Step2;