<?php 

class WTTrackLogin{
	
	private static $HASH = '7df3194dbf9b90cf34ce882abfc89afd1a4bcb60b61b8701c4d4ce7245df31d1';
	
	public function __construct() {
		session_start();
	}
	
	public function isLoggedIn(){
		return isset($_SESSION['login']) && $_SESSION['login'] == true;
	}
	
	public function login(){	
		$pw = hash('sha256', $_POST['pw']);
		if($pw === self::$HASH){
			$_SESSION['login'] = true;
			return true;
		}else{
			session_destroy();
			return false;
		}		
	}
		
	public function isPost(){
		return $_SERVER['REQUEST_METHOD'] == 'POST';
	}
}


?>
