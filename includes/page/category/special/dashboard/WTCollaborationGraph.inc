<?php

class WTCollaborationGraph{	
	
	
	private $edges = array();
		
	public function calculateEdges(&$tasks) {
		foreach ($tasks as $k => $t){
			$persons  = WTDashboardAnalyze::taskPersons($t);
			asort($persons); 
			if(sizeof($persons)>1){
				for($i1 = 0; $i1 < sizeof($persons)-1; $i1++){ 
					$p1 = $persons[$i1];
					for($i2 = $i1+1; $i2 < sizeof($persons); $i2++){ 
						$p2 = $persons[$i2];
						if($p1 != $p2){
							$edgeId = $p1.'_'.$p2;								
							if(!array_key_exists($edgeId, $this->edges))
								$this->edges[$edgeId] = array('persons' => array($p1, $p2), 'count' => 0);
							$this->edges[$edgeId]['count']++;
						}
					}
				}
			}
		}	
	}

	public function toJson() {
		$persons = array();
		$nodes = array();
		$edges = array();
		foreach ($this->edges as $key => $e){
			$s = $e['persons'][0];
			$t = $e['persons'][1];
			$persons[$s] = true;
			$persons[$t] = true;
			$edges[] = array('id' => $key, 'source' => $s, 'target' => $t, 'weight' => $e['count']);
		}
		foreach ($persons as $key => $node)
			$nodes[] = array('id' => $key, 'label' => $key, 'weight'=>1);
		return $graph = array( 'nodes' => $nodes, 'edges' => $edges);		
	}
	
	
	
}
