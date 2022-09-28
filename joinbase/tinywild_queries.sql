select count(ts) as number_of_records from iot_into_the_wild.sensors;

select animal,count(animal) as number_of_animal from iot_into_the_wild.sensors group by animal;

select count(animal) as number_of_highly_true_ducks from iot_into_the_wild.sensors where animal = 'Duck' and confidence > 70;

select count(light) as number_of_great_sunlight from iot_into_the_wild.sensors where light > 900;

select count(light) as number_of_near_lightless from iot_into_the_wild.sensors where light < 50;
