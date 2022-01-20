import React from 'react';
import { Image as KonvaImage, Layer } from 'react-konva';

const BaseImage = ({ image }) => {
  const layerRef = React.useRef(null);

  return (
    <Layer ref={layerRef}>
      <KonvaImage image={image} />
    </Layer>
  );
};

export default BaseImage;
