<?php

//TODO rename to taskQuery or taskUtil
class WTTaskUtil{
	
	public static function taskExist($taskId){		
		$query = "[[Category:Task]][[".$taskId."]] OR [[Category:Task]] [[SubTask::".$taskId."]]";
		$rawparams = array($query, "?SubTask");
	
		SMWQueryProcessor::processFunctionParams( $rawparams, $query, $params, $printouts );
		SMWQueryProcessor::addThisPrintout( $printouts, $params );		

		$m_params = SMWQueryProcessor::getProcessedParams( $params, $printouts );
		$queryobj = SMWQueryProcessor::createQuery( $query, $m_params, null, 'list', $printouts);	
		$results = smwfGetStore()->getQueryResult( $queryobj );
				
		$res = $results->getNext();
		$tasks = array();
		while($res != null) {
			if(sizeof($res) == 2)	
				while($res[0]->getNextObject())
					return true;	
			$res = $results->getNext();
		}		
		return false;
	}	
	
	public static function pageExist($title){
		$titobj = Title::newFromText($title);
		return $titobj->exists();
	}
	
	public static function findTaskidByTitle($title) {
		$query = "[[".$title."]]";
		$rawparams = array($query);	
		SMWQueryProcessor::processFunctionParams( $rawparams, $query, $params, $printouts );
		SMWQueryProcessor::addThisPrintout( $printouts, $params );
		$m_params = SMWQueryProcessor::getProcessedParams( $params, $printouts );
		$queryobj = SMWQueryProcessor::createQuery( $query, $m_params, null, 'list', $printouts);
		
		$results = smwfGetStore()->getQueryResult( $queryobj );
		$res = $results->getNext();
		if($res && sizeof($res) == 1) {		
			$tasks = $res[0];
			while($task = $tasks->getNextObject()) 
				return $task->getDbKey();			
		}	
		return null;	
	}
	
	public static function findParenttaskByTaskid($taskId){		
		$query = "[[Category:Task]] [[SubTask::".$taskId."]]";
		$rawparams = array($query);
	
		SMWQueryProcessor::processFunctionParams( $rawparams, $query, $params, $printouts );
		SMWQueryProcessor::addThisPrintout( $printouts, $params );		

		$m_params = SMWQueryProcessor::getProcessedParams( $params, $printouts );
		$queryobj = SMWQueryProcessor::createQuery( $query, $m_params, null, 'list', $printouts);	
		$results = smwfGetStore()->getQueryResult( $queryobj );
				
		$res = $results->getNext();
		while($res != null) {
			if(sizeof($res) == 1)
				while($maintask = $res[0]->getNextObject())				
					return $maintask->getDbKey();							
			$res = $results->getNext();
		}		
		return null;		
	}
	
	public static function findChildtasksByTaskid($taskId){
		//TODO write real query and return only childs
		list($tasks, $hasChild) = self::findAllTasks();		
		return $hasChild;
	}
	
	public static function findAllTasks(){
		$tasks = array();		 //[taskId] = tasks details
		$hasChild = array(); 	 //[taskId] = [SubtaskId1, ...]
	
		$query = "[[Category:Task]]";
		$rawparams = array($query, "?SubTask", "?Type", "?Expertise", "?StartDate", "?TargetDate", "?Progress", "?Owner", "?Participants", "limit=9999");
	
		SMWQueryProcessor::processFunctionParams( $rawparams, $query, $params, $printouts );
		SMWQueryProcessor::addThisPrintout( $printouts, $params );		

		$m_params = SMWQueryProcessor::getProcessedParams( $params, $printouts );
		$queryobj = SMWQueryProcessor::createQuery( $query, $m_params, null, 'list', $printouts);	
		$results = smwfGetStore()->getQueryResult( $queryobj );
				
		$res = $results->getNext();
		while($res != null) {
			if(sizeof($res) == 9) {
				$subs = $res[1];
				$subtasks = array();
				while($sub = $subs->getNextObject()) {
					$subtitle = $sub->getTitle();
					$subtasks[] = $subtitle->getDbKey();
				}
				while($maintask = $res[0]->getNextObject()) {					
					$details = array();
					$details['id'] = $maintask->getDbKey();
					$details['title'] = $maintask->getText();
					$details['exist'] = true;
					$details['facts'] = WTTaskFacts::getExistingTaskFacts($res);
            		$details["_BC_bools"] = array("exist");
					$tasks[$maintask->getDbKey()] = $details;
					if(sizeof($subtasks)>0)
						$hasChild[$maintask->getDbKey()] = $subtasks;
				}
			}			
			$res = $results->getNext();
		}
		return array(&$tasks, &$hasChild);		
	}
	

	
	
	
}
