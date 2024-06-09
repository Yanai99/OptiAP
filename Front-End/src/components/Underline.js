import React from 'react';
import YOLosoft from "./YOLosoft";
import '../ButtonsDesigns.css'

function Underline() {
    return (
        <div>
            <table>
                <tr> <YOLosoft/></tr>
                <tr className="headerText">2024 © YOLosoft</tr>
                <tr className="headerText"> All rights reserved.</tr>
            </table>
        </div>

    );
}

export default Underline;