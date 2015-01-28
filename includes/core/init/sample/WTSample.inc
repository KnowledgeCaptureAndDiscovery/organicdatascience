<?php


class WTSample{
	 
   	public function __construct(){   		
   		$this->loadXMLSchema();
   		set_time_limit (60*60);
   		foreach ($this->xml->children() as $child){
   			if($child->getName() == 'person')
   				self::createPersonPage($child);
   			if($child->getName() == 'task')
   				$this->create(null, $child);
   		}
   	}
   	
   	private function loadXMLSchema(){
   		global $wgCanonicalServer;
		global $wgScriptPath;
		$url = $wgCanonicalServer.$wgScriptPath.'/extensions/WorkflowTasks/includes/sample/sample.xml';
   		$this->xml = simplexml_load_file($url);
   	}
   	
   	private function createPersonPage($person){
   		$attr = $person->attributes();
   		$name = $attr->title.'';
   		if($name != '' && !WTTaskUtil::pageExist($name)){
   			WTFactsAPI::createPageWithCategory($name, 'Person');
			foreach (explode('|', $attr->expertise.'') as $e)
				WTFactsAPI::addFact($name, 'Expertise', $e);	
   		}
   		if($userId = $attr->userid)
   			WTFactsAPI::setFact($name, 'Has User ID', $userId);
   	}
	
	private function create($parentTitle, $task){
		$attr = $task->attributes();
		$title = $attr->title.'';
		$type = $attr->type.'';
		$progress = $attr->progress.'';		
		$owner = $attr->owner.'';
		$participants = explode('|', $attr->participants.'');
		$expertise = explode('|', $attr->expertise.'');
		$start = self::getDate($attr->start);
		$target = self::getDate($attr->target);
		$content = $task->content.'';	
		$taskId = self::addTask($parentTitle, $title, $type, $progress, $owner, $participants, $start, $target, $expertise, $content);	
		if($subtasks = $task->subtasks)
			foreach ($subtasks->children() as $child)
				$this->create($taskId, $child);		
	}
	
	public static function addTask($parentTitle, $subtitle, $type, $progress, $owner, $participants, $start, $target, $expertise, $content){
		$subtitle = preg_replace("/\//", " or ", $subtitle);
		if(!WTTaskUtil::pageExist($subtitle)){ // && !WTTaskUtil::taskExist($subtitle)){ //TODO check by once
			WTFactsAPI::createPageWithCategory($subtitle, 'Task');			
			self::addConent($subtitle, $content);
			if($parentTitle)
				WTFactsAPI::addFact($parentTitle, 'SubTask', $subtitle, null, true, null);				
			if($type)
				WTFactsAPI::setFact($subtitle, 'Type', $type);		
			if($type == 'low' && $progress)
				WTFactsAPI::setFact($subtitle, 'Progress', $progress);	
			if($start)
				WTFactsAPI::setFact($subtitle, 'StartDate', $start);	
			if($target)
				WTFactsAPI::setFact($subtitle, 'TargetDate', $target);
			if($owner)
				WTFactsAPI::setFact($subtitle, 'Owner', $owner);	
			foreach($participants as $k => $p)
				WTFactsAPI::addFact($subtitle, 'Participants', $p);		
			foreach($expertise as $k => $e)
				WTFactsAPI::addFact($subtitle, 'Expertise', $e);			
		}	
		return $subtitle;
	}
	
	private static function getDate($days){
		if($days){
			$pdays = abs($days);			
			$i = intval($pdays);
			$start = new DateTime("now", new DateTimeZone('UTC'));
			$interval = new DateInterval('P'.$i.'D');
			if($days<0)
				$interval->invert = true;
			$start->add($interval);
			return $start->format('Y-m-d');
		}
		return null;
	}
	
	private static function addConent($title, $content){
		$title = Title::newFromText($title); 
    	if ($title) { 
      		$article = new Article($title); 
      		return $article->doEdit($article->getContent().$content, 'Init', EDIT_UPDATE); 
    	}
	}

	
}