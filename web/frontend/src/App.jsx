/* eslint no-unused-vars: ["error", { "args": "none" }] */
import React, { useEffect } from 'react';
import axios from 'axios';
import Box from '@mui/material/Box';
import Grid from '@mui/material/Grid';
import Container from '@mui/material/Container';
import Slider from '@mui/material/Slider';
import { Stage, Layer, Circle } from 'react-konva';
import BaseImage from './BaseImage';
import Select from '@mui/material/Select';
import MenuItem from '@mui/material/MenuItem';
import FormControl from '@mui/material/FormControl';
import InputLabel from '@mui/material/InputLabel';
import { useSelector, useDispatch } from 'react-redux';
import { addPoint, setFrame, setFrames } from './reducers/points';
import { ActionCreators as UndoActionCreators } from 'redux-undo';
import Button from '@mui/material/Button';
import Controls from './Controls';
import ImageGridCell from './ImageGridCell';

const baseURL = 'http://localhost:5000/'; //TODO: proper backend url

const App = () => {
  const dispatch = useDispatch();
  const [images, setImages] = React.useState([]);
  const [firstImageIdx, setFirstImageIdx] = React.useState(0);
  const [secondImageIdx, setSecondImageIdx] = React.useState(0);
  const [categoryId, setCategoryId] = React.useState(0);

  const frames = useSelector((state) => {
    return state.points.present.frames;
  });

  useEffect(() => {
    axios.get(baseURL).then((response) => {
      const reduxFrames = response.data.sort().map((frame) => {
        const idx = frame.idx;
        const keypoints = frame.keypoints;
        return { idx, keypoints };
      });
      const images = response.data.sort().map((frame) => {
        const image = new Image(640, 480);
        image.src = baseURL + frame.file;
        return image;
      });
      dispatch(setFrames(reduxFrames));
      setImages(images);
    });
  }, [dispatch]);

  const handleCategoryIdChange = (event) => {
    setCategoryId(event.target.value);
  };

  const handleFirstSliderChange = (event, newValue) => {
    setFirstImageIdx(newValue);
  };

  const handleSecondSliderChange = (event, newValue) => {
    setSecondImageIdx(newValue);
  };

  const handleTriangulate = () => {
    axios
      .post(baseURL + 'triangulate', {
        frame_1: frames[firstImageIdx],
        frame_2: frames[secondImageIdx],
      })
      .then((response) => {
        const reduxFrames = response.data.sort().map((frame) => {
          const idx = frame.idx;
          const keypoints = frame.keypoints;
          return { idx, keypoints };
        });

        dispatch(setFrames(reduxFrames));
      });
  };

  return (
    <Container maxWidth="xl">
      <Controls
        handleCategoryIdChange={handleCategoryIdChange}
        handleTriangulate={handleTriangulate}
        categoryId={categoryId}
      />
      {frames.length > 0 && (
        <Grid container spacing={4}>
          <ImageGridCell
            frame={frames[firstImageIdx]}
            image={images[firstImageIdx]}
            categoryId={categoryId}
            handleSliderChange={handleFirstSliderChange}
            sliderRange={frames.length - 1}
          />
          <ImageGridCell
            frame={frames[secondImageIdx]}
            image={images[secondImageIdx]}
            categoryId={categoryId}
            handleSliderChange={handleSecondSliderChange}
            sliderRange={frames.length - 1}
          />
        </Grid>
      )}
    </Container>
  );
};

export default App;
