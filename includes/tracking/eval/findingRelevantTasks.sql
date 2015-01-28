SELECT component, subcomponent, count(*) openedTasks, count(*)/sum percentage
FROM track t, (SELECT count(*) sum FROM track WHERE action = 'open task') a
WHERE action = 'open task'
GROUP BY component, subcomponent;

SELECT count(*) 
FROM wttracking.track
WHERE action = 'open page';
