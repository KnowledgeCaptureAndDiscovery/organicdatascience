<?php 
require_once 'WTTrack.php';
require_once 'WTTrackLogin.php';

class WTTrackAPI{
	
	public static function call($call){
		header('Content-type: application/json');
		switch ($call){
			case('track'): self::track(); break;
			case('read'): self::read(); break;
			case('del'): self::del(); break;
			default: throw new InvalidArgumentException('Action not defined!');
		}
	}
		
	public static function track(){
		$c = $_POST['component'];
		$sc = $_POST['subcomponent'];
		$a = $_POST['action'];
		$t = $_POST['actiontype'];
		$i = $_POST['taskId'];
		$v = $_POST['value'];
		$u = $_POST['user'];	
		$d = $_POST['domain'];	
		$winX = $_POST['windowWidth'];	
		$winY = $_POST['windowHeight'];	
		$screenX = $_POST['screenWidth'];	
		$screenY = $_POST['screenHeight'];			
		$ip = $_SERVER['REMOTE_ADDR'];
		$agent = $_SERVER['HTTP_USER_AGENT'];
		$referrer = $_SERVER['HTTP_REFERER'];
		$success = WTTrack::track($c, $sc, $a, $t, $i, $v, $u, $d, $winX, $winY, $screenX, $screenY, $ip, $agent);
		echo json_encode(array("success"=>$success));		
	}
	
	public static function read(){	
		$tl = new WTTrackLogin();
		if($tl->isLoggedIn())
			echo json_encode(WTTrack::readAll());		
	}
	
	public static function del(){	
		$tl = new WTTrackLogin();
		if($tl->isLoggedIn())
			echo json_encode(WTTrack::delAll());
	}
}

WTTrackAPI::call($_POST['call']);



?>