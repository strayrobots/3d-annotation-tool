import undoable from 'redux-undo';
import { createSlice } from '@reduxjs/toolkit';

export const pointSlice = createSlice({
  name: 'points',
  initialState: {
    frames: [],
    //firstPoints: [],
    //secondPoints: [],
  },
  reducers: {
    addPoint: (state, action) => {
      state.frames = state.frames.map((frame) => {
        return { ...frame, keypoints: [...frame.keypoints, action.payload] };
      });
      //state.firstPoints = [...state.firstPoints, action.payload];
      //state.secondPoints = [...state.secondPoints, action.payload];
    },
    setFrame: (state, action) => {
      state.frames = state.frames.map((frame) => {
        if (action.payload.idx === frame.idx) {
          return action.payload;
        } else {
          return frame;
        }
      });
    },
    setFrames: (state, action) => {
      state.frames = action.payload;
    },
    /*
    setFirstPoints: (state, action) => {
      state.firstPoints = action.payload;
    },
    setSecondPoints: (state, action) => {
      state.secondPoints = action.payload;
    },
    */
  },
});

// Action creators are generated for each case reducer function
export const { addPoint, setFrame, setFrames } = pointSlice.actions;

const undoablePoints = undoable(pointSlice.reducer);

export default undoablePoints;
