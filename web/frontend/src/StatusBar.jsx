import * as React from 'react';
import Box from '@mui/material/Box';
import Grid from '@mui/material/Grid';

const StatusBar = (props) => {
  return (
    <Box sx={{ display: 'flex', justifyContent: 'space-between' }}>
      {props.children}
    </Box>
  );
};

export default StatusBar;
