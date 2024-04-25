

enum DistanceSensorState
{
  DISTANCE_SENSOR_OFF,
  DISTANCE_SENSOR_ON,
  DISTANCE_SENSOR_BETWEEN_READINGS,
  DISTANCE_SENSOR_AWAITING_READING,
  DISTANCE_SENSOR_READING_READY
};

inline void startDistanceSensorReading();

void setupDistanceSensor(int readingIntervalInMillisecs);

inline void startWaitBetweenReadings();
inline void updateSensorBetweenReadings();

void updateDistanceSensor();

int getDistanceValueInt();

float getDistanceValueFloat();

void directDistanceReadTest();

void testDistanceSensor();
