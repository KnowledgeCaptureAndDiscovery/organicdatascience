#registred wiki users
SELECT count(CONVERT(user_name USING utf8)) registerdUsers
FROM ods_wiki.user; 

#active users
SELECT count(DISTINCT user) activeUser 
FROM track 
WHERE user REGEXP  '[a-z]' AND user != 'undefined' AND user != 'notloggedin';

#number of logs
SELECT count(*) FROM wttrackingreal.track;

#tracking interval
SELECT min(time) tfrom, max(time) tto FROM wttrackingreal.track;

#accessed task pages
SELECT count(*) FROM wttrackingreal.track
WHERE action = 'open page' AND value='Task';

#accessed person pages
SELECT count(*) FROM wttrackingreal.track
WHERE action = 'open page' AND value='Person';

