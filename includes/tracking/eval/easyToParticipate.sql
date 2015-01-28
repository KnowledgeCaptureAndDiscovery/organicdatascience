#shows how many users have deleted a task short after creation

SELECT  ta.addId, addtime, deltime, adduser, deluser
FROM 
	(SELECT SUBSTRING_INDEX(SUBSTRING_INDEX(value, '"', 4) , '"', -1) addId, time addtime, user adduser
	FROM track 
	WHERE action = 'add subtask' OR action = 'add root task') ta,
	(SELECT replace(taskId, '_', ' ') delId, time deltime, user deluser
	FROM track 
	WHERE action = 'del task') td
WHERE ta.addId = td.delId AND (addtime > TIMESTAMPADD(MINUTE, -5, deltime) AND addtime < deltime);

'Xuans first task', '2014-09-17 05:50:03', '2014-09-17 05:50:41', 'Xuan Yu', 'Xuan Yu'
'Xuans first task', '2014-09-17 05:50:03', '2014-09-17 05:54:00', 'Xuan Yu', 'Xuan Yu'
'Xuans first task', '2014-09-17 05:51:22', '2014-09-17 05:54:00', 'Xuan Yu', 'Xuan Yu'