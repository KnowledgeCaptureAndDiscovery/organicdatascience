<?php

class WTDashboardAnalyze{
	
	public static $u = 'unknown';
	private $tasks = array();
	
	public function analyis(){
		$exp = new WTTaskExplorer("");
		$exp = $exp->getExplorer();
		$exp = $exp['explorer'];
		foreach($exp as $key => $roottask)
			$this->recinit($roottask, false, 0);
	}	
	
	private function recinit($task, $parentId, $level){
		$t = $task;
		unset($t['children']);
		$t['level'] = $level;		
		$this->tasks[] = $t;
		if(array_key_exists('children', $task) && sizeof($task['children']) >0)
			foreach ($task['children'] as $key => $subtask)
				$this->recinit($subtask, $task['id'], $level+1);
	}
	

	/**
	 * Task Statistics
	 * Groups tasks with same amout of persons and counts them!
	 */
	private function personsPerTask() {
		$pptask = array(); //[nr of persns] = #tasks
		$nrOfTasks = sizeof($this->tasks);
		foreach ($this->tasks as $key => $t){
			$nrPersons = sizeof($this->taskPersons($t));
			if(!array_key_exists($nrPersons, $pptask))
				$pptask[$nrPersons] = array('count' => 0);
			$pptask[$nrPersons]['count']++;
			$pptask[$nrPersons]['percentage'] = $pptask[$nrPersons]['count']/$nrOfTasks;
		}	
	}
		
	
	/**
	 * Returns an array with the task owner and participants
	 * @param person names as array
	 */
	public static function taskPersons($task) {
		$ps = array();	
		if($task['facts']['owner'] != self::$u)
			$ps[] = $task['facts']['owner'];	
		foreach ($task['facts']['participants'] as $k => $p) 
			if($p != self::$u)
				$ps[] = $p;
		return $ps;
	}
	
	
	public function toJson(){
		$graph = new WTCollaborationGraph();
		$graph->calculateEdges($this->tasks);		
//		var_dump(json_encode($graph->toJson()));
		return array('collaborationgraph' => $graph->toJson());
	}
	
	

	

	
	

	
	
}
