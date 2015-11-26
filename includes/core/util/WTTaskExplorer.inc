<?php

class WTTaskExplorer{
 
	private $selectedId;			 //opened task
	private $tasks = array();		 //[taskId] = tasks details
	private $roots = array();	     //[rootId1, rootId2, ...]
	private $hasChild = array(); 	 //[taskId] = [SubtaskId1, ...]
	private $hasParent = array(); 	 //[taskId] = parentId // inverted $hasChild
	private $level = array();	     //[treelevel] = [taskId1, taskId2, ...]
		
	function __construct($selectedTitle) {
		$this->selectedId = WTTaskUtil::findTaskidByTitle($selectedTitle);
	}
	
	public function getExplorer(){		
		global $wgUser;		
		$this->findAllTasks();
		$this->calcParentTasks();
		$this->calcRootTasks();	
		$this->addNotExistingTasks();
		$this->calcTaskLevel($this->roots, 0);		
		$this->calcTaskProgress();	
		$this->calcWarnings();	
		
		list($persons, $expertise) = WTPerson::findAllExpertise();	
		$uname = 'notloggedin';
		$uid = 'undefined';
		if($wgUser->isLoggedIn()){
			$uid = $wgUser->getName();
			$uname = str_replace('_',' ',WTPerson::getPersonNameFromUserId($uid));			
		}  			
		$e = array();
		$e['explorer'] = $this->generateExplorer($this->roots);
		$e['selectedId'] = $this->selectedId;			
		$e['user']['id'] = $uid;
		$e['user']['name'] = $uname;		
		$e['user']['isLoggedIn'] = $wgUser->isLoggedIn();	
		$e['user']['persons'] = $persons;	
		$e['user']['expertise'] = $expertise;		
		$e['user']["_BC_bools"] = array("isLoggedIn");
		return $e;
	}
	
	private function findAllTasks(){		
		list($this->tasks, $this->hasChild) = WTTaskUtil::findAllTasks();
	}
	
	private function calcParentTasks(){
		foreach($this->hasChild as $taskId => $subtaskIds)
			foreach($subtaskIds as $subtaskId)
				$this->hasParent[$subtaskId]= $taskId;
	}
	
	private function calcRootTasks(){		
		foreach($this->tasks as $id => $task) 		
			if(!array_key_exists($id, $this->hasParent))
				$this->roots[] = $id;
	}
	
	private function addNotExistingTasks(){
		foreach($this->hasChild as $childIds)
			foreach($childIds as $childId)
				if(!array_key_exists($childId, $this->tasks)){
					$task = array();
					$task['id'] = $childId;
					$task['title'] = str_replace('_', ' ', $childId);
					$task['exist'] = false;				
					$task['facts'] = WTTaskFacts::getInitialTaskFacts();	
					$task["_BC_bools"] = array("exist");
					$this->tasks[$childId] = $task;
				}
	}
		
	private function calcTaskLevel($taskIds, $level){
		foreach($taskIds as $taskId){	
			if(!array_key_exists($level, $this->level))						
				$this->level[$level] = array();						
			$this->level[$level][$taskId] = $taskId;						
			$this->calcTaskLevel($this->subtaskIds($taskId), $level+1);	
		}
	}
	
	private function calcTaskProgress(){
		$levels = array_keys($this->level);
		arsort($levels);
		foreach($levels as $level){
			foreach($this->level[$level] as $taskId){	
				$task = $this->tasks[$taskId];			
				if(!$task['facts']['missingparam']){								
					switch ($task['facts']['type']){
						case 'high': 	$this->calcHighlevelTask($task, $taskId); break;
						case 'medium':	$this->calcMediumlevelTask($task, $taskId); break;
						case 'low':		$this->calcLowlevelTask($task, $taskId); break;
					}
					$this->calcOverdue($taskId);
				}
			}
		}
	}
	
	private function calcLowlevelTask(&$task, $taskId){	
	}
	
	private function calcMediumlevelTask(&$task, $taskId){		
		$nr = 0;
		$p = 0;
		foreach($this->subtaskIds($taskId) as $subtaskId){
			$f = $this->tasks[$subtaskId]['facts'];				
			if(!$f['missingparam']){
				$nr++;
				$p+=$f['progress'];	
			}							
		}
		if($nr>0)
			$p = ($p/$nr);
		$this->tasks[$taskId]['facts']['progress'] = $p;
	}
	
	private function calcHighlevelTask(&$task, $taskId){			
		$start = $task['facts']['start'];
		$target = $task['facts']['target'];
		$now = new DateTime();
		$now = $now->getTimestamp();	
		/** in future */
		$p = 0;
		if($now<$start)
			$p = 0;
		/** in progress */
		if($start<$now && $now<$target)
			$p = (($now-$start)/($target-$start))*100;
		/** in past */
		if($target<$now)			
			$p = 100;		
		$this->tasks[$taskId]['facts']['progress'] = $p;	
	}
	
