<?php 

class WTTrackDB{
	
	private static $URL = "localhost";
	private static $USER = "root";
	private static $PASSWORD = "";
	private static $DATABASE = "wttracking";
	
	public static function connect(){				
		return new PDO('mysql:host='.self::$URL.';dbname='.self::$DATABASE, self::$USER, self::$PASSWORD);
	}	

}


?>
