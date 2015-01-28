<?php 
require_once 'Icon.php';

class ViewIcon extends Icon{	
	
	public function viewIcon($type){
		$this->border = 2;
		$this->size = 20; // width, height
		$this->radius = 2.5;
		$this->type = $type;	
		$this->color = '#066bbc';		
	}
	
	protected function renderSVG(){
		
		$this->lineHeight = ($this->size-$this->border*2)/4;
		$x1 = $this->border+$this->radius;
		$x2 = $this->size-$this->border;
		$y = $this->border+$this->lineHeight;
		$lines = '';
		for($i=0; $i<3; $i++){
			if($i==1 && $this->type == 'hierarchy')
				$x1 += 4;
			$lines .= '<circle cx="'.$x1.'" cy="'.$y.'" r="'.$this->radius.'" fill="'.$this->color .'" />';
			$lines .= '<line x1="'.$x1.'" y1="'.$y.'" x2="'.$x2.'" y2="'.$y.'" style="stroke:'.$this->color .';stroke-width:1" />';
			$y += $this->lineHeight;
		}
		return $lines;
	}
	
}





?>
