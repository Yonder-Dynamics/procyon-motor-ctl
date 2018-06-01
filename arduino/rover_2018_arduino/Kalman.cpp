class Kalman {
  float F[9], B[9], x[3], p[3], Q[3], R;
  Kalman(float * F, float * B, float * x, float * p, float * Q, float R);
  float* predict(float dt, float * u);
  float* update(float * z);
};
