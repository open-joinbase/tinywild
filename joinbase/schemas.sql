creata database if not exists iot_into_the_wild;

create table iot_into_the_wild.sensors (
    ts DateTime,
    light Int16,
    sound Int16,
    imu_x Int16,
    imu_y Int16,
    imu_z Int16,
    animal String,
    confidence Int8
)
PARTITION BY yyyymmdd(ts);