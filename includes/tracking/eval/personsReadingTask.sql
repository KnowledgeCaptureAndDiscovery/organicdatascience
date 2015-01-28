SELECT nrOfPersons, count(*) nrOfTasks
FROM 
	(SELECT taskId, count(Distinct user) nrOfPersons
	FROM track
	WHERE action = 'open task' AND user REGEXP '^[a-z ]'
	GROUP BY taskId) taskPersons
GROUP BY nrOfPersons;