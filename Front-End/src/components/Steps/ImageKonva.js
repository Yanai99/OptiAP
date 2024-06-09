import useImage from 'use-image';
import { Image } from 'react-konva';

const ImageKonva = ({img, handleClickImage}) => {

    const [image, status, width, height] = useImage(img);
    return <Image image={image} width={width} height={height} onClick={handleClickImage}/>;

}

export default ImageKonva;