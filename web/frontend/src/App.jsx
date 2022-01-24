/* eslint no-unused-vars: ["error", { "args": "none" }] */
import React, { useEffect } from 'react';
import axios from 'axios';
import CssBaseline from '@mui/material/CssBaseline';
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
import StatusBar from './StatusBar';
import { InstanceIdPicker, Undo, Redo, Controls } from './Controls';
import ImageGridCell from './ImageGridCell';
import { createTheme, ThemeProvider } from '@mui/material/styles';

const baseURL = 'http://localhost:5000/'; //TODO: proper backend url

const theme = createTheme({
    palette: {
      mode: 'dark',
      primary: {
        main: '#ffe910',
      },
      secondary: {
        main: '#f50057',
      },
      background: {
        default: '#303030',
      },
    }
});

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
    <ThemeProvider theme={theme}>
      <CssBaseline />
      <Container maxWidth="xl" sx={{padding: '24px',
          display: 'flex', flexDirection: 'column',
          justifyContent: 'space-between'
      }}>
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
        <StatusBar>
          <InstanceIdPicker handleCategoryIdChange={handleCategoryIdChange} categoryId={categoryId} />
          <Controls handleTriangulate={handleTriangulate} />
          <Box sx={{display: 'flex'}}>
            <Undo />
            <Redo />
          </Box>
        </StatusBar>
      </Container>
    </ThemeProvider>
  );
};

export default App;
