<?php 

$url =  'http://'.$_SERVER['HTTP_HOST'].$_SERVER['REQUEST_URI']; 
$url = str_replace('index.php', 'WTTrackAPI.php', $url);

?>
<html>
	<head>		
		<script type="text/JavaScript" src="http://code.jquery.com/jquery-2.1.1.min.js"></script>	
		<script type="text/JavaScript" src="WTTrackEval.js"></script>
		<link href="WTTrackEval.css" rel="stylesheet" type="text/css"/>	
		<script type="text/javascript">
			$(function() {
				
				var url = '<?php echo $url; ?>';
				var $table = $('#table');
				var wtte = new WTTrackEval(url, $table);
				wtte.init();
			});
		</script>
	</head>
	<body>
		<h1>WorkflowTask Tracking</h1>
		<a href="#" id="del">Delete All</a>
		<div id="table"></div>
	</body>
</html>
