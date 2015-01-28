SELECT nrOfEditors, count(taskId) nrOfTasks, count(taskId)/nrOfDistTasks perentage
FROM
	(SELECT taskId, count(distinct user) nrOfEditors
	FROM track 
	WHERE 
		(action = 'set type' OR 
		action = 'set progress' OR 
		action = 'set start' OR 
		action = 'set target' OR
		action = 'set owner' OR 
		action = 'add participant' OR
		action = 'del participant' OR
		action = 'add expertise' OR
		action = 'del expertise') AND
		user REGEXP '^[a-z ]' 
	GROUP BY taskId) as taskEditors,
	(SELECT count(DISTINCT taskId) nrOfDistTasks
	FROM track 
	WHERE 
		(action = 'set type' OR 
		action = 'set progress' OR 
		action = 'set start' OR 
		action = 'set target' OR
		action = 'set owner' OR 
		action = 'add participant' OR
		action = 'del participant' OR
		action = 'add expertise' OR
		action = 'del expertise') AND
		user REGEXP '^[a-z ]' 
	) as allTasks
GROUP BY nrOfEditors
