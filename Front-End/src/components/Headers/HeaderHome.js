import React from 'react';
import YOLosoft from "../YOLosoft";
import '../../ButtonsDesigns.css'
import './Header.css'

function HeaderHome(props) {
    return (
        <div>
            <table>
                <td> <YOLosoft/></td>
                <td><button className="headerTextBlue" onClick={()=>props.goToPage('Welcome')}>Home</button></td>
                <td><button className="headerText" onClick={()=>props.goToPage('AboutUs')}>About us</button></td>
            </table>
        </div>

    );
}

export default HeaderHome;