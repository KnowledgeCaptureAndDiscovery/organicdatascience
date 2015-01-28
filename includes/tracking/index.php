<?php 
require_once 'WTTrackLogin.php';

$tl = new WTTrackLogin();
if($tl->isPost())
	$tl->login();
if($tl->isLoggedIn())
	include 'WTTrackEval.php';
else 	
	include 'WTTrackLogin.html';

?>