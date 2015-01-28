<?php 
require_once  'Icon.php';

class AlertIcon extends Icon{
	
	public function alertIcon($alert){
		$this->size = 18; // width, height
		$this->alert = $alert;			
	}
		
	protected function renderSvg(){		
		$r = $this->size/3;
		$x = $this->size/2;
		$y = $r;
		$bell = '<circle cx="'.$x.'" cy="'.$y.'" r="'.$r.'" fill="'.$this->color().'" />';
		$x = ($this->size/3)/2;
		$bell .= '<rect x="'.$x.'" y="'.$y.'" width="'.($r*2).'" height="'.$r.'" fill="'.$this->color().'"/>';
		$x = $this->size/2;
		$y += $r;
		$r = $this->size/2;
		$bell .= '<ellipse cx="'.$x.'" cy="'.$y.'" rx="'.$r.'" ry="'.($r/2).'" fill="'.$this->color().'"/>';		
		return $bell;
	}
	
	private function color(){
		switch ($this->alert) {
			case true: 	return '#ba0000';
			case false: return '#d9d9d9';
			default: throw new Exception('Alert must be a boolean value');
		}
	}
	
}





?>
