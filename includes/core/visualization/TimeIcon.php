<?php
require_once 'MetaIcon.php';

abstract class TimeIcon extends MetaIcon{

	private $lineWidth = 2;
	
   	public function __construct($fade, $warning) {
       	parent::__construct($fade, $warning);
   	}
	
	protected abstract function isLeft();
	
	protected function renderSvg(){	
		$svg = $this->vertivalElement();
		$svg .= $this->horizontalElement();					
		return $svg;
	}
	
	private function vertivalElement(){
		$vwidth = $this->lineWidth; 
		$vheight = $this->innerSize()*.75;
		$vx = $this->border;
		if(!$this->isLeft())
			$vx += $this->innerSize();
		$vy = $this->innerSize()*.25/2;
		return  '<rect x="'.$vx.'" y="'.$vy.'" width="'.$vwidth.'" height="'.$vheight.'" style="fill:'.$this->lightColor().'" />';		
	}
	
	private function horizontalElement(){
		$hx = $this->border;
		$hy = $this->size/2-$this->lineWidth;
		$hwidth = $this->innerSize();
		$hheight = $this->lineWidth; 
		return '<rect x="'.$hx.'" y="'.$hy.'" width="'.$hwidth.'" height="'.$hheight.'" style="fill:'.$this->lightColor().'" />';	
	}
}