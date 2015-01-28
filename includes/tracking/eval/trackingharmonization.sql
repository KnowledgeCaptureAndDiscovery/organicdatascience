SELECT user FROM wttrackingreal.track GROUP BY user;

UPDATE track SET user = 'Felix' WHERE user = 'Felix Michel' AND id >0;
UPDATE track SET user = 'Paul' WHERE user = 'Paul Hanson' AND id >0;
UPDATE track SET user = 'Gil' WHERE user = 'Yolanda Gil' AND id >0;
UPDATE track SET user = 'Chris' WHERE user = 'Chris Duffy' AND id >0;
UPDATE track SET user = 'Varun' WHERE user = 'Varun Ratnakar' AND id >0;
UPDATE track SET user = 'Craig' WHERE user = 'Craig Snortheim' AND id >0;
UPDATE track SET user = 'Hilary' WHERE user = 'Hilary Dugan' AND id >0;
UPDATE track SET user = 'Jordan' WHERE user = 'Jordan Read' AND id >0;

UPDATE track SET user = 'Jordan' WHERE user = '130.11.177.41' AND id >0;
UPDATE track SET user = 'Admin' WHERE user = '86.179.210.119' AND id >0;
UPDATE track SET user = 'Paul' WHERE user = '24.177.202.220' AND id >0;
UPDATE track SET user = 'Paul' WHERE user = '10.129.9.232' AND id >0;
UPDATE track SET user = 'Hilary' WHERE user = '144.92.62.163' AND id >0;


SELECT count(*) FROM track WHERE user REGEXP '^[a-z ]'