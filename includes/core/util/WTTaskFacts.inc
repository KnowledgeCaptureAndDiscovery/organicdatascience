<?php

class WTTaskFacts{
	
	public static $unknown = 'unknown';
	public static $tasktypes = array('high', 'medium', 'low');
	
	public static function getInitialTaskFacts(){
		$f = array();		
		$f['type'] = self::$unknown;	
		$f['expertise'] = array();
		$f['expertise'][] = self::$unknown;		
		$f['start'] = self::$unknown;
		$f['target'] = self::$unknown;
		$f['overdue'] = self::$unknown;
		$f['progress'] = self::$unknown;
		$f['owner'] = self::$unknown;
		$f['participants'] = array();
		$f['participants'][] = self::$unknown;	
		$f['metaprogress'] = 0;
		$f['missingparam'] = true;		
		return $f;
	}	
	
	public static function getExistingTaskFacts($res){		
		$f = array();		
		$f['type'] = self::typeFromFacts($res[2]);	
		$f['expertise'] = self::expertiseFromFacts($res[3]);	
		$f['start'] = self::startFromFacts($res[4]);
		$f['target'] = self::targetFromFacts($res[5]);
		$f['progress'] = self::progressFromFacts($res[6]);
		$f['owner'] = self::ownerFromFacts($res[7]);	
		$f['participants'] = self::participantsFromFacts($res[8]);				
		$f['metaprogress'] = self::metaProgress($f);	
		$f['missingparam'] = $f['metaprogress'] < 100;		
		return $f;
	}
	
	private static function typeFromFacts(&$res){		
		$type = self::$unknown;
		if($t = $res->getNextObject())	{		
			$t = strtolower($t->getText());
			if(in_array($t, self::$tasktypes))
				$type = $t;
		}
		return $type;
	}
	
	private static function expertiseFromFacts(&$res){
		$expertise = array();
		while($e = $res->getNextObject()) 
			$expertise[] = strtolower($e->getText());
		if(empty($expertise)) 
			$expertise[] = self::$unknown;
		return $expertise;
	}	
	
	private static function startFromFacts(&$res){
		$start = self::$unknown;
		if($s = $res->getNextObject()){
			if(preg_match('/^[2][0-9]{3}[-][0-1][0-9][-][0-3][0-9]/', $s->getText())){
				$date = DateTime::createFromFormat('Y-m-d', $s->getText(), new DateTimeZone('UTC'));
				$start = $date->getTimestamp();			
			}
		}
		return $start;		
	}
	
	private static function targetFromFacts(&$res){
		$target = self::$unknown;
		if($t = $res->getNextObject()){
			if(preg_match('/^[2][0-9]{3}[-][0-1][0-9][-][0-3][0-9]/', $t->getText())){ 
				$date = DateTime::createFromFormat('Y-m-d', $t->getText(), new DateTimeZone('UTC')); 
				$target = $date->getTimestamp();			
			}
		}
		return $target;		
	}
	
	private static function progressFromFacts(&$res){
		$progress = self::$unknown;
		if($p = $res->getNextObject()){
			if(get_class($p) == "SMWNumberValue")
				$progress = $p->getNumber();
		}
		return $progress;		
	}
	
	private static function participantsFromFacts(&$res){
		$participants = array();
		while($p = $res->getNextObject()) 
			$participants[] = $p->getText();	
		if(empty($participants)) 
			$participants[] = self::$unknown;
		return $participants;
	}
	
	private static function ownerFromFacts(&$res){
		$owner = self::$unknown;
		if($o = $res->getNextObject())
			$owner = $o->getText();
		return $owner;
	}
	
	private static function metaProgress(&$facts){
		$u = self::$unknown;
		$missing = 0;
		if($facts['type'] == $u)
			$missing ++;
		if($facts['start'] == $u)
			$missing ++;
		if($facts['target'] == $u)
			$missing ++;
		if($facts['owner'] == $u)
			$missing ++;
		if($facts['progress'] === $u && ($facts['type'] == 'low' || $facts['type'] == self::$unknown))
			$missing ++;
			
		if($facts['type'] == 'low' || $facts['type'] == self::$unknown)
			return ((5-$missing)/5)*100;
		else
			return ((4-$missing)/4)*100;
	}
	
	
	
	
	
}
