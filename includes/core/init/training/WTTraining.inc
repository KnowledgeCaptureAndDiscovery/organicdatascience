<?php


class WTTraining{
	
	public static $USER_PLACEHOLDER = '@@name@@';
	public static $TRAININDEX_PLACEHOLDER = '@@trainindex#@@';

    public function __construct() { 
        $a = func_get_args(); 
        $i = func_num_args(); 
        if (method_exists($this,$f='__construct'.$i)) { 
            call_user_func_array(array($this,$f),$a); 
        } 
    } 
 
    public function __construct2($fullname, $username) {   		
        $url = $wgCanonicalServer.$wgScriptPath.'/extensions/WorkflowTasks/includes/training/training.xml';
        $this->loadXMLSchema($url);
        set_time_limit (60*60);
        $this->fullname = $fullname;
        $this->username = $username;
        $this->create(null, $this->xml->task);
        $this->createPersonPage();
    }
   	
    public function __construct3($fullname, $username, $xmlfile) {
        $url = "file://".$xmlfile;
        $this->loadXMLSchema($url);
        set_time_limit (60*60);
        $this->fullname = $fullname;
        $this->username = $username;
        $this->create(null, $this->xml->task);
        $this->createPersonPage();
    }
   	
   	private function loadXMLSchema($url){
   		global $wgCanonicalServer;
        global $wgScriptPath;
   		$this->xml = simplexml_load_file($url);
   	}

   	private function createPersonPage(){   		
   		if(!WTTaskUtil::pageExist($this->fullname)){
   			WTFactsAPI::createPageWithCategory($this->fullname, 'Person');	
   			WTFactsAPI::setFact($this->fullname, 'Has User ID', $this->username);
   		}
   	}
	
	private function create($parentTitle, $task){
		$attr = $task->attributes();
		if($parentTitle == null && $attr->parentTitle.'')
			$parentTitle = $attr->parentTitle.'';
		$title = $this->addUsername($attr->title);
		$type = $attr->type.'';
		$progress = $attr->progress.'';		
		$start = self::getDate($attr->start);
		$target = self::getDate($attr->target);
		$docu = $attr->docu.'';
		$expertise = $attr->expertise ? explode('|', $attr->expertise.'') : array();
		$content = $this->getTaskContent($task);
		if($parentTitle == null)
			$content = self::addTrainIndex($content, $task);	
		$taskId = self::addTask($parentTitle, $title, $type, $progress, $this->fullname, $start, $target, $expertise, $docu, $content);	
		if($subtasks = $task->subtasks)
			foreach ($subtasks->children() as $child)
				$this->create($taskId, $child);		
	}
	
	private function getTaskContent($task){
		$content = $task->content->asXML();
		$content = str_replace('<content>', '', $content);
		$content = str_replace('</content>', '', $content);
		$content = str_replace("\t", '', $content);	
		return $this->addUsername($content);
	}
	
	public static function addTask($parentTitle, $subtitle, $type, $progress, $owner, $start, $target, $expertise, $docu, $content){
		$subtitle = preg_replace("/\//", " or ", $subtitle);
		if(!WTTaskUtil::pageExist($subtitle)){ // && !WTTaskUtil::taskExist($subtitle)){ //TODO check by once
			WTFactsAPI::createPageWithCategory($subtitle, 'Task');			
			self::addConent($subtitle, $content);
			if($parentTitle)
				WTFactsAPI::addFact($parentTitle, 'SubTask', $subtitle, null, true, null);				
			if($type)
				WTFactsAPI::setFact($subtitle, 'Type', $type);		
			if($type == 'low')// && $progress)
				WTFactsAPI::setFact($subtitle, 'Progress', $progress);	
			if($start)
				WTFactsAPI::setFact($subtitle, 'StartDate', $start);	
			if($target)
				WTFactsAPI::setFact($subtitle, 'TargetDate', $target);
			if($owner)
				WTFactsAPI::setFact($subtitle, 'Owner', $owner);
			foreach($expertise as $k => $e)
				WTFactsAPI::addFact($subtitle, 'Expertise', $e);	
			if($docu)	
				WTFactsAPI::setFact($subtitle, 'Documentation', $docu);	
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
	
	private function addUsername($rawText){
		return str_replace(self::$USER_PLACEHOLDER, $this->fullname, $rawText);
	}
	
	private function addTrainIndex($rawContent, $task){
		$count = 1;	
		foreach ($task->subtasks->children() as $subtask){	
			$index = $this->createTrainIndex($subtask);
			$placeholder = str_replace('#', $count, self::$TRAININDEX_PLACEHOLDER);
			$rawContent = str_replace($placeholder, $index, $rawContent);
			$count++;
		}
		return $rawContent;
	}
	
	private function createTrainIndex($task){
		return '<ul style="list-style:none;font-weight:bold;font-size:12px"><li>'.$this->createTrainIndexRec($task, 1).'</li></ul>';
	}
	
	private function createTrainIndexRec($task, $level){
		$title = $this->addUsername($task->attributes()->title);
		$items = '';
		if($subtasks = $task->subtasks)
			foreach ($subtasks->children() as $child)
				if(!$child->attributes()->noindex)
					$items .= '<li>'.$this->createTrainIndexRec($child, $level+1).'</li>';	
		$style = 'style="list-style: square outside none;"';
		if($level > 1)
			$style = 'style="list-style: square outside none;font-weight:normal; font-size:10px;"';	
		return '[['.$title.']]<ul '.$style.'>'.$items.'</ul>';
	}


	
}
