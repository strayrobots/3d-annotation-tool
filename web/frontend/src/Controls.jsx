/* eslint no-unused-vars: ["error", { "args": "none" }] */
import * as React from 'react';
import Select from '@mui/material/Select';
import MenuItem from '@mui/material/MenuItem';
import FormControl from '@mui/material/FormControl';
import InputLabel from '@mui/material/InputLabel';
import { ActionCreators as UndoActionCreators } from 'redux-undo';
import Button from '@mui/material/Button';
import { useDispatch } from 'react-redux';

const Controls = ({
  handleCategoryIdChange,
  handleTriangulate,
  categoryId,
}) => {
  const dispatch = useDispatch();
  return (
    <FormControl sx={{ m: 1, minWidth: 120 }}>
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
      <Button
        onClick={() => dispatch(UndoActionCreators.undo())}
        variant="text"
      >
        Undo
      </Button>
      <Button
        onClick={() => dispatch(UndoActionCreators.redo())}
        variant="text"
      >
        Redo
      </Button>
      <Button variant="text">Save</Button>
      <Button onClick={handleTriangulate} variant="text">
        Apply to all
      </Button>
    </FormControl>
  );
};

export default Controls;
