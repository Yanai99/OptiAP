import React,{useState, useRef} from 'react';
import Step1 from "./step1";
import Step2 from "./step2";
import Step3 from "./Step3";
import Step4 from "./Step4";
import Welcome from "../welcome/welcome";
import axios from 'axios';

function Steps(props) {
    const [uploadedImg, setUploadedImg] = useState(null);
    const[stepNumber,setStepNumber]=useState('1');
    const [arrayBoundingImg,setArrayBoundingImg]=useState([]);
    const [arrayBeaconsLocations , setArrayBeaconsLocations]=useState([]);
    const [arrayApsLocations , setArrayApsLocations]=useState([]); //Results from API...
     

    let step;
    // Render the appropriate screen based on the currentPage state

    // axios requests handling
    const baseURL = "http://localhost:8000";

    const SendBeacnosLocAndRGB = (arrayBeaconsLocations) => {
        axios.post(`${baseURL}/beacons_locations_and_colors`, {
            beacons_data: {arrayBeaconsLocations}
            }, {
            headers: {
                'Content-Type': 'application/json'
            }
        }).then((response) => console.log(response))
            .catch((error) => console.error(error));
    }

    const SendDone = (arrayBeaconsLocations) => {
        axios.get(`${baseURL}/done`)
            .then((response) => {
                const arrayApsLocations = response.data.optimal_locations;
                setArrayApsLocations(arrayApsLocations);
                console.log(arrayApsLocations); // This will log the arrayApsLocations array
        })
            .catch((error) => console.error(error));
    }


    if (uploadedImg === null)
    {
        step = <Step1 goToPage = {props.goToPage} setStepNumber={setStepNumber} setUploadedImg = {setUploadedImg} />;
    }
    else
    {
        // eslint-disable-next-line default-case
        switch (stepNumber)
        {
            case '2':
                step = <Step2 goToPage = {props.goToPage} setStepNumber={setStepNumber}
                              uploadedImg = {uploadedImg} setUploadedImg = {setUploadedImg}
                              arrayBoundingImg={arrayBoundingImg} setArrayBoundingImg={setArrayBoundingImg}/>;
                break;
            case '3':
                step = <Step3 goToPage = {props.goToPage} setStepNumber={setStepNumber} uploadedImg = {uploadedImg}
                arrayBoundingImg={arrayBoundingImg} arrayBeaconsLocations={arrayBeaconsLocations} setArrayBeaconsLocations={setArrayBeaconsLocations}
                SendBeacnosLocAndRGB = {SendBeacnosLocAndRGB}/>;
                break;
            case '4':
                step = <Step4 goToPage = {props.goToPage} setStepNumber={setStepNumber} uploadedImg = {uploadedImg}
                              arrayApsLocations={arrayApsLocations} SendDone = {SendDone}/>;
                break;
            case '0':
                props.goToPage('Welcome');
                step= <Welcome goToPage='Welcome' />;
                break;
        }
    }
    return (
        <div>
            {step}
        </div>
    );
}

export default Steps;