import { configureStore } from '@reduxjs/toolkit';
import pointReducer from './reducers/points';

export default configureStore({
  reducer: { points: pointReducer },
});
