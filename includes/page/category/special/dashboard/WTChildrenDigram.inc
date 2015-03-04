<?php

class WTChildrenDigram{	
	
	
	private $childrenPerTask = array();
	private $childrensum = 0;
	private $childcount = 0;
	private $avgNrOfChilds = 0;
		
	public function calculateChildren(&$tasks) {
		
		foreach($tasks as $k => $task){			
			$nrOfChilds = 0;
			if(array_key_exists('children', $task))
				$nrOfChilds = sizeof($task['children']);			
			if(!array_key_exists($nrOfChilds, $this->childrenPerTask)) 
				$this->childrenPerTask[$nrOfChilds] = array('metalevel' => 0, 'contentlevel' => 0);
			if(WTDashboardAnalyze::isOnMetalevel($task))
				$this->childrenPerTask[$nrOfChilds]['metalevel']++;
			else 
				$this->childrenPerTask[$nrOfChilds]['contentlevel']++;
			$this->childrensum += $nrOfChilds;
			$this->childcount++;
		}
		$this->avgNrOfChilds = $this->childrensum/$this->childcount;
	}
	


	public function toJson() {
		$children = array();
		foreach($this->childrenPerTask as $nrOfchilds => $data)
			$children[] = array('nrofchilds'=> $nrOfchilds, 'metalevel'=> $data['metalevel'], 'contentlevel'=>$data['contentlevel']);
		return array( 'avg' => $this->avgNrOfChilds, 'childdata' => $children);		
	}	
}
