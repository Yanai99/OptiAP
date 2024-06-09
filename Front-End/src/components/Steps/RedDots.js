import React from 'react';

const RedDots = ({ coordinates }) => {
    return (
        <div style={{ position: 'relative', width: '100%', height: '100%' }}>
            {coordinates.map((coordinate, index) => (
                <div
                    key={index}
                    style={{
                        position: 'absolute',
                        left: coordinate.x,
                        top: coordinate.y,
                        width: '10px',
                        height: '10px',
                        borderRadius: '50%',
                        backgroundColor: 'red'
                    }}
                />
            ))}
        </div>
    );
};

export default RedDots;