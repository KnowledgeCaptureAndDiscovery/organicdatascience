DROP VIEW IF EXISTS eval_userlog;
CREATE VIEW eval_userlog as
	SELECT page_title title, rev_user_text user,  CONVERT(rev_comment USING utf8) comment, rev_text_id, CONVERT(rev_timestamp USING utf8) timestamp
	FROM revision, page
	WHERE (rev_comment = '' OR CONVERT(rev_comment USING utf8) like '/*%*/') AND # may filter not logged in users
		rev_page=page_id AND rev_text_id IN (
		SELECT rev_text_id
		FROM text
		WHERE CONVERT(old_text USING utf8) like '[[Category:Task]]%')
	ORDER BY rev_timestamp DESC
	LIMIT 100000000000;


SELECT nrOfEditors, count(title) nrOfTasks, count(title)/nrOfDistTasks perentage
FROM
	(SELECT title, count(distinct user) nrOfEditors
	FROM eval_userlog 
	GROUP BY title) as taskEditors,
	(SELECT count(DISTINCT title) nrOfDistTasks
	FROM eval_userlog
	) as allTasks
GROUP BY nrOfEditors;

#SELECT * FROM eval_userlog WHERE title = 'Train_Felix_on_using_organic_data_science_wiki';