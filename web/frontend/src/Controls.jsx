/* eslint no-unused-vars: ["error", { "args": "none" }] */
import * as React from 'react';
import Box from '@mui/material/Box';
import Select from '@mui/material/Select';
import MenuItem from '@mui/material/MenuItem';
import FormControl from '@mui/material/FormControl';
import InputLabel from '@mui/material/InputLabel';
import UndoIcon from '@mui/icons-material/Undo';
import RedoIcon from '@mui/icons-material/Redo';
import { ActionCreators as UndoActionCreators } from 'redux-undo';
import Button from '@mui/material/Button';
import { useDispatch, connect } from 'react-redux';
import store from './store';


const InstanceIdPicker = ({handleCategoryIdChange, categoryId}) => {
  const dispatch = useDispatch();
  return (<Box sx={{ 'minWidth': '120px' }}>
    <InputLabel id="demo-simple-select-helper-label">Category ID</InputLabel>
    <Select
      value={categoryId}
      label="Category ID"
      onChange={handleCategoryIdChange}
    >
      {[...Array(10).keys()].map((idx) => (
        <MenuItem value={idx}>{idx}</MenuItem>
      ))}
    </Select>
  </Box>);
};

var Undo = ({canUndo}) => {
  const dispatch = useDispatch();
  return <Button
      disabled={!canUndo}
      onClick={() => dispatch(UndoActionCreators.undo())}
      variant="text">
      <UndoIcon />
  </Button>;
};

const mapStateToProps = state => {
  return { canUndo: state.points.past.length > 0,
    canRedo: state.points.future.length > 0 };
};


var Redo = ({canRedo}) => {
  const dispatch = useDispatch();
  return <Button
        disabled={!canRedo}
        onClick={() => dispatch(UndoActionCreators.redo())}
        variant="text">
    <RedoIcon />
  </Button>;
};

Undo = connect(mapStateToProps)(Undo);
Redo = connect(mapStateToProps)(Redo);

const Controls = ({
  handleTriangulate,
  handleSave
}) => {
  return (
    <FormControl sx={{ m: 1, minWidth: 120 }}>
      <Button onClick={handleSave} variant="text">Save</Button>
      <Button onClick={handleTriangulate} variant="text">
        Apply to all
      </Button>
    </FormControl>
  );
};

export { InstanceIdPicker, Undo, Redo, Controls };