	private function calcOverdue(&$taskId){
		$task = $this->tasks[$taskId];
		$target = $task['facts']['target'];
		$progress = $task['facts']['progress'];
		$date = new DateTime();
		$today = $date->getTimestamp()-1;	
		$overdue = $target<$today && $progress != 100;	
		$childOverdue = false;	
		
		/** If task itself is not overdue a subtask could be 
		 *  not finished and the parent already finished 
		 *  than its also overdue */
		if(!$overdue){			
			foreach($this->subtaskIds($taskId) as $subtaskId){
				$f = $this->tasks[$subtaskId]['facts'];				
				if(!$f['missingparam']){
					$subOverdue = $f['overdue'];
					if($subOverdue || $f['childoverdue'])
						$childOverdue = true;
					if($subOverdue && $target<$today)
						$overdue = true;					
				}							
			}
		}		
		$this->tasks[$taskId]['facts']['overdue'] =  $overdue;
		$this->tasks[$taskId]['facts']['childoverdue'] =  $childOverdue;
	}
	
	private function calcWarnings(){
		$s = new DateTime("1973-01-01");
		$t = new DateTime("2037-01-01");
		$this->findWarnings($this->roots, 'high', $s->getTimestamp(), $t->getTimestamp());
		$this->calcChildWarning();
	}
	
	private function findWarnings(&$taskIds, $parentType, $parentStart, $parentTarget){
		$u = WTTaskFacts::$unknown;
		foreach ($taskIds as $taskId){
			$f = $this->tasks[$taskId]['facts'];
			$type = $f['type'];
			$start = $f['start'];
			$target = $f['target'];
			$this->initWarnings($taskId);
			if($type != $u && !self::isTypeHierachy($parentType, $type))
				$this->addWarning($taskId, 'type', $this->allcompatipleTypes($parentType));
			if($start != $u && ($parentStart-1 > $start || $start > $parentTarget+1))
				$this->addWarning($taskId, 'start', array('lower'=>$parentStart,'upper'=>$parentTarget));			
			if($target != $u && ($parentTarget+1 < $target || $target < $parentStart-1))
				$this->addWarning($taskId, 'target', array('lower'=>$parentStart,'upper'=>$parentTarget));
			$subtaskIds = $this->subtaskIds($taskId);
			$type = self::lowestType($parentType, $type);
			$start = self::latestStart($parentStart, $start);
			$target = self::erliestTarget($parentTarget, $target);
			if($taskId == $this->selectedId){
				$e = array();
				$e['type'] = $this->allcompatipleTypes($parentType);
				$e['start']['lower'] = $parentStart;
				$e['start']['upper'] = $parentTarget;
				$e['target']['lower'] = $parentStart;
				$e['target']['upper'] = $parentTarget;
				$this->tasks[$taskId]['facts']['expected'] = $e;
			}
			$this->findWarnings($subtaskIds, $type, $start, $target);
		}	
	}
	
	private static function isTypeHierachy($parentType, $childType){
		if($childType == WTTaskFacts::$unknown)
			return true;
		$types = WTTaskFacts::$tasktypes;
		return array_search($parentType, $types) <= array_search($childType, $types);
	}	
	private static function lowestType($parentType, $childType){
		$u = WTTaskFacts::$unknown;
		if($childType == $u) 
			return $parentType;
		$types = WTTaskFacts::$tasktypes;
		$pidx = array_search($parentType, $types);
		$cidx = array_search($childType, $types);
		return $types[max($pidx, $cidx)];
	}	
	private static function latestStart($parentStart, $childStart){
		$u = WTTaskFacts::$unknown;
		if($childStart == $u) 
			return $parentStart;		
		return max($parentStart, $childStart);
	}	
	private static function erliestTarget($parentTarget, $childTarget){
		$u = WTTaskFacts::$unknown;
		if($childTarget == $u) 
			return $parentTarget;		
		return min($parentTarget, $childTarget);
	}	
	private function addWarning($taskId, $warning, $expected){			
		$this->tasks[$taskId]['facts']['warnings'][$warning] =  $expected;
	}	
	private function initWarnings($taskId){
		$this->tasks[$taskId]['facts']['warnings'] = array();
	}
	private function allcompatipleTypes($type){
		$types = WTTaskFacts::$tasktypes;
		$i = array_search($type, $types);
		$compatible = array();
		for($i; $i<count($types); $i++)
			$compatible[] = $types[$i];
		return $compatible;
	}
	
	private function calcChildWarning(){
		$levels = array_keys($this->level);
		arsort($levels);
		foreach($levels as $level){			
			foreach($this->level[$level] as $taskId){				
				$warning = false;	
				foreach($this->subtaskIds($taskId) as $subtaskId){
					$f = $this->tasks[$subtaskId]['facts'];
					if(sizeof($f['warnings'])>0 || $f['childwarning'])
						$warning = true;
				}
				$this->tasks[$taskId]['facts']['childwarning'] = $warning;
			}
		}
	}
	
	
	/** Helper to get subtaskIds or empty array */
	private function subtaskIds(&$taskId){
		$subtaskIds = array();
		if(array_key_exists($taskId, $this->hasChild))
			$subtaskIds = $this->hasChild[$taskId];
		return $subtaskIds;
	}
	
	
	private function generateExplorer($taskIds){		
		$taskTree = array();
		foreach($taskIds as $taskId){	
			$task = $this->tasks[$taskId];				
			if(array_key_exists($taskId, $this->hasChild))
				$task['children'] = $this->generateExplorer($this->hasChild[$taskId]);
			$taskTree[] = $task;
		}
		return $taskTree;
	}
	


	
}
