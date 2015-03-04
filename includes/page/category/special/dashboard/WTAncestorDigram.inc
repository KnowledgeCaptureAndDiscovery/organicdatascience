<?php

class WTAncestorDigram{	
	
	
	private $tasksPerLevel = array();
	private $levelsum = 0;
	private $avgLevelsum = 0;
		
	public function calculateAncestors(&$tasks) {		
		foreach($tasks as $k => $task) {
			$this->levelsum += $task['level']; 
			if(array_key_exists($task['level'], $this->tasksPerLevel))
				$this->tasksPerLevel[$task['level']]++;
			else
				$this->tasksPerLevel[$task['level']] = 1;		
		}
		$this->avgLevel = $this->levelsum/sizeof($tasks);	
	}

	public function toJson() {
		$levels = array();
		foreach($this->tasksPerLevel as $level => $nrOfTasks){
			$levels[] = array('level'=>$level, 'nrOfTasks'=>$nrOfTasks);
		}
		return array( 'avg' => $this->avgLevelsum, 'leveldata' => $levels);		
	}	
}
