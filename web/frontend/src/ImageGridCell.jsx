/* eslint no-unused-vars: ["error", { "args": "none" }] */
import * as React from 'react';
import Box from '@mui/material/Box';
import Grid from '@mui/material/Grid';
import Slider from '@mui/material/Slider';
import { Stage, Layer, Circle } from 'react-konva';
import BaseImage from './BaseImage';
import { useDispatch } from 'react-redux';
import { addPoint, setFrame } from './reducers/points';

const INSTANCE_COLORS = [
  [0.85098039, 0.37254902, 0.00784314],
  [0.10588235, 0.61960784, 0.46666667],
  [0.49803922, 0.78823529, 0.49803922],
  [0.74509804, 0.68235294, 0.83137255],
  [0.99215686, 0.75294118, 0.5254902],
  [1, 1, 0.6],
  [0.21960784, 0.42352941, 0.69019608],
  [0.94117647, 0.00784314, 0.49803922],
  [0.74901961, 0.35686275, 0.09019608],
  [0.90588235, 0.16078431, 0.54117647],
].map(
  (color) => `rgb(${color[0] * 255}, ${color[1] * 255}, ${color[2] * 255})`,
);

const ImageGridCell = ({
  frame,
  image,
  categoryId,
  handleSliderChange,
  sliderRange,
}) => {
  const dispatch = useDispatch();

  const handleDragStart = (e) => {
    const id = e.target.id();
    const updatedKeypoints = frame.keypoints.map((kp) => {
      return {
        ...kp,
        isDragging: kp.id === id,
      };
    });

    const updatedFrame = { ...frame, keypoints: updatedKeypoints };
    dispatch(setFrame(updatedFrame));
  };
  const handleDragEnd = (e) => {
    const id = e.target.id();
    const x = e.target.x();
    const y = e.target.y();

    const updatedKeypoints = frame.keypoints.map((kp) => {
      return {
        ...kp,
        x: kp.id === id ? x : kp.x,
        y: kp.id === id ? y : kp.y,
        isDragging: false,
      };
    });

    const updatedFrame = { ...frame, keypoints: updatedKeypoints };
    dispatch(setFrame(updatedFrame));
  };

  const handleAddKeypoint = (e) => {
    const x = e.evt.layerX;
    const y = e.evt.layerY;
    const id = frame.keypoints.length.toString();
    const kp = {
      id,
      x,
      y,
      categoryId,
      isDragging: false,
    };
    dispatch(addPoint(kp));
  };
  return (
    <Grid item xs={6}>
      <Stage width={640} height={480} onDblClick={handleAddKeypoint}>
        <BaseImage image={image} />
        <Layer>
          {frame.keypoints.map((kp, i) => {
            return (
              <Circle
                id={kp.id}
                key={i}
                x={kp.x}
                y={kp.y}
                radius={3}
                fill={INSTANCE_COLORS[kp.categoryId]}
                draggable
                onDragStart={handleDragStart}
                onDragEnd={handleDragEnd}
              />
            );
          })}
        </Layer>
      </Stage>
      <Box width={640}>
        <Slider
          step={1}
          min={0}
          max={sliderRange}
          onChange={handleSliderChange}
        />
      </Box>
    </Grid>
  );
};

export default ImageGridCell;
